// $Id: test_PageMappingList.cc 137 2006-07-24 10:56:30Z sdi2 $ von Timo

#include <l4/types.h>

#include <stdio.h>

#include "PageMappingList.h"

int main()
{
	SlabAllocator<PageMappingEntry>::Pool slab;

	SlabAllocator<PageMappingEntry>::Page somepage;
	slab.addPage(&somepage);

	PageMappingList pml (&slab);

	pml.insert(L4_Fpage(0x0010000, 64*1024), L4_Fpage(0xA000000, 64*1024));

	pml.insert(L4_Fpage(0x0030000, 64*1024), L4_Fpage(0xB000000, 64*1024));

	printf("Found:\n");
	struct PageMappingEntry *pme = pml.findAddress(0x0040000);
	pml.printPageMappingEntry(pme);

	pml.dump();

	for(unsigned int i = 1; i < 2; i++)
	{
		L4_Word_t a = pml.findSpace(0xA000000, i*8*1024);
		printf("found: %p\n", a);

		pml.insert(L4_Fpage(a, i*8*1024), L4_Fpage(i*1024, i*8*1024));
		pml.dump();
	}
}
