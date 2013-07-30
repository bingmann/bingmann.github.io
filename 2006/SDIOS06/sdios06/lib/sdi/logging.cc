/*
 * $Id$
 */
#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <sdi/log.h>
#include <sdi/locator.h>

#include <idl4glue.h>
#include <if/iflocator.h>
#include <if/iflogging.h>

namespace
{
	L4_ThreadId_t logger = L4_GlobalId(0x3ffff - 3, 1);
}

extern "C" {

	void LogMessage(const char* message, ...)
	{
		char buf[160];
		
		va_list ap;
		va_start(ap, message);
		vsnprintf(buf, sizeof(buf), message, ap);
		va_end(ap);
		
		CORBA_Environment env (idl4_default_environment);
		IF_LOGGING_LogMessage ((CORBA_Object)logger, buf, &env);
	}

}
