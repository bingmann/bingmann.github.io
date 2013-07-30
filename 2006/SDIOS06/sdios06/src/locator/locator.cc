#include <stdint.h>
#include <string.h>
#include <sdi/log.h>

#include <idl4glue.h>
#include <if/types.h>
#include <if/iflocator.h>

#include "locator.h"


Locator::Locator()
{
	// Clear the list
	memset( &entries, 0, sizeof( Entry ) * NUM_ENTRIES );
}


Locator::~Locator()
{
	// ...
}


ErrorCode Locator::GetEntry( objectid_t     directory,
                             const char*    name,
                             interfaceid_t  iid,
                             L4_ThreadId_t* server,
                             objectid_t*    handle )
{
	if( name[0] == 0 )
		return( INVALIDPARAMS );
	
	bool found = false;

	for( uint8_t i = 0; i < NUM_ENTRIES; i++ )
	{
		if( entries[i].directory == directory &&
		    ( strncmp( entries[i].name, name, NAME_LENGTH ) == 0 ) )
		{
			found = true;
			
			if( iid == ANY_INTERFACE || iid == entries[i].iid )
			{
				if( server != NULL ) (*server) = entries[i].server;
				if( handle != NULL ) (*handle) = entries[i].handle;
				
				return( OK );
			}
		}
	}
	
	return( ( found ) ? NOTSUPPORTED : NOTFOUND );
}


ErrorCode Locator::Register( objectid_t    directory,
                             const char*   name,
                             interfaceid_t iid,
                             L4_ThreadId_t server,
                             objectid_t    handle )
{	
	 // Check for valid params
	if( iid != ANY_INTERFACE //&&
	    /* L4_IsGlobalId( server ) */ )
	{	
		// Does it already exist?
		L4_ThreadId_t existing_server;
		objectid_t    existing_handle;
		if( GetEntry( directory, name, iid, &existing_server, &existing_handle) == OK )
		{
			if( server == existing_server && handle == existing_handle )
				return( OK ); // This is not so bad...
			else
				return( ALREADYREGISTERED ); // But the user should notice this..
		}

		
		// Walk the list for a free entry...
		for( uint8_t i = 0; i < NUM_ENTRIES; i++ )
		{
			if( entries[i].name[0] == 0 ) // An entry is free, when the name is an empty string
			{	
				strncpy( entries[i].name, name, NAME_LENGTH );
				entries[i].directory = directory;
				entries[i].iid       = iid;
				entries[i].server    = server;
				entries[i].handle    = handle;

				return( OK );
			}
		}
		
		return( LISTFULL );
	}
	
	return( INVALIDPARAMS );
}


ErrorCode Locator::Unregister( objectid_t directory,
                               const char* name,
                               interfaceid_t iid )
{
	// We *could* allow ANY_INTERFACE, but I think it is dangerous
	if( iid != ANY_INTERFACE )
	{	
		for( uint8_t i = 0; i < NUM_ENTRIES; i++ )
		{
			if( entries[i].directory == directory &&
				entries[i].iid == iid &&
			    strncmp( entries[i].name, name, NAME_LENGTH ) == 0 )
			{	
				entries[i].name[0] = 0;
				return( OK );
			}
		}
		return( NOTFOUND );
	}
	return( INVALIDPARAMS );
}

/*
 * If iid == ANY_INTERFACE there can be more results for the same (dir,name)
 * Do you think this is a bug??
 */
ErrorCode Locator::EnumerateEntry( objectid_t     directory,
                                   interfaceid_t  iid,
                                   L4_Word_t      entry,
                                   L4_ThreadId_t* server,
                                   objectid_t*    handle,
                                   const char*    name)
{
	uint8_t hit = 0;
	
	for( uint8_t i = 0; i < NUM_ENTRIES; i++ )
	{
		if( entries[i].name[0] != 0 &&
		    entries[i].directory == directory &&
		    ( iid == ANY_INTERFACE || iid == entries[i].iid ) &&
		    hit++ == entry )
		{
			if( server != NULL ) (*server) = entries[i].server;
			if( handle != NULL ) (*handle) = entries[i].handle;
			strncpy( const_cast<char*>( name ), entries[i].name, NAME_LENGTH );
					
			return( OK );		
		}
	}
	
	if( server != NULL) (*server) = L4_nilthread;
	if( handle != NULL) (*handle) = 0;
	return( OK );
}
