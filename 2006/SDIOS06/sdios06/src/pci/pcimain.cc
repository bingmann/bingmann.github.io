#include <config.h>

#include <stdio.h>

#include <l4/thread.h>

#include <sdi/panic.h>
#include <sdi/ports.h>
#include <sdi/log.h>
#include <sdi/locator.h>

#include <idl4glue.h>
#include <if/iflocator.h>
#include <if/iflogging.h>

#include "pciheader.h"
#include "pci.h"
#include "pci-server.h"

int main()
{
	if(!pci_available()) {	
        LogMessage("No PCI configuration space found");
        return 1;
    }		
			 
	LogMessage("PCI server started");	
	pci_server();
	
	return 0;
}
