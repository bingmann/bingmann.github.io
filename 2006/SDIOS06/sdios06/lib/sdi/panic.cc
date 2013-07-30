/*
 * $Id$
 */
#include <config.h>

#include <l4/kdebug.h>

#include <stdio.h>
#include <stdlib.h>

#include <sdi/panic.h>

extern "C" {

	/* Panic Managment */
	void __panic(const char* message, const char* function, const char* file,
	             unsigned int line)
	{
		printf("\n\nPanic at %s\n", function);
		printf("%s:%u\n", file, line);
		printf("%s\n", message);
		while(true)
			L4_KDB_Enter("panic");
	}

}
