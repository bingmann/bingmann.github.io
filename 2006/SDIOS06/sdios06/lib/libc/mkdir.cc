#include <sys/stat.h>
#include <stdio.h>

extern "C" {

	int mkdir(const char* path, mode_t mode)
	{
		printf("MKDir not implemented yet\n");
		return -1;
	}

}
