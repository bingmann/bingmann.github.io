
// disable inlining in header files, thus this file contains extern symbols
// like memset
#define LIBC_INLINE

#include <config.h>

#include <string.h>
#include <stdlib.h>

extern "C"
{
	char* strdup(const char* str)	
	{
		size_t len = strlen(str) + 1;
		char* result = (char*) malloc(len);
		memcpy(result, str, len);
		
		return result;
	}
}
