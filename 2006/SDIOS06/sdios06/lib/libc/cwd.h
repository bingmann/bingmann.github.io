#ifndef CWD_H_
#define CWD_H_

#include <sys/cdefs.h>
#include <l4/types.h>

#include <idl4glue.h>
#include <if/types.h>

__BEGIN_DECLS
	
extern L4_ThreadId_t __cwd_server;
extern objectid_t __cwd_handle;

void __resolve_cwd();

int __resolve_parentdir(const char* path, interfaceid_t iid, L4_ThreadId_t* server, objectid_t* handle);
const char* __basename(const char* path);
int __resolve_path(const char* path, interfaceid_t iid, L4_ThreadId_t* server, objectid_t* handle);

__END_DECLS

#endif
