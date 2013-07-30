// $Id: test_PageFreeList.cc 46 2006-07-14 15:03:10Z sdi2 $ von Timo

#include <l4/types.h>

#include <stdio.h>

#include "PageFreeList.h"

int main()
{
	SlabAllocator<PageFreeEntry>::Pool slab;

	SlabAllocator<PageFreeEntry>::Page somepage;
	slab.addPage(&somepage);

	PageFreeList pfl (&slab);

	pfl.insert(L4_FpageLog2(0x20000, 12));

	pfl.insert(L4_FpageLog2(0x30000, 12));

	pfl.insert(L4_FpageLog2(0x25000, 12));

	pfl.insert(L4_FpageLog2(0x15000, 12));

	pfl.dump();
}
