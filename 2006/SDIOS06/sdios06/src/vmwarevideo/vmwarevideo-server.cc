/*****************************************************************
 * Source file : src/vmwarevideo/vmware.idl
 * Platform    : V4 IA32
 * Mapping     : CORBA C
 * 
 * Generated by IDL4 1.0.2 (roadrunner) on 08/07/2006 10:50
 * Report bugs to haeberlen@ira.uka.de
 *****************************************************************/
#include <config.h>

#include <stdio.h>
#include <sdi/panic.h>
#include <sdi/log.h>
#include <sdi/locator.h>
 
#include <idl4glue.h>
#include "vmwarevideo-server.h"
#include "vmwarevideodriver.h"
#include "vmwarevideo.h"
#include <if/iflocator.h>

using namespace vmwarevideo;

// print messages on error conditions
#define PRINT_ERROR

/* Interface vmware */
IDL4_INLINE void vmwarevideo_SetMode_implementation(CORBA_Object _caller, const objectid_t card, const CORBA_long width, const CORBA_long height, const CORBA_long bpp, idl4_server_environment *_env)
{
	if(card >= card_count) {
#ifdef PRINT_ERROR
		printf("Invalid video card number %lu specified\n", card);
#endif
		CORBA_exception_set (_env, ex_invalid_card, 0);
		return;
	}
	
	VMWareVideoDriver* driver = cards[card];
	LogMessage("VideoDriver SetMode %dx%d", width, height);
	driver->set_mode(width, height);		
}

IDL4_PUBLISH_VMWAREVIDEO_SETMODE(vmwarevideo_SetMode_implementation);

IDL4_INLINE void vmwarevideo_MapFrameBuffer_implementation(CORBA_Object _caller, const objectid_t card, idl4_fpage_t *f, idlsize_t *fbsize, idl4_server_environment *_env)
{
	if(card >= card_count) {
#ifdef PRINT_ERROR
		printf("MapFrameBuffer: Invalid video card number %lu specified\n", card);
#endif
		CORBA_exception_set (_env, ex_invalid_card, 0);
		return;
	}
	
	VMWareVideoDriver* driver = cards[card];
	L4_Word_t addr = reinterpret_cast<L4_Word_t>(driver->get_framebuffer());
	L4_Word_t size = static_cast<L4_Word_t>(driver->get_framebuffer_size());
	L4_Fpage_t fpage = L4_Fpage(addr, size);
	
	idl4_fpage_set_mode(f, IDL4_MODE_MAP);
	idl4_fpage_set_page(f, fpage);
	idl4_fpage_set_base(f, 0);
	idl4_fpage_set_permissions(f, IDL4_PERM_FULL);

	*fbsize = driver->get_framebuffer_size();
}

IDL4_PUBLISH_VMWAREVIDEO_MAPFRAMEBUFFER(vmwarevideo_MapFrameBuffer_implementation);

IDL4_INLINE void vmwarevideo_Update_implementation(CORBA_Object _caller, const objectid_t card, idl4_server_environment *_env)
{
	if(card >= card_count) {
#ifdef PRINT_ERROR
		printf("Update: Invalid video card number %lu specified\n", card);
#endif
		CORBA_exception_set (_env, ex_invalid_card, 0);
		return;
	}
	
	VMWareVideoDriver* driver = cards[card];
	if(!driver->is_initialized()) {
#ifdef PRINT_ERROR
		printf("Update: Video driver not initialized yet\n");
#endif
		CORBA_exception_set(_env, ex_not_initialized, 0);
		return;
	}
	
	driver->update();
}

IDL4_PUBLISH_VMWAREVIDEO_UPDATE(vmwarevideo_Update_implementation);

