#include <config.h>

#include <l4/sigma0.h>
#include <l4/schedule.h>
#include <string.h>
#include <sdi/log.h>
#include <sdi/ports.h>
#include "vmwarevideodriver.h"

//#define VMWARE_DEBUG

namespace vmwarevideo
{
	VMWareVideoDriver::VMWareVideoDriver(uint16_t index, uint16_t value)
	{
		width = 0;
		height = 0;
		this->index_port = index;
		this->value_port = value;
		
		// Check version
		for(uint32_t i = 2; i != -1UL; --i) {
			out(SVGA_REG_ID, make_id(i));
			if(in(SVGA_REG_ID) == make_id(i)) {
				LogMessage("Version %d is supported", i);
				break;
			}
		}
		
		out(SVGA_REG_GUEST_ID, GUEST_OS_OTHER);	
		
		fbstart = (char*) in(SVGA_REG_FB_START);
		fbsize = (size_t) in(SVGA_REG_FB_SIZE);
		memstart = (char*) in(SVGA_REG_MEM_START);
		memsize = (size_t) in(SVGA_REG_MEM_SIZE);
#ifdef VMWARE_DEBUG
		LogMessage("FBStart %p fbsize %u MemStart %p MemSize %u",
			fbstart, fbsize, memstart, memsize);
#endif
			
		capabilities = in(SVGA_REG_CAPABILITIES);
		uint32_t max_width = in(SVGA_REG_MAX_WIDTH);
		uint32_t max_height = in(SVGA_REG_MAX_HEIGHT);
		uint32_t host_bpp = in(SVGA_REG_HOST_BITS_PER_PIXEL);
		(void) max_height;
		(void) max_height;
		(void) host_bpp;
#ifdef VMWARE_DEBUG
		LogMessage("Caps: %x MWidth: %u MHeight: %u HostBPP: %d", capabilities, max_width, max_height,
			host_bpp);
#endif
			
		L4_Fpage_t page = L4_Sigma0_GetPage_RcvWindow(L4_nilthread,
			L4_Fpage((L4_Word_t) memstart, (L4_Word_t) memsize),
			L4_Fpage(0xa0000000, (L4_Word_t) memsize));
		if(L4_IsNilFpage(page)) {
			LogMessage("not ok");
		}
		memstart = (char*) 0xa0000000;
		
		page = L4_Sigma0_GetPage_RcvWindow(L4_nilthread,
			L4_Fpage((L4_Word_t) fbstart, (L4_Word_t) fbsize),
			L4_Fpage(0x90000000, (L4_Word_t) fbsize));
		if(L4_IsNilFpage(page)) {
			LogMessage("not ok");
		}
		fbstart = (char*) 0x90000000;
		LogMessage("FrameBuffer at %p length %u", fbstart, fbsize);
						
		init_fifo();
	}
	
	VMWareVideoDriver::~VMWareVideoDriver()
	{
	}
		
	void
	VMWareVideoDriver::set_mode(int new_width, int new_height)
	{
		if(new_height == 0 && new_height == 0) {
			out(SVGA_REG_ENABLE, 0);
			this->width = new_height;
			this->height = new_height;
			return;
		}

		uint32_t depth = in(SVGA_REG_HOST_BITS_PER_PIXEL);
		LogMessage("Trying to set mode %dx%d D:%u", new_width, new_height, depth);
		
		out(SVGA_REG_WIDTH, new_width);
		out(SVGA_REG_HEIGHT, new_height);
		out(SVGA_REG_BITS_PER_PIXEL, depth);
		
		fb_offset = in(SVGA_REG_FB_OFFSET);
		fb_pitch = in(SVGA_REG_BYTES_PER_LINE);
		bpp = in(SVGA_REG_DEPTH);
		// vmware seems to report 24bpp also in 32bpp modes
		if(bpp == 24)
			bpp = 32;
		in(SVGA_REG_PSEUDOCOLOR);
		redmask = in(SVGA_REG_RED_MASK);
		greenmask = in(SVGA_REG_GREEN_MASK);
		bluemask = in(SVGA_REG_BLUE_MASK);
		
		this->width = new_width;
		this->height = new_height;
		out(SVGA_REG_ENABLE, 1);
		
		memset((void*) fbstart, 0, fbsize);
		update();
	}
		
	void
	VMWareVideoDriver::update()
	{
		write_fifo(SVGA_CMD_UPDATE);
		write_fifo(0);	// x
		write_fifo(0);	// y
		write_fifo(width); // width
		write_fifo(height); // height
		sync();
	}
		
	void 
	VMWareVideoDriver::sync()
	{
		out(SVGA_REG_SYNC, 1);
		while(in(SVGA_REG_BUSY) != 0) {
			L4_Yield();
		}
	}
		
	void
	VMWareVideoDriver::init_fifo()
	{
		bool extended_fifo = capabilities & SVGA_CAP_EXTENDED_FIFO;
		uint32_t min = extended_fifo ? in(SVGA_REG_MEM_REGS) : 4;
		
		uint32_t* fifo = (uint32_t*) memstart;
		fifo[SVGA_FIFO_MIN] = min * sizeof(uint32_t);
		fifo[SVGA_FIFO_MAX] = memsize & ~0x3;
		fifo[SVGA_FIFO_NEXT_CMD] = min * sizeof(uint32_t);
		fifo[SVGA_FIFO_STOP] = min * sizeof(uint32_t);
		out(SVGA_REG_CONFIG_DONE, 1);
	}
		
	void
	VMWareVideoDriver::write_fifo(uint32_t value)
	{
		uint32_t* fifo = (uint32_t*) memstart;

		/* Need to sync? */
	    if ((fifo[SVGA_FIFO_NEXT_CMD] + sizeof(uint32_t) == fifo[SVGA_FIFO_STOP])
	     || (fifo[SVGA_FIFO_NEXT_CMD] == fifo[SVGA_FIFO_MAX] - sizeof(uint32_t) &&
         fifo[SVGA_FIFO_STOP] == fifo[SVGA_FIFO_MIN])) {
         	LogMessage("Syncing because of full FIFO");
         	sync();
		}
		
		fifo[fifo[SVGA_FIFO_NEXT_CMD] / sizeof(uint32_t)] = value;
		if(fifo[SVGA_FIFO_NEXT_CMD] == fifo[SVGA_FIFO_MAX] - sizeof(uint32_t)) {
			fifo[SVGA_FIFO_NEXT_CMD] = fifo[SVGA_FIFO_MIN];
		} else {
			fifo[SVGA_FIFO_NEXT_CMD] += sizeof(uint32_t);
		}
	}
	
	void
	VMWareVideoDriver::out(SVGAReg reg, uint32_t value) {
		outl(index_port, reg);
		outl(value_port, value);
	}
	
	uint32_t
	VMWareVideoDriver::in(SVGAReg reg) {
		outl(index_port, reg);
		return inl(value_port);
	}
}
