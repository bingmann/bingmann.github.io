//
// File:  src/names/locatormain.cc
//
// Description: Root name service
//
#include <new>
#include <stdio.h>
#include <l4/thread.h>
#include <l4/schedule.h>
#include <sdi/log.h>
#include <sdi/panic.h>

#include <idl4glue.h>
#include <if/types.h>
#include <if/iflocator.h>

#include "locator.h"
#include "debug.h"
#include "locator-server.h"


Locator* locator = NULL;

/*
void test( LogLocator* p_locator )
{
	locator->Register( ROOT_DIRECTORY_HANDLE,
	                   "testfoo",
	                   static_cast<interfaceid_t>( 0 ),
	                   L4_nilthread,
	                   static_cast<objectid_t>( 5 ) );
	locator->Register( ROOT_DIRECTORY_HANDLE,
	                   "testbar",
	                   static_cast<interfaceid_t>( 2 ),
	                   L4_nilthread,
	                   static_cast<objectid_t>( 13 ) );
	locator->Register( ROOT_DIRECTORY_HANDLE,
	                   "huibuh",
	                   static_cast<interfaceid_t>( 4 ),
	                   L4_nilthread,
	                   static_cast<objectid_t>( 13 ) );

	locator->Register( ROOT_DIRECTORY_HANDLE,
	                   "testbar",
	                   static_cast<interfaceid_t>( 2 ),
	                   L4_nilthread,
	                   static_cast<objectid_t>( 13 ) ); // ALREADYREGISTERED
	locator->Register( ROOT_DIRECTORY_HANDLE,
	                   "melo",
	                   static_cast<interfaceid_t>( ANY_INTERFACE ),
	                   L4_nilthread,
	                   static_cast<objectid_t>( 13 ) ); // INVALIDPARAMS

	L4_ThreadId_t server;
	objectid_t    handle;
	locator->GetEntry( ROOT_DIRECTORY_HANDLE, "sonja", ANY_INTERFACE, &server, &handle ); // NOTFOUND
	locator->GetEntry( ROOT_DIRECTORY_HANDLE, "testfoo", ANY_INTERFACE, &server, &handle );
	locator->GetEntry( ROOT_DIRECTORY_HANDLE, "testfoo", static_cast<interfaceid_t>( 0 ), &server, &handle );
	locator->GetEntry( ROOT_DIRECTORY_HANDLE, "testfoo", static_cast<interfaceid_t>( 1 ), &server, &handle ); // NOTSUPP

	locator->Unregister( ROOT_DIRECTORY_HANDLE, "testfoo", static_cast<interfaceid_t>( 1 ) ); // NOTFOUND
	locator->Unregister( ROOT_DIRECTORY_HANDLE, "testfoo", static_cast<interfaceid_t>( 0 ) );
	locator->Unregister( ROOT_DIRECTORY_HANDLE, "testfoo", static_cast<interfaceid_t>( 0 ) ); // NOTFOUND

	uint32_t i = 0;
	char name[NAME_LENGTH];
	while( locator->EnumerateEntry( ROOT_DIRECTORY_HANDLE,
	                                ANY_INTERFACE,
	                                i++,
	                                &server,
	                                &handle,
	                                name ) == OK )
	{
		// ...
	}
}		
*/

int main()
{
	LogMessage( "[locator] Locator is alive..." );
	locator = new Locator();

	locator->Register(ROOT_DIRECTORY_HANDLE,
					  "task",
					  static_cast<interfaceid_t>(IF_DIRECTORY_ID),
					  L4_Pager(),
					  static_cast<objectid_t>(0) );
	
	locator_server();
	
	panic( "Reached end of locator program..\n" );	
	return( 1 );
}
