#include <config.h>

#include <sys/stat.h>
#include <errno.h>

#include <sdi/log.h>
#include <sdi/panic.h>
#include <sdi/locator.h>
#include <sdi/util.h>

#include <idl4glue.h>
#include <if/iflocator.h>
#include <if/iffile.h>

int stat(const char* fname, struct stat* stat)
{
	L4_ThreadId_t server;
	objectid_t handle;
	bool isdir = false;
	
	if(GetObject(fname, IF_FILE_ID, &server, &handle) != OK) {
		if(GetObject(fname, IF_DIRECTORY_ID, &server, &handle) != OK) {
			errno = ENOENT;
			return -1;
		}
		isdir = true;
	}
	
	size_t size = 0;
	if(!isdir) {
		CORBA_Environment env (idl4_default_environment);		
		IF_FILE_GetFileSize(server, handle, &size, &env);
		if(env._major != CORBA_NO_EXCEPTION) {
			errno = EIO;
			return -1;	
		}
	}
	
	stat->st_mode = isdir ? S_IFDIR : S_IFREG;
	stat->st_size = size;
	return 0;
}