IDL4_INLINE void vmwarevideo_GetInfos_implementation(CORBA_Object _caller, const objectid_t card, CORBA_long *width, CORBA_long *height, CORBA_long *bpp, idlsize_t* fboffset, idlsize_t *pitch, uint32_t *redmask, uint32_t *greenmask, uint32_t *bluemask, idl4_server_environment *_env) 
{
	if(card >= card_count) {
#ifdef PRINT_ERROR
		printf("GetInfos: Invalid video card number %lu specified\n", card);
#endif
		CORBA_exception_set (_env, ex_invalid_card, 0);
		return;
	}
	
	VMWareVideoDriver* driver = cards[card];
	if(!driver->is_initialized()) {
#ifdef PRINT_ERROR
		printf("GetInfos: Video driver not initialized yet\n");
#endif
		CORBA_exception_set(_env, ex_not_initialized, 0);
		return;
	}
	
	*width = driver->get_width();
	*height = driver->get_height();
	*bpp = driver->get_bpp();
	*fboffset = driver->get_fboffset();
	*pitch = driver->get_pitch();
	*redmask = driver->get_redmask();
	*greenmask = driver->get_greenmask();
	*bluemask = driver->get_bluemask();
}

IDL4_PUBLISH_VMWAREVIDEO_GETINFOS(vmwarevideo_GetInfos_implementation);

void *vmwarevideo_vtable_16[VMWAREVIDEO_DEFAULT_VTABLE_SIZE] = VMWAREVIDEO_DEFAULT_VTABLE_16;
void *vmwarevideo_vtable_discard[VMWAREVIDEO_DEFAULT_VTABLE_SIZE] = VMWAREVIDEO_DEFAULT_VTABLE_DISCARD;
void **vmwarevideo_itable[32] = { vmwarevideo_vtable_discard, vmwarevideo_vtable_discard, vmwarevideo_vtable_discard, vmwarevideo_vtable_discard, vmwarevideo_vtable_discard, vmwarevideo_vtable_discard, vmwarevideo_vtable_discard, vmwarevideo_vtable_discard, vmwarevideo_vtable_discard, vmwarevideo_vtable_discard, vmwarevideo_vtable_discard, vmwarevideo_vtable_discard, vmwarevideo_vtable_discard, vmwarevideo_vtable_discard, vmwarevideo_vtable_discard, vmwarevideo_vtable_discard, vmwarevideo_vtable_16, vmwarevideo_vtable_discard, vmwarevideo_vtable_discard, vmwarevideo_vtable_discard, vmwarevideo_vtable_discard, vmwarevideo_vtable_discard, vmwarevideo_vtable_discard, vmwarevideo_vtable_discard, vmwarevideo_vtable_discard, vmwarevideo_vtable_discard, vmwarevideo_vtable_discard, vmwarevideo_vtable_discard, vmwarevideo_vtable_discard, vmwarevideo_vtable_discard, vmwarevideo_vtable_discard, vmwarevideo_vtable_discard };

static void announce()
{
	CORBA_Environment env (idl4_default_environment);
	// to be really cool we would create a subdir /videocards with
	// entries for each videocard ;-)
	for(size_t i = 0; i < card_count; ++i) {
		char name[128];
		snprintf(name, sizeof(name), "video%u",i);
		if(Register(GetLocator(), name, IF_FRAMEBUFFER_ID, i) != OK)
			panic("Couldn't register graphics card");
	}	
}

void vmwarevideo_server()
{
	const size_t MSGBUFSIZE = 100;
	L4_ThreadId_t partner;
	L4_MsgTag_t msgtag;
	idl4_msgbuf_t msgbuf;
	long cnt;
	
	idl4_msgbuf_init(&msgbuf);
	for (cnt = 0;cnt < VMWAREVIDEO_STRBUF_SIZE;cnt++)
		idl4_msgbuf_add_buffer(&msgbuf, malloc(MSGBUFSIZE), MSGBUFSIZE);
		
	announce();

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

          idl4_process_request(&partner, &msgtag, &msgbuf, &cnt, vmwarevideo_itable[idl4_get_interface_id(&msgtag) & VMWAREVIDEO_IID_MASK][idl4_get_function_id(&msgtag) & VMWAREVIDEO_FID_MASK]);
        }
    }
}

void vmwarevideo_discard()
{
	panic("vmware message dicarded");
}

