#ifndef _CTYPE_H
#define _CTYPE_H

#include <sys/cdefs.h>

__BEGIN_DECLS

// can later be set to nothing to create extern definitions in a .c
#ifndef LIBC_INLINE
# define LIBC_INLINE extern inline
#endif

LIBC_INLINE int toupper(int c)
{
	if(c >= 'a' && c <= 'z')
		c -= 'A' - 'a';
		
	return c;
}

LIBC_INLINE int tolower(int c)
{
	if(c >= 'A' && c <= 'Z')
		c -= 'a' - 'A';

	return c;
}

LIBC_INLINE int isalnum(int ch)
{
	return (unsigned int)((ch | 0x20) - 'a') < 26u  ||
           (unsigned int)( ch         - '0') < 10u;
}

LIBC_INLINE int isspace(int c)
{
	if(c == ' ' || c == '\t' || c == '\n' || c == '\r')
		return 1;

	return 0;
}
	
LIBC_INLINE int isdigit(int c)
{
	if(c >= '0' && c <= '9')
		return 1;
			
	return 0;
}

LIBC_INLINE int isxdigit(int c)
{
	if( (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F') )
		return 1;
			
	return 0;
}

__END_DECLS

#endif

