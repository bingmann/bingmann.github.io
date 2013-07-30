#include <config.h>

#include <idl4glue.h>
#include <stdlib.h>

extern "C" {
	
	void* CORBA_alloc(size_t size)
	{
		// workaround for idl4 bugs :-/
		if(size % 4 > 0)
			size += 4 - (size % 4);
		return malloc(size);
	}
	
	void CORBA_free(void* ptr)
	{
		free(ptr);
	}
	
	CORBA_char* CORBA_string_alloc(size_t size)
	{
		return static_cast<CORBA_char*> (CORBA_alloc(size));
	}
	
	void CORBA_string_free(CORBA_char* ptr)
	{
		CORBA_free(ptr);
	}	
	
}
