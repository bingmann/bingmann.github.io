#include <new>

#include <config.h>
#include <assert.h>
#include <string.h>

#include <l4/kip.h>
#include <l4/schedule.h>
#include <l4/thread.h>
#include <l4/types.h>

#include <sdi/locator.h>
#include <sdi/log.h>

#include <l4/types.h>

#include <idl4glue.h>
#include <if/iflocator.h>



LErrorCode GetEntry( L4_ThreadId_t      server,
                     objectid_t         handle,
                     const char*        name,
                     interfaceid_t      iid,
                     L4_ThreadId_t*     out_server,
                     objectid_t*        out_handle,
                     CORBA_Environment* env )
{
	LErrorCode error = OK;
	
	
	IF_DIRECTORY_GetEntry( server,
	                       handle,
	                       name,
	                       iid,
	                       out_server,
	                       out_handle,
	                       env );

	if( env->_major != CORBA_NO_EXCEPTION )
	{	
		switch( CORBA_exception_id( env ) )
		{
			case ex_not_found: error = NOTFOUND; break;
			default:           error = UNKNOWN;  break;
		}
		
		CORBA_exception_free( env );
	}
	
	return( error );
}



extern "C" {

L4_ThreadId_t GetLocator()
{
	return L4_GlobalId (0x3ffff - 2 , 1);
}

LErrorCode GetObject( const char* path,
                      interfaceid_t iid,
                      L4_ThreadId_t* server,
                      objectid_t* handle )
{
	return GetObject2( GetLocator(),
	                   ROOT_DIRECTORY_HANDLE,
					   path,
					   iid,
					   server,
					   handle );
}


LErrorCode GetObject2( L4_ThreadId_t dir_server,
                       objectid_t dir_handle,
		               const char* path,
                       interfaceid_t iid,
                       L4_ThreadId_t* server,
                       objectid_t* handle )
{
	if( path == NULL /* || server == NULL || handle == NULL */ )
		return( INVALIDPARAMS );
	

	CORBA_Environment env( idl4_default_environment );
	
	char  entry[NAME_LENGTH];
	uint8_t j = 0;
	
	entry[0] = 0;
	
	for( ; *path != 0; path++ )
	{
		// Copy character to entry, if it is not a separator or the end
		// otherwise check the entry and perform a lookup if valid
		if( *path != PATH_SEPARATOR && *path != 0 )
		{
			entry[j++] = *path;
			
			if( j == NAME_LENGTH )
				return( INVALIDPARAMS );
		}
		else
		{
			// 0-terminate the string
			entry[j] = 0;

			// ignore multiple separators in paths
			if( entry[0] == 0 )
				continue;
			
			LErrorCode error = GetEntry( dir_server,
			                             dir_handle,
			                             entry,
			                             IF_DIRECTORY_ID,
			                             &dir_server,
			                             &dir_handle,
			                             &env );
			
			if( error != OK )
				return( error );
			
			// Reset entry
			j = 0;
			entry[0] = 0;
		}
	}
	
	if( entry[0] == 0 )
	{
		if( iid != IF_DIRECTORY_ID ) return( NOTFOUND );

		// return the directory itself.
		if( server != NULL ) *server = dir_server;
		if( handle != NULL ) *handle = dir_handle;

		return( OK );
	}
	else
	{
		// We are in the right directory and can perform the final lookup with
		// the right interface
	
		entry[j] = 0;
	
		if( server == NULL ) server = new L4_ThreadId_t;
		if( handle == NULL ) handle = new objectid_t;
	
		LErrorCode error = GetEntry( dir_server,
		                             dir_handle,
		                             entry,
		                             iid,
		                             server,
		                             handle,
		                             &env );
		
		return( error );
	}
}


LErrorCode Register( L4_ThreadId_t server,
                  /* objectid_t    directory, */
                     const char*   name,
                     interfaceid_t iid,
                     objectid_t    handle )
{
	if( !L4_IsGlobalId( server ) || name == NULL || name[0] == 0 || iid == (interfaceid_t) ANY_INTERFACE )
		return( INVALIDPARAMS );
	
	while(true) {
		CORBA_Environment env( idl4_default_environment );
	
		IF_NAMING_Register( server, name, iid, handle, &env );
		if(env._major == CORBA_NO_EXCEPTION)
			return OK;

		if(env._major == CORBA_USER_EXCEPTION )
		{	
			switch( CORBA_exception_id( &env ) )
			{
				case ex_already_registered:
					return ALREADYREGISTERED;
			}
			CORBA_exception_free( &env );
		}
		if(env._major == CORBA_SYSTEM_EXCEPTION)
		{
			int id = CORBA_exception_id(&env);
			if(id == L4_ErrInvalidThread || id == L4_ErrInvalidScheduler) {
				// locator is probably still in startup phase, try again
				LogMessage("waiting for locator");
                                L4_Sleep(L4_TimePeriod(100000));
				continue;
			}
		}

		return UNKNOWN;
	}
}


LErrorCode Unregister( L4_ThreadId_t server,
                    /* objectid_t    directory, */
                       const char*   name,
                       interfaceid_t iid )
{
	if( !L4_IsGlobalId( server ) || name == NULL || name[0] == 0 || iid == (interfaceid_t) ANY_INTERFACE )
		return( INVALIDPARAMS );
	
	LErrorCode error = OK;
	CORBA_Environment env( idl4_default_environment );
	
	IF_NAMING_Unregister( server, name, iid, &env );
	
	if( env._major != CORBA_NO_EXCEPTION )
	{	
		switch( CORBA_exception_id( &env ) )
		{
			default: error = UNKNOWN; break;
		}
		
		CORBA_exception_free( &env );
	}
	
	return( error );	
}


LErrorCode EnumerateEntry( L4_ThreadId_t  server,
                           objectid_t     directory,
                           interfaceid_t  iid,
                           L4_Word_t      entry,
                           L4_ThreadId_t* out_server,
                           objectid_t*    out_handle,
                           char*          out_name )
{
	if( !L4_IsGlobalId( server ) || out_server == NULL || out_handle == NULL || out_name == NULL )
		return( INVALIDPARAMS );

	LErrorCode error = OK;
	CORBA_Environment env( idl4_default_environment );
	CORBA_char *namebuf = NULL;

	IF_ENUMERABLE_EnumerateEntry( server,
	                              directory,
	                              iid,
	                              entry,
	                              out_server,
	                              out_handle,
	                              &namebuf,
	                              &env );	

	if( env._major != CORBA_NO_EXCEPTION )
	{	
		switch( CORBA_exception_id( &env ) )
		{
			case ex_not_found:        error = NOTFOUND; break;
			case ex_invalid_objectid: error = INVALIDOBJECTID; break;
			default: error = UNKNOWN; break;
		}
		
		CORBA_exception_free( &env );
	}

	if (error == OK) {
		strncpy( out_name, namebuf, NAME_LENGTH );

		CORBA_free( namebuf );
	}
	
	return( error );
}


} /* extern "C" */
