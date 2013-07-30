#include <config.h>

#include <stdlib.h>
#include <stdio.h>

#include <l4/types.h>
#include <l4/thread.h>
#include <l4/kdebug.h>

#include <idl4glue.h>
#include <if/ifmemory.h>

#include <sdi/panic.h>

typedef void (*function)(void);

#define NUM_ATEXIT	32

extern "C"
{

static function __atexitlist[NUM_ATEXIT];
static int __atexit_counter = 0;

int atexit(void (*function)(void))
{
	if (__atexit_counter < NUM_ATEXIT)
	{
		__atexitlist[__atexit_counter] = function;
		++__atexit_counter;
		return 0;
	}
	return -1;
}

void exit(int retval)
{
	// call atexit functions

	int i = __atexit_counter;
	while(i) {
		__atexitlist[--i]();
	}

	// send KillMe to pager

	CORBA_Environment env (idl4_default_environment);

	IF_MEMORY_KillMe(L4_Pager(), retval, &env);
	if (env._major != CORBA_NO_EXCEPTION)
	{
		panic("__exit: pager call KillMe failed");
	}

	panic("__exit: KillMe didn't work...");

	L4_Stop(L4_Myself());
		
	panic("__exit: Stop(Myself) didn't work...");
	while(true) ;
}

void abort()
{
	while(true)
		L4_KDB_Enter("abort called");
}

}
