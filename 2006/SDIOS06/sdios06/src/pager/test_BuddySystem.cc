// $Id: test_BuddySystem.cc 46 2006-07-14 15:03:10Z sdi2 $ von Timo

#include <l4/types.h>

#include <stdio.h>
#include <vector>

#include "BuddySystem.h"

int main()
{
	SlabAllocator<PageFreeEntry>::Pool slab;
	SlabAllocator<PageFreeEntry>::Page somepages[4];

	slab.addPage(&somepages[0]);
	slab.addPage(&somepages[1]);
	slab.addPage(&somepages[2]);
	slab.addPage(&somepages[2]);

	BuddySystem bs (slab);

	bs.insert(L4_FpageLog2(0x20000, 12));

	bs.insert(L4_FpageLog2(0x120000, 12));

	bs.insert(L4_FpageLog2(0x12000000, 22)); // big area

	// should coalesce into (0x30000, 13)
	bs.insert(L4_FpageLog2(0x30000, 12));
	bs.insert(L4_FpageLog2(0x31000, 12));

	bs.insert(L4_FpageLog2(0x25000, 12));

	bs.insert(L4_FpageLog2(0x14000, 14));

	// coalesce with greater buddy from above
	bs.insert(L4_FpageLog2(0x24000, 12));

	// dont coalesce
	bs.insert(L4_FpageLog2(0x51000, 12));
	bs.insert(L4_FpageLog2(0x52000, 12));

	bs.dump();
	bs.test();

	std::vector<L4_Fpage_t> pages;
	L4_Fpage_t p1;
	
	while( !L4_IsNilFpage(p1 = bs.allocate(4096)) )
	{		
		printf("\nAllocated: 0x%08X \n", L4_Address(p1));

		pages.push_back(p1);
		
		bs.dump();
		bs.test();
	}

	// readd extracted pages

	for(unsigned int i = 0; i < pages.size(); i++)
	{
		bs.insert(pages[i]);

		bs.dump();
		bs.test();
	}

	// last output should be the same as after inserting pages.
}
