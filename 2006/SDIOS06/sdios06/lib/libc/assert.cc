/*
 * $Id$
 */
#include <config.h>

#include <assert.h>
#include <stdio.h>

#include <l4/kdebug.h>

extern "C" {

void __assert_fail(const char* assertion, const char* file,
                   unsigned int line, const char* function)
{
	fprintf(stddebug, "\n");
	fprintf(stddebug, "Assertion failed: %s\n", assertion);
	fprintf(stddebug, "%s\n", function);
	fprintf(stddebug, "%s:%u\n", file, line); 
	
	L4_KDB_Enter("assertion failed");
}

}
