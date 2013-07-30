/*****************************************************************
 * Source file : syscall.idl
 * Platform    : V4 IA32
 * Mapping     : CORBA C
 * 
 * Generated by IDL4 1.0.2 (roadrunner) on 13/07/2006 20:56
 * Report bugs to haeberlen@ira.uka.de
 *****************************************************************/
#include <idl4glue.h>
#include "syscall-server.h"

#include <stdio.h>
#include <sdi/panic.h>
#include <sdi/log.h>
#include <sdi/locator.h>
#include "root.h"

namespace {
	const size_t MSGBUFSIZE = 128;
}

extern L4_ThreadId_t sigma1id;

/* Interface syscall */

IDL4_INLINE void syscall_AssociateInterrupt_implementation(CORBA_Object _caller, const L4_ThreadId_t *InterruptThread, const L4_ThreadId_t *InterruptHandler, L4_Word_t *word, idl4_server_environment *_env)

{
	*word = L4_AssociateInterrupt(*InterruptThread, *InterruptHandler);
}

IDL4_PUBLISH_SYSCALL_ASSOCIATEINTERRUPT(syscall_AssociateInterrupt_implementation);

IDL4_INLINE void syscall_DeassociateInterrupt_implementation(CORBA_Object _caller, const L4_ThreadId_t *InterruptThread, L4_Word_t *word, idl4_server_environment *_env)

{
	*word = L4_DeassociateInterrupt(*InterruptThread);
}

IDL4_PUBLISH_SYSCALL_DEASSOCIATEINTERRUPT(syscall_DeassociateInterrupt_implementation);

IDL4_INLINE void syscall_Sigma1ThreadControl_implementation(CORBA_Object _caller, const L4_ThreadId_t *dest, const L4_ThreadId_t *SpaceSpecifier, const L4_ThreadId_t *scheduler, const L4_ThreadId_t *pager, const L4_Word_t utcblocation, idl4_server_environment *_env)
{
	if (_caller != sigma1id) return;

	if (!L4_ThreadControl(*dest, *SpaceSpecifier, *scheduler, *pager, (void*)utcblocation))
	{
		CORBA_exception_set(_env, ex_IF_SYSCALL_syscall_error, 0);
	}
}

IDL4_PUBLISH_SYSCALL_SIGMA1THREADCONTROL(syscall_Sigma1ThreadControl_implementation);

IDL4_INLINE void syscall_Sigma1SpaceControl_implementation(CORBA_Object _caller, const L4_ThreadId_t *SpaceSpecifier, const L4_Word_t control, const L4_Fpage_t *KIPArea, const L4_Fpage_t *UtcbArea, const L4_ThreadId_t *redirector, idl4_server_environment *_env)

{
	if (_caller != sigma1id) return;

	L4_Word_t dummy;
	if (!L4_SpaceControl(*SpaceSpecifier, control, *KIPArea, *UtcbArea, *redirector, &dummy))
	{
		CORBA_exception_set(_env, ex_IF_SYSCALL_syscall_error, 0);
	}
}

IDL4_PUBLISH_SYSCALL_SIGMA1SPACECONTROL(syscall_Sigma1SpaceControl_implementation);

void *syscall_vtable_9[SYSCALL_DEFAULT_VTABLE_SIZE] = SYSCALL_DEFAULT_VTABLE_9;
void *syscall_vtable_discard[SYSCALL_DEFAULT_VTABLE_SIZE] = SYSCALL_DEFAULT_VTABLE_DISCARD;
void **syscall_itable[16] = { syscall_vtable_discard, syscall_vtable_discard, syscall_vtable_discard, syscall_vtable_discard, syscall_vtable_discard, syscall_vtable_discard, syscall_vtable_discard, syscall_vtable_discard, syscall_vtable_discard, syscall_vtable_9, syscall_vtable_discard, syscall_vtable_discard, syscall_vtable_discard, syscall_vtable_discard, syscall_vtable_discard, syscall_vtable_discard };

void syscall_server()
{
	LogMessage("Syscall-Server alive...");
	L4_ThreadId_t partner;
	L4_MsgTag_t msgtag;
  	idl4_msgbuf_t msgbuf;
  	long cnt;

  	idl4_msgbuf_init(&msgbuf);
  	for (cnt = 0;cnt < SYSCALL_STRBUF_SIZE;cnt++)
    	idl4_msgbuf_add_buffer(&msgbuf, malloc(MSGBUFSIZE), MSGBUFSIZE);
    
  	while (1)
    {
    	partner = L4_nilthread;
      	msgtag.raw = 0;
      	cnt = 0;

      	while (1)
        {
          	idl4_msgbuf_sync(&msgbuf);

          	idl4_reply_and_wait(&partner, &msgtag, &msgbuf, &cnt);

          	if (idl4_is_error(&msgtag))
            	break;

          	idl4_process_request(&partner, &msgtag, &msgbuf, &cnt, syscall_itable[idl4_get_interface_id(&msgtag) & SYSCALL_IID_MASK][idl4_get_function_id(&msgtag) & SYSCALL_FID_MASK]);
       	}
    }
}

void syscall_discard()
{
	panic("Syscall-Server message discarded\n");
}

