/*
 * $id$
 */
#ifndef STDLIB_H_
#define STDLIB_H_

#include <sys/cdefs.h>

#define __need_size_t
#define __need_NULL
#include <stddef.h>

__BEGIN_DECLS

#define RAND_MAX        ((1<<31) -2)

#ifndef LIBC_INLINE
# define LIBC_INLINE extern inline
#endif
	
extern void exit(int retval) __attribute__((noreturn));

void abort() __attribute__((noreturn));

int atexit(void (*function)(void));
        
#define alloca(size)   __builtin_alloca(size)

void* malloc(size_t size);
void* calloc(size_t nmemb, size_t size);
void free(void* ptr);
void* realloc(void* ptr, size_t size);

void* malloc_debug(size_t size, const char* function, const char* file, unsigned int line);
void* calloc_debug(size_t nmemb, size_t size, const char* function, const char* file, unsigned int line);
void free_debug(void* ptr, const char* function, const char* file, unsigned int line);
void* realloc_debug(void* ptr, size_t size, const char* function, const char* file, unsigned int line);

#ifdef DEBUG_MALLOC

#define malloc(size)		malloc_debug((size), __PRETTY_FUNCTION__, __FILE__, __LINE__)
#define calloc(nmemb, size)	calloc_debug((nmemb), (size), __PRETTY_FUNCTION__, __FILE__, __LINE__)
#define free(ptr)			free_debug((ptr), __PRETTY_FUNCTION__, __FILE__, __LINE__)
#define realloc(ptr, size)	realloc_debug((ptr), (size), __PRETTY_FUNCTION__, __FILE__, __LINE__)

#endif

int rand();
int rand_r(unsigned int* seedp);
void srand(unsigned int seed);

int atoi(const char* str);
double atof(const char* str);
	
long int strtol(const char *nptr, char **endptr, int base) __THROW;
unsigned long int strtoul(const char *nptr, char **endptr, int base) __THROW;

double strtod(const char* str, char** end);

LIBC_INLINE int abs(int v)
{
	if(v < 0)
		return -v;
	return v;
}

// environment functions
char *getenv(const char *name) __THROW;
int clearenv(void) __THROW;
int putenv(const char *string) __THROW;
int setenv(const char *name, const char *value, int overwrite) __THROW;
int unsetenv(const char *name) __THROW;

__END_DECLS

#endif
