#include <config.h>

#include <new>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <limits.h>
#include <unistd.h>

#include <l4io.h>

#include <sdi/log.h>
#include <sdi/panic.h>
#include <sdi/locator.h>
#include <sdi/util.h>

#include <idl4glue.h>
#include <if/iflocator.h>
#include <if/iffile.h>

#include "cwd.h"

// some calls will output what they currently do to the log
//#define TRACE_CALLS
#define SHOW_ERRORS

struct _IO_FILE
{
	L4_ThreadId_t fileserver;
	objectid_t file_handle;
	fpos_t pos;		// the current file position (bytes inside the buffer are not counted here)
	int error;
	char buffer[1024];
	size_t bufpos;	// read position inside the buffer
	size_t buflen;	// position inside the buffer which contains no valid data anymore
};

extern "C" {
	
// TODO
FILE debug;
FILE* stddebug = &debug; 
FILE* stderr = &debug;
FILE* stdout = &debug;
FILE* stdin = &debug;

FILE* fopen(const char* filename, const char* mode)
{
	//enum flags = { WRITE, READ };

	L4_ThreadId_t threadid;		
	objectid_t filehandle;
	if(__resolve_path(filename, IF_FILE_ID, &threadid, &filehandle) != 0) {
		//if(mode[0] == 'w') {	
		return NULL;
	}
	
	FILE* file = new FILE();
	file->fileserver = threadid;
	file->file_handle = filehandle;
	
	file->pos = 0;
	file->error = 0;
	file->bufpos = 0;
	file->buflen = 0;
	
	return file;
}

FILE* fdopen(int fd, const char* mode)
{
	printf("fdopen not supported\n");
	return NULL;
}

int fclose(FILE* stream)
{
	assert(stream != NULL);
	delete stream;
	
	return 0;
}

int fflush(FILE* stream)
{
	return 0;
}

int fgetc(FILE* stream)
{
	int c = 0;
	size_t rs;

	rs = fread(&c, 1, 1, stream);
	if(rs == 0)
		return EOF;

	return c;
}

char *fgets(char *s, int size, FILE *stream)
{
	char *orig = s;
	int l;
	for (l = size; l > 1; ) {
		int c = fgetc(stream);
		if (c == EOF) break;
		*s = c;
		++s;
		--l;
		if (c == '\n') break;
	}
	if (l == size) {
		printf("fgets: EOF\n");
		return 0;
	}

	*s = 0;

#ifdef TRACE_CALLS
	printf("fgets: %s\n", orig);
#endif
	return orig;
}

int fputc(int c, FILE* stream)
{
	unsigned char ch = static_cast<char> (c);
	size_t res = fwrite(&ch, 1, 1, stream);
	if(res != 1)
		return EOF;
		
	return ch;
}

int putchar(int c)
{
	return fputc(c, stdout);
}

int fputs(const char* str, FILE* stream)
{
	return fprintf(stream, "%s\n", str);
}

int puts(char* str)
{
	return fputs(str, stdout);
}

static void refill_buffer(FILE* stream)
{
	buffer_t buffer;
	buffer._buffer = reinterpret_cast<CORBA_char*>(stream->buffer);
	buffer._length = 0;
	buffer._maximum = sizeof(stream->buffer);

	CORBA_Environment env (idl4_default_environment);	
	IF_FILE_Read(stream->fileserver, stream->file_handle, stream->pos, buffer._maximum, &buffer, &env);
	if(env._major != CORBA_NO_EXCEPTION) {
		LogMessage("Exception while sending read request...");
		return;
	}
#ifdef TRACE_CALLS
	LogMessage("Read (refill buffer), buffmax %u bufflen %u buffptr %p", buffer._maximum, buffer._length, buffer._buffer);
#endif
	stream->bufpos = 0;
	stream->buflen = buffer._length; 
}

size_t fread(void* ptr, size_t size, size_t nmemb, FILE* stream)
{
	CORBA_char* dest = reinterpret_cast<CORBA_char*> (ptr);
	const size_t MAX_BUFSIZE = 8192;
	const size_t readsize = size * nmemb;
	size_t todosize = readsize;			  // bytes that still need to be read

	assert(stream != NULL);
	
#ifdef TRACE_CALLS
	LogMessage("Read, Members %lu of size %lu (=%lu), buffptr %p", nmemb, size, readsize, ptr);
#endif

	// TODO handle debug stream?
	
	size_t buflen = stream->buflen - stream->bufpos;
	if(todosize <= buflen) {
		memcpy(ptr, stream->buffer + stream->bufpos, todosize);
		stream->bufpos += todosize;
		stream->pos += todosize;
		return nmemb;
	}
	
	// copy remainig data in buffer into dest
	if(buflen > 0) {
		memcpy(dest, stream->buffer + stream->bufpos, buflen);
		dest += buflen;
		todosize -= buflen;
		stream->pos += buflen;
		stream->bufpos = 0;
		stream->buflen = 0;
	}
	
	// read into buffer if we are reading a small chunk, for bigger chunks
	// we directly read into the target memory
	if(todosize < sizeof(stream->buffer) / 2) {
		refill_buffer(stream);
		
		buflen = stream->buflen - stream->bufpos;
		size_t copysize = min(buflen, readsize);
		memcpy(dest, stream->buffer + stream->bufpos, copysize);
		stream->pos += copysize;
		stream->bufpos += copysize;
		todosize -= copysize;
		dest += copysize;
	} else {
		// read directly into target memory
		while(todosize > 0)
		{
			size_t thissize;
			if(todosize > MAX_BUFSIZE) {
				thissize = MAX_BUFSIZE;
			} else {
				thissize = todosize;
			}
			
			buffer_t buffer;
			buffer._buffer = dest;
			buffer._length = 0;
			buffer._maximum = thissize;
		
			CORBA_Environment env (idl4_default_environment);	
			IF_FILE_Read(stream->fileserver, stream->file_handle, stream->pos, thissize, &buffer, &env);
			if(env._major != CORBA_NO_EXCEPTION) {
				LogMessage("Exception while sending read request...");
				break;
			}
		
			stream->pos += buffer._length;
			dest += buffer._length;
			todosize -= buffer._length;
	
			if (buffer._length < thissize)
				break;
		}
	}

	if(todosize > 0) {
		stream->error = EOF;
		
		// put back elements that are not completely loaded
		size_t donesize = readsize - todosize;
		size_t elemsdone = donesize / size;
		
		size_t putback = donesize - elemsdone * size;
		if(putback > 0) {
			assert(putback < sizeof(stream->buffer));
		
			memcpy(stream->buffer, dest - putback, putback);
			stream->bufpos = 0;
			stream->buflen = putback; 
			stream->pos -= putback;
		}
		
		return elemsdone;
	}
	
	return nmemb;
}

size_t fwrite(const void* ptr, size_t size, size_t nmemb, FILE* stream)
{
	assert(stream != NULL);
	
#ifdef TRACE_CALLS
	LogMessage("FWrite of %u * %u bytes", size, nmemb);
#endif

	// is it our special comport stream?
	if(stream == &debug) {
		const size_t writesize = size * nmemb;
		
		const char* p = static_cast<const char*> (ptr);
		for(size_t i = 0; i < writesize; ++i) {
			l4_putchar(p[i]);
		}
		
		return nmemb;
	}
	
	// TODO buffering...
	
	const size_t MAX_BUFSIZE = 8192;
	const size_t writesize = size * nmemb;
	size_t todosize = writesize;			  // bytes that still need to be written
	CORBA_char* src = reinterpret_cast<CORBA_char*> (const_cast<void*> (ptr)); 

	while(todosize > 0) {
		size_t thissize = todosize;
		if(thissize > MAX_BUFSIZE);
			thissize = MAX_BUFSIZE;
		
		buffer_t buffer;
		buffer._buffer = src;
		buffer._length = writesize;
		buffer._maximum = writesize;
		
		CORBA_Environment env (idl4_default_environment);
		size_t byteswritten = 0;
		IF_FILE_Write(stream->fileserver, stream->file_handle, stream->pos, &byteswritten, &buffer, &env);
		if(env._major != CORBA_NO_EXCEPTION) {
			LogMessage("Exception while sending write request...");
			break;
		}
		
		stream->pos += byteswritten;
		src += byteswritten;
		todosize -= byteswritten;
	
		if(byteswritten < thissize)
			break;
	}
	
	if(todosize > 0) {
		stream->error = EOF;
		
		size_t donesize = writesize - todosize;
		size_t elemsdone = donesize / size;
		
		// need a way to handle half-written elements...
				
		return elemsdone;
	}
	
	return nmemb;
}

int fseek(FILE* stream, long int offset, int whence)
{
	switch(whence) {
	case SEEK_CUR:
		stream->pos += offset;
		break;
	case SEEK_SET:
		stream->pos = offset;
		break;
	case SEEK_END: {
		CORBA_Environment env (idl4_default_environment);
		idlsize_t size = 0;
		IF_FILE_GetFileSize(stream->fileserver, stream->file_handle, &size, &env);
		if(env._major != CORBA_NO_EXCEPTION) {
			LogMessage("Exception while sending GetFileSize request...");
			return -1;
		}
		stream->pos = size + offset;
		break;
	}
	default:
		printf("Invalid whence specified for fseek\n");
		return -1;
	}
#ifdef TRACE_CALLS
	LogMessage("Seeking to %d - offset %ld ResultPos: %lld", whence, offset, stream->pos);
#endif

	// invalidate buffer
	stream->bufpos = 0;
	stream->buflen = 0;
	
	return 0;
}

long int ftell(FILE* stream)
{
	return stream->pos;
}

void rewind(FILE* stream)
{
	fseek(stream, 0, SEEK_SET);
	clearerr(stream);
}

void clearerr(FILE* stream)
{
	stream->error = 0;
}

int feof(FILE* stream)
{
	return stream->error != 0;
}

int ferror(FILE* stream)
{
	return stream->error;
}

int sprintf(char* str, const char* format, ...)
{
	va_list args;

	va_start(args, format);
	int res = vsnprintf(str, ULONG_MAX, format, args);
	va_end(args);

	return res;
}

void perror(const char* msg)
{
	printf("%s: (unknown)", msg);	
}

int ungetc(int c, FILE* stream)
{
	if (stream->pos > 0) {
		stream->pos--;
		if(stream->bufpos > 0) {
			stream->bufpos--;
			assert(stream->buffer[stream->bufpos] == c);
		}	
	} else {
		printf("Warning: ungetc at start of file\n");
		return EOF;
	}

	// printf("ungetc not implemented\n");
	return c;
}

int remove(const char* pathname)
{
	printf("remove not implemented\n");
	return -1;
}

void __init_stdstreams()
{
	const char* stdoutpath = getenv("STDOUT");
	if(stdoutpath != NULL) {
		stdout = fopen(stdoutpath, "w");
		if(stdout == NULL) {
			fprintf(stddebug, "Warning: Couldn't open stdout device '%s'\n", stdoutpath);
			stdout = stddebug;
		} 
	}
	
	const char* stderrpath = getenv("STDERR");
	if(stderrpath != NULL) {
		stderr = fopen(stderrpath, "w");
		if(stderr == NULL) {
			fprintf(stddebug, "Warning: Couldn't open stderr device '%s'\n", stderrpath);
			stderr = stddebug;
		} 
	}

	const char* stdinpath = getenv("STDIN");
	if(stdinpath != NULL) {
		stdin = fopen(stdinpath, "r");
		if(stdin == NULL) {
			fprintf(stddebug, "Warning: Couldn't open stdin device '%s'\n", stdinpath);
			stdin = stddebug;
		} 
	}
}

}
