#ifndef STDIO_H_
#define STDIO_H_

#include <stdint.h>
#define __need_size_t
#define __need_NULL
#include <stddef.h>
#include <stdarg.h>

typedef int64_t fpos_t;
typedef struct _IO_FILE FILE;

#define	EOF				((fpos_t) -1)
#define FOPEN_MAX		0xffff
#define FILENAME_MAX 	1024
#define L_tmpnam		FILENAME_MAX
#define TMP_MAX			FILENAME_MAX

#define SEEK_SET		0
#define SEEK_CUR		1
#define SEEK_END		2

/* Standard streams.  */
extern struct _IO_FILE *stdin;          /* Standard input stream.  */
extern struct _IO_FILE *stdout;         /* Standard output stream.  */
extern struct _IO_FILE *stderr;         /* Standard error output stream.  */
extern struct _IO_FILE *stddebug;       /* Standard debug output stream.  */

/* C89/C99 say they're macros.  Make them happy.  */
#define stdin stdin
#define stdout stdout
#define stderr stderr

#ifdef __cplusplus
extern "C" {
#endif

	int remove(const char* filename);
	//int rename(const char* old, const char* newname);
	//FILE* tmpfile();
	//char* tmpnam(char* s);
	
	int fclose(FILE *stream);
	int fflush(FILE *stream);
	FILE* fopen(const char* filename, const char* mode);
	FILE* fdopen(int fd, const char* mode);
	//FILE *freopen(const char* filename, const char* mode, FILE* stream);
	//void setbuf(FILE* stream, char* buf);
	//int setvbuf(FILE* stream, char* buf, int mode, size_t size);
	int fprintf(FILE* stream, const char* format, ...)
		__attribute__((format(printf, 2, 3)));
	int fscanf(FILE * stream, const char* format, ...);
	int vfprintf(FILE *stream, const char *format, va_list ap);
	int printf(const char* format, ...)
		__attribute__((format(printf, 1, 2)));;
	int vprintf(const char* format, va_list ap);
	int sprintf(char* str, const char* format, ...);
	int snprintf(char* str, size_t size, const char* format, ...);	
	int vsnprintf(char* str, size_t size, const char* format, va_list ap);	
	int fgetc(FILE *stream);
	#define getc(stream) fgetc(stream)
	char *fgets(char* s, int n, FILE* stream);
	int fputc(int c, FILE *stream);
	#define putc(stream, c) fputc(c, stream)
	int putchar(int c);
	int fputs(const char* s, FILE* stream);
	int puts(char* str);
	char* fgets(char* s, int size, FILE* stream);
	size_t fread(void* ptr, size_t size, size_t nmemb, FILE* stream);
	size_t fwrite(const void* ptr, size_t size, size_t nmemb, FILE* stream);
	//int fgetpos(FILE* stream, fpos_t* pos);
	int fseek(FILE *stream, long int offset, int whence);
	long int ftell(FILE *stream);
	void rewind(FILE *stream);
	void clearerr(FILE *stream);
	int feof(FILE *stream);
	int ferror(FILE *stream);
	void perror(const char* msg);
	int ungetc(int c, FILE* stream);
	
#ifdef __cplusplus
}
#endif

#endif /*STDIO_H_*/
