// $Id$
// opendir and co using the name service
#include <config.h>

#include <dirent.h>
#include <assert.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>

#include <idl4glue.h>
#include <if/iflocator.h>
#include <sdi/locator.h>

#include "cwd.h"

// private 
struct __dirstream
{
	L4_ThreadId_t	nsserver;
	objectid_t		dirhandle;
	L4_Word_t		entryiter;

	// the static object returned by readdir()
	struct dirent	de;
};


DIR* opendir(const char* name)
{
	assert(name);

	L4_ThreadId_t nss;
	objectid_t hand;
	DIR *newdir;

	if(__resolve_path(name, IF_DIRECTORY_ID, &nss, &hand) != 0)
		return NULL;

	newdir = malloc(sizeof(DIR));
	memset(newdir, 0, sizeof(DIR));
	newdir->nsserver = nss;
	newdir->dirhandle = hand;
	newdir->entryiter = 0;

	return newdir;
}

int closedir(DIR* dir)
{
	assert(dir);

	free(dir);
	return 0;
}

struct dirent* readdir(DIR* dir)
{
	assert(dir);

	LErrorCode ec = EnumerateEntry(dir->nsserver,
								   dir->dirhandle,
								   (interfaceid_t) ANY_INTERFACE,
								   dir->entryiter,
								   &dir->de.d_server,
								   &dir->de.d_object,
								   &dir->de.d_name);
	if (ec != OK || L4_IsNilThread(dir->de.d_server))
		return NULL;

	dir->entryiter++;

	return &dir->de;
}

