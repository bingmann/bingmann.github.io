/*
 * $Id: locator.h 204 2006-08-21 09:55:42Z sdi2 $
 */
#ifndef LOCATOR_H_
#define LOCATOR_H_

#include <l4/types.h>
#include <idl4glue.h>
#include <if/types.h>

#ifdef __cplusplus
extern "C" {
#endif

	#define PATH_SEPARATOR '/'

	/*
	 * TODO: This is a HACK:
	 * I had to rename ErrorCode for the GetObject-Function as it name-conflicts
	 * with Matze's Minixfs ErrorCode. Maybe we should use namespaces here.
	 */
	typedef enum
	{
		OK                = 0,
		UNKNOWN           = 1,
		INVALIDPARAMS     = 2,
		NOTFOUND          = 3,
		NOTSUPPORTED      = 4,
		ALREADYREGISTERED = 5,
		INVALIDOBJECTID   = 6
	} LErrorCode;

	L4_ThreadId_t GetLocator();
	
	LErrorCode GetObject( const char*    path,
	                      interfaceid_t  iid,
	                      L4_ThreadId_t* server,
	                      objectid_t*    handle );

	LErrorCode GetObject2(L4_ThreadId_t  dir_server,
	                      objectid_t     dir_handle,
	                      const char*    path,
	                      interfaceid_t	 iid,
						  L4_ThreadId_t* server,
						  objectid_t*    handle);

	LErrorCode Register( L4_ThreadId_t server,
	                  /* objectid_t    directory, */
	                     const char*   name,
	                     interfaceid_t iid,
	                     objectid_t    handle );

	LErrorCode Unregister( L4_ThreadId_t server,
	                    /* objectid_t    directory, */
	                       const char*   name,
	                       interfaceid_t iid );
	
	LErrorCode EnumerateEntry( L4_ThreadId_t  server,
	                           objectid_t     directory,
	                           interfaceid_t  iid,
	                           L4_Word_t      entry,
	                           L4_ThreadId_t* out_server,
	                           objectid_t*    out_handle,
	                           char*          out_name );

#ifdef __cplusplus
}
#endif	

#endif
