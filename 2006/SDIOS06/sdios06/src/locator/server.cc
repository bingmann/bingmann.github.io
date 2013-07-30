/*****************************************************************
 * Source file : locator.idl
 * Platform    : V4 IA32
 * Mapping     : CORBA C
 * 
 * Generated by IDL4 1.0.2 (roadrunner) on 16/07/2006 21:19
 * Report bugs to haeberlen@ira.uka.de
 *****************************************************************/

#include <idl4glue.h>

#include "locator-server.h"

#include "locator.h"
#include "debug.h"

extern Locator* locator;




/* Interface locator */

IDL4_INLINE void locator_GetEntry_implementation( CORBA_Object _caller,
                                                  const objectid_t directory_handle,
                                                  const CORBA_char *name,
                                                  const interfaceid_t interfaceid,
                                                  L4_ThreadId_t *server,
                                                  objectid_t *object_handle,
                                                  idl4_server_environment *_env )
{
	/* implementation of IF_DIRECTORY::GetEntry */
	
	ErrorCode error = locator->GetEntry( directory_handle,
	                                     name,
	                                     interfaceid,
	                                     server,
	                                     object_handle );

	if( error == OK )
		return;

	switch( error )
	{
		case NOTFOUND:     CORBA_exception_set ( _env, ex_not_found, 0 );     return;
		case NOTSUPPORTED: CORBA_exception_set ( _env, ex_not_supported, 0 ); return;
		default: return;
	}

	return;
}

IDL4_PUBLISH_LOCATOR_GETENTRY(locator_GetEntry_implementation);



IDL4_INLINE void locator_Register_implementation( CORBA_Object _caller,
                                                  const CORBA_char *name,
                                                  const interfaceid_t interfaceid,
                                                  const objectid_t root_directory,
                                                  idl4_server_environment *_env )
{
	/* implementation of IF_NAMING::Register */
	
	ErrorCode error = locator->Register( ROOT_DIRECTORY_HANDLE,
	                                     name,
	                                     interfaceid,
	                                     static_cast<L4_ThreadId_t>( _caller ),
	                                     root_directory );
	
	if( error == OK )
		return;
		
	switch( error )
	{
		case ALREADYREGISTERED: CORBA_exception_set (_env, ex_already_registered, 0); return;
		default: return;
	}

	return;
}

IDL4_PUBLISH_LOCATOR_REGISTER(locator_Register_implementation);



IDL4_INLINE void locator_Unregister_implementation( CORBA_Object _caller,
                                                    const CORBA_char *name,
                                                    const interfaceid_t interfaceid,
                                                    idl4_server_environment *_env)
{
	/* implementation of IF_NAMING::Unregister */

	ErrorCode error = locator->Unregister( ROOT_DIRECTORY_HANDLE,
	                                       name,
	                                       interfaceid );
	
	if( error == OK )
		return;

	// No Exceptions in interface.. . .  .

	return;
}

IDL4_PUBLISH_LOCATOR_UNREGISTER(locator_Unregister_implementation);



IDL4_INLINE void locator_EnumerateEntry_implementation( CORBA_Object _caller,
                                                        const objectid_t directory_handle,
                                                        const interfaceid_t interfaceid,
                                                        const L4_Word_t entry,
                                                        L4_ThreadId_t *server,
                                                        objectid_t *object_handle,
                                                        CORBA_char **name,
                                                        idl4_server_environment *_env)
{
	/* implementation of IF_ENUMERABLE::EnumerateEntry */

	ErrorCode error = locator->EnumerateEntry( directory_handle,
	                                           interfaceid,
	                                           entry,
	                                           server,
	                                           object_handle,
	                                           *name );
	if( error == OK )
		return;
		
	switch( error )
	{
		case NOTFOUND: CORBA_exception_set (_env, ex_invalid_objectid, 0); return;
		default: return;
	}
	return;
}

IDL4_PUBLISH_LOCATOR_ENUMERATEENTRY(locator_EnumerateEntry_implementation);



void *locator_vtable_5[LOCATOR_DEFAULT_VTABLE_SIZE] = LOCATOR_DEFAULT_VTABLE_5;
void *locator_vtable_6[LOCATOR_DEFAULT_VTABLE_SIZE] = LOCATOR_DEFAULT_VTABLE_6;
void *locator_vtable_7[LOCATOR_DEFAULT_VTABLE_SIZE] = LOCATOR_DEFAULT_VTABLE_7;
void *locator_vtable_discard[LOCATOR_DEFAULT_VTABLE_SIZE] = LOCATOR_DEFAULT_VTABLE_DISCARD;
void **locator_itable[8] = { locator_vtable_discard, locator_vtable_discard, locator_vtable_discard, locator_vtable_discard, locator_vtable_discard, locator_vtable_5, locator_vtable_6, locator_vtable_7 };



void locator_server( void )
{
	L4_ThreadId_t partner;
	L4_MsgTag_t msgtag;
	idl4_msgbuf_t msgbuf;
	long cnt;

	idl4_msgbuf_init( &msgbuf );
	for( cnt = 0; cnt < LOCATOR_STRBUF_SIZE; cnt++ )
		idl4_msgbuf_add_buffer( &msgbuf, malloc( 8000 ), 8000 );

	while( 1 )
	{
		partner = L4_nilthread;
		msgtag.raw = 0;
		cnt = 0;

		while (1)
		{
			idl4_msgbuf_sync( &msgbuf );

			idl4_reply_and_wait(&partner, &msgtag, &msgbuf, &cnt);

			if( idl4_is_error( &msgtag ) )
				break;

			idl4_process_request( &partner,
			                      &msgtag,
			                      &msgbuf,
			                      &cnt,
			                      locator_itable[idl4_get_interface_id( &msgtag ) &
			                      	LOCATOR_IID_MASK][idl4_get_function_id( &msgtag ) &
			                      	LOCATOR_FID_MASK] );
		}
	}
}



void locator_discard( void )
{
	// ...
}

