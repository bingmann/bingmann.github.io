#include <config.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include <sdi/locator.h>
#include <sdi/log.h>

#include "cwd.h"
#include <idl4glue.h>
#include "if/iflocator.h"

extern "C" {

L4_ThreadId_t __cwd_server = L4_nilthread;
objectid_t __cwd_handle = 0;

char *getcwd(char *buf, size_t size)
{
	char *envcwd = getenv("CWD");
	if (!envcwd)
		envcwd = "/";

	size_t el = strlen(envcwd);
	if(el > size-1) {
		errno = ERANGE;
		return NULL;
	}

	if (buf == NULL) {
		if (size > 0)
			el = size;
		buf = reinterpret_cast<char*> (malloc(el + 1));
	} else {
		if (el > size)
			el = size - 1;
	}

	// printf("Els: %u Buf: %p Envcwd: %p\n", el, buf, envcwd);

	snprintf(buf, el+1, "%s", envcwd);
	return buf;
}

int chdir(const char* path)
{
	setenv("CWD", path, 1);
	__resolve_cwd();
	return 0;
}

void __resolve_cwd()
{
	char buf[128];
	
	if(!L4_IsNilThread(__cwd_server))
		return;

	getcwd(buf, sizeof(buf));
	if(GetObject(getcwd(buf, sizeof(buf)), IF_DIRECTORY_ID, &__cwd_server, &__cwd_handle) != OK)
	{
		LogMessage("Warning: couldn't resolve current working directory\n");
		__cwd_server = GetLocator();
		__cwd_handle = ROOT_DIRECTORY_HANDLE;
	}
}

int __resolve_parentdir(const char* path, interfaceid_t iid,
                        L4_ThreadId_t* server, objectid_t* handle)
{
	size_t len = strlen(path);
	char buf[len + 1];
	memcpy(buf, path, len+1);

	int slash = static_cast<int> (len);
	for( ; slash >= 0; --slash) {
		if(buf[slash] == '/') {
			buf[slash] = 0;
			break;
		}
	}

	if(slash < 0)
		return __resolve_path("/", iid, server, handle);

	return __resolve_path(buf, iid, server, handle);
}

const char* __basename(const char* path)
{
	int slash = strlen(path);
	for( ; slash >= 0; --slash) {
		if(path[slash] == '/')
			return path + slash + 1;
	}

	return path;
}

int __resolve_path(const char* path, interfaceid_t iid, L4_ThreadId_t* server, objectid_t* handle)
{
	// is it an absolute filename starting with '/' ?
	if(path[0] == '/') {
		if(GetObject(path, iid, server, handle) != OK) {
			LogMessage("Path '%s' not found", path);
			errno = ENOENT;
			return -1;
		}
		//LogMessage("Resolved '%s' to %lx - %lu", path, server->raw, *handle);
		return 0;
	}

	__resolve_cwd();

	// no, so resolve relative to current working directory
	if(GetObject2(__cwd_server, __cwd_handle, path, iid, server, handle) != OK) {
		LogMessage("Path '%s' not found", path);
		errno = ENOENT;
		return -1;
	}
	//LogMessage("Resolved '%s' to %lx - %lu", path, server->raw, *handle);

	return 0;
}

}
