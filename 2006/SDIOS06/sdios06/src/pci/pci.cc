#include <config.h>

#include <assert.h>
#include <stdint.h>

#include <sdi/ports.h>
#include <sdi/log.h>

#include <idl4glue.h>
#include "pci.h"
#include "pciheader.h"
#include "pci-server.h"

namespace {
    const L4_Word16_t PCI_CONFIG_PORT = 0x0CF8;
    const L4_Word16_t PCI_DATA_PORT   = 0x0CFC;

    // only scan first 10 pci buses
    const int PCI_MAX_BUSSES = 10;
    const int PCI_MAX_DEVICES = 32;
    const int PCI_MAX_FUNCTIONS = 8;

	uint32_t getConfigAddress(int bus, int device, int function, int reg)
	{
	    assert(bus >= 0 && bus <= 0xff);
	    assert(device >= 0 && device <= 0x1f);
	    assert(function >= 0 && function <= 0x07);
	    assert(reg >= 0 && reg <= 0x3f);
    
	    return 0x80000000L | (bus & 0xff) << 16 | (device & 0x1f) << 11
	        | (function & 0x07) << 8 | (reg & 0x3f) << 2;
	}

	void readPCIHeader(void* buf, size_t size, int bus, int device, int function)
	{
		assert(size % 4 == 0);
		uint32_t* ptr = ((uint32_t*) buf);
		for(size_t i = 0; i < size; i += 4) {
			uint32_t addr = getConfigAddress(bus, device, function, i);
			outl(PCI_CONFIG_PORT, addr);
			*(ptr + i) = inl(PCI_DATA_PORT);
		}	
	}
}

bool pci_available()
{
    outl(PCI_CONFIG_PORT, 0x80000000L);
    L4_Word32_t reply = inl(PCI_CONFIG_PORT);
	return reply == 0x80000000L;
}

void pci_ScanForDevice(CORBA_Object _caller, const uint16_t vendorID, const uint16_t deviceID, pciconfig config, idl4_server_environment *_env)
{
	using namespace PCI;
	
    LogMessage("Scanning PCI busses:");
	for(int bus = 0; bus < PCI_MAX_BUSSES; ++bus) {
		for(int device = 0; device < PCI_MAX_DEVICES; ++device) {
			for(int function = 0; function < PCI_MAX_FUNCTIONS; ++function) {
				L4_Word32_t addr = getConfigAddress(bus, device, function, 0);
				outl(PCI_CONFIG_PORT, addr);

				VendorDevice vendorDevice;				
				uint32_t* hptr = (uint32_t*) &vendorDevice;
				*hptr = inl(PCI_DATA_PORT);
				
				if(vendorDevice.vendorID != vendorID
				    || vendorDevice.deviceID != deviceID)
					continue;
				    
				readPCIHeader(config, sizeof(pciconfig), bus, device, function);
				const Type0Header* header = (const Type0Header*) config;
				
				LogMessage("Found device at bus %d device %d function %d: Vendor %x Device %x",
				        bus, device, function,
				        header->vendorID, header->deviceID);
				LogMessage("\tClass %d Subclass %d Rev %d Base %ux", header->classCode, header->subClassCode, header->revisionID,
					header->base[0]);
				return;
			}
		}
	}
    
	CORBA_exception_set (_env, ex_no_such_device, 0);
	return;
}
