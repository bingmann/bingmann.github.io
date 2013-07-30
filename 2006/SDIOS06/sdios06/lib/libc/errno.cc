#include <config.h>

#include <errno.h>

extern "C" {
	
	int errno = 0;

	char* strerror(int errnum)
	{
		return "NO description for this error yet (sorry)";
	}
	
}
