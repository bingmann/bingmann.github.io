#ifndef _PCIHEADER_H
#define _PCIHEADER_H

#include <stdint.h>
#include <sdi/util.h>

namespace PCI {
	
struct VendorDevice {
	uint16_t vendorID;
	uint16_t deviceID;
} __attribute__((packed));

struct PCIHeader {
    uint16_t    vendorID;
    uint16_t    deviceID;
    uint16_t    commandReg;
    uint16_t    statusReg;
    uint8_t     revisionID;
    uint8_t     progIF;
    uint8_t     subClassCode;
    uint8_t     classCode;
    uint8_t     cachelineSize;
    uint8_t     latency;
    uint8_t     headerType;
    uint8_t     BIST;
} __attribute__((packed));

struct Type0Header : public PCIHeader {
	uint32_t	base[6];
	uint32_t	cardbusCis;
	uint16_t	subsystemVendor;
	uint16_t	subsystemID;
	uint32_t	expansionRomBaseAddress;
	uint32_t	res[2];
	uint8_t		interruptLine;
	uint8_t		interruptPin;
	uint8_t		minGNT;
	uint8_t		maxLatency;
	
	uint32_t get_io_base(size_t num) const {
		return base[num] & ~0x3;
	}
} __attribute__((packed));

ASSERT_SIZE(Type0Header, 64);
	
}

#endif
