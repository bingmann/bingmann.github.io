#ifndef SVGAREG_H_
#define SVGAREG_H_

namespace vmwarevideo
{
	static const uint16_t PCI_VENDOR_ID_VMWARE = 0x15AD;
	static const uint16_t PCI_DEVICE_ID_VMWARE_SVGA2 = 0x0405;
	
	static const uint16_t SVGA_INDEX_PORT = 0x0;
	static const uint16_t SVGA_VALUE_PORT = 0x1;
	
	static const uint32_t GUEST_OS_BASE = 0x5000;
	static const uint32_t GUEST_OS_OTHER  = GUEST_OS_BASE + 10;
	
	enum SVGAReg {
		SVGA_REG_ID = 0,
		SVGA_REG_ENABLE = 1,
		SVGA_REG_WIDTH = 2,
		SVGA_REG_HEIGHT = 3,
		SVGA_REG_MAX_WIDTH = 4,
		SVGA_REG_MAX_HEIGHT = 5,
		SVGA_REG_DEPTH = 6,
		SVGA_REG_BITS_PER_PIXEL = 7,                 /* Current bpp in the guest */
		SVGA_REG_PSEUDOCOLOR = 8,
		SVGA_REG_RED_MASK = 9,
		SVGA_REG_GREEN_MASK = 10,
		SVGA_REG_BLUE_MASK = 11,
		SVGA_REG_BYTES_PER_LINE = 12,
		SVGA_REG_FB_START = 13,
		SVGA_REG_FB_OFFSET = 14,
		SVGA_REG_VRAM_SIZE = 15,
		SVGA_REG_FB_SIZE = 16,
		
		SVGA_REG_CAPABILITIES = 17,
		SVGA_REG_MEM_START = 18,                             /* Memory for command FIFO and bitmaps */
		SVGA_REG_MEM_SIZE = 19,
		SVGA_REG_CONFIG_DONE = 20,                   /* Set when memory area configured */
		SVGA_REG_SYNC = 21,                                  /* Write to force synchronization */
		SVGA_REG_BUSY = 22,                                  /* Read to check if sync is done */
		SVGA_REG_GUEST_ID = 23,                              /* Set guest OS identifier */
		SVGA_REG_CURSOR_ID = 24,                             /* ID of cursor */
		SVGA_REG_CURSOR_X = 25,                              /* Set cursor X position */
		SVGA_REG_CURSOR_Y = 26,                              /* Set cursor Y position */
		SVGA_REG_CURSOR_ON = 27,                             /* Turn cursor on/off */
		SVGA_REG_HOST_BITS_PER_PIXEL = 28,   /* Current bpp in the host */
		SVGA_REG_SCRATCH_SIZE = 29,     /* Number of scratch registers */
		SVGA_REG_MEM_REGS = 30,         /* Number of FIFO registers */
		SVGA_REG_TOP = 31,              /* Must be 1 more than the last register */
		
		SVGA_PALETTE_BASE = 1024             /* Base of SVGA color map */
	};
	
	static const uint32_t SVGA_CAP_NONE             = 0x0000;
	static const uint32_t SVGA_CAP_RECT_FILL        = 0x0001;
	static const uint32_t SVGA_CAP_RECT_COPY        = 0x0002;
	static const uint32_t SVGA_CAP_RECT_PAT_FILL    = 0x0004;
	static const uint32_t SVGA_CAP_LEGACY_OFFSCREEN = 0x0008;
	static const uint32_t SVGA_CAP_RASTER_OP        = 0x0010;
	static const uint32_t SVGA_CAP_CURSOR           = 0x0020;
	static const uint32_t SVGA_CAP_CURSOR_BYPASS    = 0x0040;
	static const uint32_t SVGA_CAP_CURSOR_BYPASS_2  = 0x0080;
	static const uint32_t SVGA_CAP_8BIT_EMULATION   = 0x0100;
	static const uint32_t SVGA_CAP_ALPHA_CURSOR     = 0x0200;
	static const uint32_t SVGA_CAP_GLYPH            = 0x0400;
	static const uint32_t SVGA_CAP_GLYPH_CLIPPING   = 0x0800;
	static const uint32_t SVGA_CAP_OFFSCREEN_1      = 0x1000;
	static const uint32_t SVGA_CAP_ALPHA_BLEND      = 0x2000;
	static const uint32_t SVGA_CAP_3D               = 0x4000;
	static const uint32_t SVGA_CAP_EXTENDED_FIFO    = 0x8000;
	
	static const uint32_t SVGA_CMD_UPDATE = 1;
    /* FIFO layout: X, Y, Width, Height */
	static const uint32_t SVGA_CMD_RECT_FILL = 2;
    /* FIFO layout: Color, X, Y, Width, Height */	
	
	/*
	 *  FIFO offsets (viewed as an array of 32-bit words)
	 */
	enum {
		/*
		 * The original defined FIFO offsets
		 */
		SVGA_FIFO_MIN = 0,
		SVGA_FIFO_MAX,       /* The distance from MIN to MAX must be at least 10K */
		SVGA_FIFO_NEXT_CMD,
		SVGA_FIFO_STOP,
		
		/*
		 * Additional offsets added as of SVGA_CAP_EXTENDED_FIFO
		 */
		SVGA_FIFO_CAPABILITIES = 4,
		SVGA_FIFO_FLAGS,
		SVGA_FIFO_FENCE,
		SVGA_FIFO_3D_HWVERSION,     /* Check SVGA3dHardwareVersion in svga3d_reg.h */
		SVGA_FIFO_PITCHLOCK,
		
		/*
		 * Always keep this last.  It's not an offset with semantic value, but
		 * rather a convenient way to produce the value of fifo[SVGA_FIFO_NUM_REGS]
		 */
		SVGA_FIFO_NUM_REGS
	};
	
	static inline uint32_t make_id(uint32_t version) {
		return (0x900000UL << 8) | version;
	}		
}

#endif
