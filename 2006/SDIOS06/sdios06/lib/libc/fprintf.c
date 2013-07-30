
#include <stdarg.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>

#include "dietstdio.h"

static int __fwrite(void*ptr, size_t nmemb, FILE* f)
{
	return fwrite(ptr,1,nmemb,f);
}

int vfprintf(FILE *stream, const char *format, va_list arg_ptr)
{
	struct arg_printf ap = { stream, (int(*)(void*,size_t,void*)) __fwrite };
	return __v_printf(&ap,format,arg_ptr);
}

int fprintf(FILE *f,const char *format,...) {
	int n;
	va_list arg_ptr;
	va_start(arg_ptr,format);
	n=vfprintf(f,format,arg_ptr);
	va_end(arg_ptr);
	return n;
}

int vprintf(const char* format, va_list arg_ptr) {
	return vfprintf(stdout, format, arg_ptr);
}

int printf(const char* format, ...) {
	int n;
	va_list arg_ptr;
	va_start(arg_ptr,format);
	n = vprintf(format, arg_ptr);
	va_end(arg_ptr);
	return n;
}

