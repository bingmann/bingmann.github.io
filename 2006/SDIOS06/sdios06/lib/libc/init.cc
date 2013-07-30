#include <config.h>

extern "C"
{
	
void __init_heap();
void __init_stdstreams();
	
/**
 * This should be called by crt0.s
 */
void __libc_init()
{
	__init_heap();
	__init_stdstreams();	
}	
	
}
