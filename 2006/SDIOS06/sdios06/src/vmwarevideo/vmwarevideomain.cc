#include <config.h>

#include <l4/thread.h>
#include <l4/sigma0.h>

#include <new>

#include <string.h>
#include <stdio.h>
#include <sdi/log.h>
#include <sdi/panic.h>
#include <sdi/locator.h>
#include <sdi/ports.h>

#include <l4/schedule.h>

#include <idl4glue.h>
#include <if/iflocator.h>
#include <if/iflogging.h>
#include <if/ifpci.h>

#include "../pci/pciheader.h"
#include "vmwarevideo.h"
#include "vmwarevideodriver.h"
#include "vmwarevideo-server.h"

namespace vmwarevideo
{
	VMWareVideoDriver* cards[10];
	size_t card_count;
}

int main()
{
	using namespace vmwarevideo;
	
	L4_ThreadId_t pci;
	objectid_t dummy;
	while(GetObject("/pci", IF_PCI_ID, &pci, &dummy) != OK) {
		L4_Yield();
		LogMessage("Waiting for pci driver");
	}
	
	pciconfig config;
	CORBA_Environment env (idl4_default_environment);
	IF_PCI_ScanForDevice(pci, PCI_VENDOR_ID_VMWARE, PCI_DEVICE_ID_VMWARE_SVGA2, config, &env);
	switch (env._major) {
		case CORBA_SYSTEM_EXCEPTION:
			printf("IPC failed, code %d\n", CORBA_exception_id(&env));
			CORBA_exception_free(&env);
			return -1;
		case CORBA_USER_EXCEPTION:
			printf("User-defined exception");
			CORBA_exception_free(&env);
			return -1;
		case CORBA_NO_EXCEPTION:
			break;
	}
	
	if(CORBA_exception_id(&env) == ex_no_such_device) {
		LogMessage("No VMWare video card found");
		return 1;
	}
	
	const PCI::Type0Header* header = (const PCI::Type0Header*) &config;
	
	uint16_t index = header->get_io_base(0) + SVGA_INDEX_PORT;
	uint16_t value = header->get_io_base(0) + SVGA_VALUE_PORT;
	
	VMWareVideoDriver* driver = new VMWareVideoDriver(index, value);
	cards[0] = driver;
	card_count = 1;
	
	vmwarevideo_server();
	
	panic("vmware video loop aborted?!?");
	return 0;
}
