#include <stdio.h>

void L4_KDB_Enter(const char* msg)
{
	printf("PANIC: %s\n", msg);
	abort();
}
