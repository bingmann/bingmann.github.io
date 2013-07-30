#ifndef __DEBUG_H_INCLUDED__
#define __DEBUG_H_INCLUDED__

#include <new>
#include <l4/thread.h>
#include <sdi/log.h>

#include <idl4glue.h>
#include <if/types.h>
#include <if/iflocator.h>

#include "locator.h"


namespace {

void ErrorToString( ErrorCode error, char** string )
{
	switch( error )
	{
		case OK:                *string = "OK";                return;
	    case NOTFOUND:          *string = "NOTFOUND";          return;
		case NOTSUPPORTED:      *string = "NOTSUPPORTED";      return;
		case ALREADYREGISTERED: *string = "ALREADYREGISTERED"; return;
		case INVALIDPARAMS:     *string = "INVALIDPARAMS";     return;
		case LISTFULL:          *string = "LISTFULL";          return;
		default:                *string = "UNKNOWN";           return;
	}
}

}


/*
 * This is a proxy class that is used for debugging purposes.
 * It writes all locator-events to the log.
 * 
 * I would love to use inheritance with virtuals here, but Matze
 * told me that it doesn't work in SDIOS ;.-(
 */
class LogLocator
{
private:
	Locator* locator;

public:	
	 LogLocator() { locator = new Locator(); }
	 LogLocator( Locator* p_locator ) { locator = p_locator; }
	~LogLocator() { delete locator; }
	
	
	
	ErrorCode GetEntry( objectid_t     directory,
	                    const char*    name,
	                    interfaceid_t  iid,
	                    L4_ThreadId_t* server,
	                    objectid_t*    handle )
	{
		L4_ThreadId_t result_server;
		objectid_t    result_handle;
		
		ErrorCode error = locator->GetEntry( directory,
		                                     name,
		                                     iid,
		                                     &result_server,
		                                     &result_handle );
		
		if( error == OK )
		{
			LogMessage( "[locator] Lookup: Dir %lu, %s, IF %lu -> TID %lx , Handle %lu",
			            directory, name, iid, result_server.raw, result_handle);
			
			*server = result_server;
			*handle = result_handle;
		}
		else
		{
			char* errorstr = NULL;
			ErrorToString( error, &errorstr );
			
			LogMessage( "[locator] Lookup: Dir %lu , %s , IF %lu -> %s",
			            directory, name, iid, errorstr);
		}
		
		return( error );
	}
	
	
	
	ErrorCode Register( objectid_t    directory,
	                    const char*   name,
	                    interfaceid_t iid,
	                    L4_ThreadId_t server,
	                    objectid_t    handle )
	{
		ErrorCode error = locator->Register( directory,
		                                     name,
		                                     iid,
		                                     server,
		                                     handle );
		
		if( error == OK )
		{
			LogMessage( "[locator] Register: Dir %lu , %s , IF %lu -> TID %lx , Handle %lu",
			            directory, name, iid, server.raw, handle);
		}
		else
		{
			char* errorstr = NULL;
			ErrorToString( error, &errorstr );
			
			LogMessage( "[locator] Register: Dir %lu , %s , IF %lu -> %s",
			            directory, name, iid, errorstr);
		}		
		return( error );
	}
	
	
	
	ErrorCode Unregister( objectid_t    directory,
	                      const char*   name,
	                      interfaceid_t iid )
	{
		ErrorCode error = locator->Unregister( directory, name, iid );
		
		char* errorstr = NULL;
		ErrorToString( error, &errorstr );
		
		LogMessage( "[locator] Unregister: Dir %lu , %s , IF %lu -> %s",
            directory, name, iid, errorstr);
		
		return( error );
	}
	
	
	/*
	 * Warning: This doesn't handle NULL-Values properly - I'm too lazy ATM
	 */
	ErrorCode EnumerateEntry( objectid_t     directory,
	                          interfaceid_t  iid,
	                          L4_Word_t      entry,
	                          L4_ThreadId_t* server,
	                          objectid_t*    handle,
	                          const char*    name)
	{
		ErrorCode error = locator->EnumerateEntry( directory,
		                                           iid,
		                                           entry,
		                                           server,
		                                           handle,
		                                           name );

		if( error == OK )
		{
			LogMessage( "[locator] EnumerateEntry: Dir %lu , IF %lu , Entry %lu -> TID %lx , Handle %lu , %s",
			            directory, iid, entry, server->raw, *handle, name);
		}
		else
		{
			char* errorstr = NULL;
			ErrorToString( error, &errorstr );
			
			LogMessage( "[locator] EnumerateEntry: Dir %lu , IF %lu , Entry %lu -> %s",
			            directory, iid, entry, errorstr);
		}
		
		return( error );
	}
};



#endif /*__DEBUG_H_INCLUDED__*/
