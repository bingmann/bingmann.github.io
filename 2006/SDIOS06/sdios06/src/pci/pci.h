#ifndef PCI_H_
#define PCI_H_

#include <stdint.h>
#include "pci-server.h"

bool pci_available();
void pci_ScanForDevice(CORBA_Object _caller, const uint16_t vendorID, const uint16_t deviceID, pciconfig config, idl4_server_environment *_env);

#endif /*PCI_H_*/
