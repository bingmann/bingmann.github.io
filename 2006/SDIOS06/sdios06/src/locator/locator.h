#ifndef __LOCATOR_H_INCLUDED__
#define __LOCATOR_H_INCLUDED__


#include <stdint.h>

#include <idl4glue.h>
#include <if/types.h>
#include <if/iflocator.h>

namespace
{

const uint16_t NUM_ENTRIES = 255;

} /* namespace */


/*
 * Error codes returned by Locator::GetEntry and Locator::Unregister
 */
enum ErrorCode
{
	OK                = 0,
	UNKNOWN           = 1,
	NOTFOUND          = 2,
	NOTSUPPORTED      = 3,
	ALREADYREGISTERED = 4,
	INVALIDPARAMS     = 5,
	LISTFULL          = 6
};


class Entry
{
public:
	objectid_t    directory;
	char          name[NAME_LENGTH];
	interfaceid_t iid;
	L4_ThreadId_t server;
	objectid_t    handle;
};


class Locator
{
private:
	Entry entries[NUM_ENTRIES];
	

public:	
	Locator();
	~Locator();
	
	ErrorCode GetEntry( objectid_t     directory,
	                    const char*    name,
	                    interfaceid_t  iid,
	                    L4_ThreadId_t* server,
	                    objectid_t*    handle );
	
	ErrorCode Register( objectid_t    directory,
	                    const char*   name,
	                    interfaceid_t iid,
	                    L4_ThreadId_t server,
	                    objectid_t    handle );
	
	ErrorCode Unregister( objectid_t directory,
	                      const char* name,
	                      interfaceid_t iid );
	
	ErrorCode EnumerateEntry( objectid_t     directory,
	                          interfaceid_t  iid,
	                          L4_Word_t      entry,
	                          L4_ThreadId_t* server,
	                          objectid_t*    handle,
	                          const char*    name);
};




#endif /*__LOCATOR_H_INCLUDED__*/
