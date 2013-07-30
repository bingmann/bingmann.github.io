#ifndef VMWAREDRIVER_H_
#define VMWAREDRIVER_H_

#include <stdint.h>
#include "svgareg.h"

namespace vmwarevideo
{
	class VMWareVideoDriver {
	public:
		VMWareVideoDriver(uint16_t index_port, uint16_t value_port);
		~VMWareVideoDriver();
		
		bool is_initialized() const
		{
			return width != 0;
		}
		
		void set_mode(int new_width, int new_height);
		void update();
		
		void* get_framebuffer() const
		{
			return fbstart;
		}
		
		size_t get_framebuffer_size() const
		{
			return fbsize;
		}

		uint32_t get_fboffset() const
		{
			return fb_offset;
		}
		
		uint32_t get_pitch() const
		{
			return fb_pitch;
		}
		
		int get_width() const
		{
			return width;
		}
		
		int get_height() const
		{
			return height;
		}
		
		int get_bpp() const
		{
			return bpp;
		}
		
		uint32_t get_redmask() const
		{
			return redmask;
		}
		
		uint32_t get_greenmask() const
		{
			return greenmask;
		}
		
		uint32_t get_bluemask() const
		{
			return bluemask;
		}
				
	private:
		void sync();
	
		void init_fifo();	
		void write_fifo(uint32_t value);
	
		void out(SVGAReg reg, uint32_t value);
		uint32_t in(SVGAReg reg);
	
		uint16_t index_port;
		uint16_t value_port;
		uint32_t capabilities;
		uint32_t fb_offset;
		uint32_t fb_pitch;
		
		char* fbstart;
		size_t fbsize;
		char* memstart;
		size_t memsize;
		
		uint32_t width, height;
		int bpp;
		uint32_t redmask, greenmask, bluemask;
	};
}

#endif
