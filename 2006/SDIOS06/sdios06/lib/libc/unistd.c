#include <config.h>

#include <sys/types.h>

#include <sdi/log.h>

ssize_t write(int fd, const char *buf, size_t count)
{
	// This is only here becuase libsupc++ needs it...
	LogMessage("Write called");
	return count;	
}

struct dl_phdr_info;
int dl_iterate_phdr(int (*callback) (struct dl_phdr_info *info,
                    size_t size, void *data), void *data)
{
	// This is only here becuase libsupc++ needs it...
	return 0;
}
