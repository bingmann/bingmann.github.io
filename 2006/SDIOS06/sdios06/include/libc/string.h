#ifndef STRING_H_
#define STRING_H_

#define __need_size_t
#define __need_NULL
#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef LIBC_INLINE
# define LIBC_INLINE extern inline
#endif

LIBC_INLINE void* memset(void * dest, int c, size_t size)
{
	char * d = (char*) dest;
	
	for ( ;size--; )
		*( d++ ) = c;
		
	return dest;
}

LIBC_INLINE void* memcpy(void* dest, const void* src, size_t size)
{
	char* d = (char*) dest;
	const char* s = (const char*) src;
	
	while(size--)
		*( d++ ) = *( s++ );
		
	return dest;
}

LIBC_INLINE void* memmove(void* dest, const void* src, size_t size)
{
	char* d = (char*) dest;
	const char* s = (const char*) src;
	
	if(src == dest)
		return dest;

	if(dest < src) {
		while(size--)
			*d++ = *s++;
	} else {
		s += size - 1;
		d += size - 1;
		while(size--)
			*d-- = *s--;
	}
	
	return dest;	
}

LIBC_INLINE int memcmp(const void* p1, const void* p2, size_t size)
{
	const char* s1 = (const char*) p1;
	const char* s2 = (const char*) p2;
	
	for( ; size > 0; size--) {
		if(*s1 != *s2)
			return *s1 - *s2;
		s1++;
		s2++;
	}

	return 0;
}

LIBC_INLINE char* strncpy(char* dest, const char* src, size_t max)
{
	size_t i;
	for(i = 0; i < max; ++i) {
		if ( !( *( dest++ ) = *( src++ ) ) )
			break;
	}
	
	// pad with zeros (ansi requires this)
	for( ; i < max; ++i) {
		*( dest++ ) = '\0';
	};
	
	return dest;
}

LIBC_INLINE int strncmp(const char* dest, const char* src, size_t max)
{
	while ( ( max ) && ( *dest ) ) {
		if ( *dest != *src )
			break;
		dest++;
		src++;
		max--;
	}
	
	return (max) ? *dest - *src : 0;
}

LIBC_INLINE int strcmp(const char* dest, const char* src)
{
	while(*dest != 0 && *src == *dest) {
		dest++;
		src++;
	}
	
	return *dest - *src;
}
    
LIBC_INLINE size_t strlen(const char* src)
{
	const char* start = src;
	while ( *src ) 
		src++;
	return src - start;
}
    
LIBC_INLINE char* strchr(const char* s, int c)
{
	const char *src = s;
	
	while(*src && *src != c)
		src++;
		
	if(*src != c) {
		src = NULL;
	}
	
	return (char*) src;
}

LIBC_INLINE char* strrchr(const char* s, int c)
{
	const char *lastocc = NULL;
	
	while(*s != 0) {
		if(*s == c)
			lastocc = s;
		s++;
	}
		
	return (char*) lastocc;
}

LIBC_INLINE char* strcpy(char* dest, const char* src)
{
	char* start = dest;
	while ((*dest++ = *src++))
		;
	
	return start;
}

LIBC_INLINE char* strcat(char* d, const char* src)
{
	char* dest = d;
	d += strlen(d);
	strcpy(d, src);

	return dest;
}

char* strdup(const char* str);
char* strerror(int errnum);
char* strstr(const char* haystack, const char* needle);

#ifdef __cplusplus
}
#endif

#endif
