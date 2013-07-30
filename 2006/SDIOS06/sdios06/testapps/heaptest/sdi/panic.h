#include <stdio.h>
#include <stdlib.h>

static inline void panic(const char* msg)
{
	printf("PANIC: %s\n", msg);
	exit(1);
}	
