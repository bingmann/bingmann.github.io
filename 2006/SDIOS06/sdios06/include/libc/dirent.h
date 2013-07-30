#ifndef _DIRENT_H
#define _DIRENT_H

#include <idl4glue.h>
#include <if/types.h>
#include <sys/cdefs.h>

__BEGIN_DECLS

#define NAME_MAX	255

struct dirent {
	unsigned short d_reclen;
	char d_name[NAME_MAX+1];
	L4_ThreadId_t	d_server;
	objectid_t		d_object;
};

typedef struct __dirstream DIR;

DIR* opendir(const char* name);
int closedir(DIR* dir);
struct dirent* readdir(DIR* dir);

__END_DECLS

#endif

