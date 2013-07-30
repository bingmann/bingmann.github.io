// $Id: pager.cc 155 2006-07-24 20:21:28Z sdi2 $
//
// File:  src/pager/pager.cc
//
// Description: pager main compilation file
//

#include <l4/thread.h>
#include <l4io.h>
#include <l4/sigma0.h>

#include <idl4glue.h>
#include <if/ifsyscall.h>

#include <sdi/panic.h>
#include <sdi/util.h>

#include <string.h>

#include "pager-server.h"

#include "RangeClassifier.h"
#include "BuddySystem.h"
#include "PageMappingList.h"
#include "TaskList.h"
#include "pdebug.h"

// *** Pointer to the KIP

static L4_KernelInterfacePage_t *KIP;

static const L4_Word_t kipmin_pagesize = 10;

// helpers for rounding pages up and down according to min_pgsize (from sigma0)
static inline L4_Word_t page_start(L4_Word_t page)
{
    return page & ~((1UL << kipmin_pagesize) - 1);
}

static inline L4_Word_t page_end(L4_Word_t page)
{
    return (page + ((1UL << kipmin_pagesize) - 1)) & ~((1UL << kipmin_pagesize) - 1);
}

// *** memory address classifier

enum { MT_INVALID=0, MT_CONVENTIONAL, MT_SPECIAL, MT_RESERVED };

static class RangeClassifier rangeclassifier(MT_INVALID);

static void rangeclassifier_init()
{
	// based on code from sigma0
    L4_MemoryDesc_t *md;

    // parse through all memory descriptors in kip.
    for (L4_Word_t n = 0; (md = L4_MemoryDesc(KIP, n)); n++)
    {
		if (L4_IsVirtual(md)) continue;

		L4_Word_t low = page_start(L4_MemoryDescLow(md));
		L4_Word_t high = page_end(L4_MemoryDescHigh(md)) - 1;

		if ((L4_MemoryDescType(md) &0xf) == L4_BootLoaderSpecificMemoryType ||
			(L4_MemoryDescType(md) &0xf) == L4_ArchitectureSpecificMemoryType)
		{
			/*
			 * Boot loader or architecture dependent memory.  Remove
			 * from conventional memory pool and insert into
			 * non-conventional memory pool.
			 */
			rangeclassifier.set(low, high, MT_SPECIAL);
		}
		else
		{
			switch (L4_MemoryDescType(md))
			{
			case L4_UndefinedMemoryType:
				break;

			case L4_ConventionalMemoryType:
				rangeclassifier.set(low, high, MT_CONVENTIONAL);
				break;

			case L4_ReservedMemoryType:
				rangeclassifier.set(low, high, MT_RESERVED);
				break;

			case L4_DedicatedMemoryType:
				rangeclassifier.set(low, high, MT_SPECIAL);
				break;

			case L4_SharedMemoryType:
				rangeclassifier.set(low, high, MT_SPECIAL);
				break;

			default:
				printf("pager: unknown memory type 0x%x\n", (int)L4_MemoryDescType(md));
				break;
			}
		}
    }

	// rangeclassifier.dump();
}


// *** virtual memory address classifier

enum { VMA_SEGFAULT=0, VMA_ANONYMOUS, VMA_PROGRAM };

static class RangeClassifier vmaddressclass(VMA_SEGFAULT);

static void vmaddressclass_init()
{
	// UTCB area 
	vmaddressclass.set(0x02010000, 0x0202ffff, VMA_ANONYMOUS);

	// program and heap area - manual upper limit check against brk
	vmaddressclass.set(0x08408000, 0x90000000, VMA_PROGRAM);

	// 0x90000000 - 0x9fffffff: user mapping - segfault

	// 0xa0000000 - 0xb7ffffff: pager mapping: should already be in the mapping list

	// stack area
	vmaddressclass.set(0xb8000000, 0xbfffffff, VMA_ANONYMOUS);
}

// *** slab allocator pool and a few initial pages for the buddy system's page
// *** free lists

typedef SlabAllocator<PageFreeEntry> PageFreeAllocator;

static PageFreeAllocator::Page pagefreeslabpages[32];
static PageFreeAllocator::Pool pagefreeslaballoc (pagefreeslabpages, 32);

static class BuddySystem buddysystem (pagefreeslaballoc);

// *** Per-Task Management

typedef SlabAllocator<PageMappingEntry> PageMappingAllocator;

static PageMappingAllocator::Page pagemappingslabpages[8];
static PageMappingAllocator::Pool pagemappingslaballoc (pagemappingslabpages, 8);

typedef SlabAllocator<TaskEntry> TaskAllocator;

static TaskAllocator::Page tasklistslabpages[4];
static TaskAllocator::Pool tasklistslaballoc (tasklistslabpages, 4);

static class TaskList tasklist (&tasklistslaballoc);

// *** Sigma0

static L4_ThreadId_t sigma0id;

static L4_ThreadId_t getSigma0Id()
{
	// taked from include/l4/sigma0.h

	return L4_GlobalId(KIP->ThreadInfo.X.UserBase, 1);
}

// *** Import ThreadId of syscallServer from roottast.o

extern L4_ThreadId_t syscallServerId;

// *** Some Miscellaneous Functions

#include "pager-ia32.h"

static inline void zeroFpage(L4_Fpage_t fp)
{
	pager_memset_long((void*)L4_Address(fp), 0, L4_Size(fp));
}

/// callback for slab allocator to get more anonymous memory from the buddy
/// system
static void* slaballocator_callback(unsigned int pagesize)
{
	L4_Fpage_t fp = buddysystem.allocate(pagesize);
	if (L4_IsNilFpage(fp)) return NULL;

	zeroFpage(fp);

	return (void*)L4_Address(fp);
}

// *** Test Application from pistachio

#define KB(x) (x*1024)
#define MB(x) (x*1024*1024)
#define GB(x) (x*1024*1024*1024)

int memgrab()
{
    printf ("Hello world, I will now allocate all available memory.\n\n");

    L4_Word_t tsize = 0;

    for (L4_Word_t s = sizeof (L4_Word_t) * 8 - 1; s >= 10; s--)
    {
		L4_Fpage_t f;
		int n = -1;

		do {
			f = L4_Sigma0_GetAny (L4_nilthread, s, L4_CompleteAddressSpace);
			n++;
		} while (! L4_IsNilFpage (f));

		L4_Word_t size = n * (1UL << s);
		tsize += size;

		if (n)
			printf ("Allocated %d pages of %3ld%cB (log2size %2ld) [%ld%cB]\n",
					n,
					s >= 30 ? 1UL << (s-30) :
					s >= 20 ? 1UL << (s-20) : 1UL << (s-10),
					s >= 30 ? 'G' : s >= 20 ? 'M' : 'K',
					s,
					size >= GB(1) ? size/GB(1) :
					size >= MB(1) ? size/MB(1) : size/KB(1),
					size >= GB(1) ? 'G' : size >= MB(1) ? 'M' : 'K');
    }

    // Avoid using floating point
    printf ("\nTotal memory: %ld.%ldGB | %ld.%ldMB | %ldKB\n",
			tsize / GB(1), ((tsize * 100) / GB(1)) % 100,
			tsize / MB(1), ((tsize * 100) / MB(1)) % 100,
			tsize / KB(1));

    for (;;);
    return 0;
}

// *** IDL Interface Implementation

#include "pager-impl.h"

void pager_main()
{
	printf("Pager startup. Allocating all available memory from sigma0.\n");

	KIP = (L4_KernelInterfacePage_t*)L4_GetKernelInterface();

	// construct memory class layout from KIP area
	rangeclassifier_init();

	vmaddressclass_init();

	pagefreeslaballoc.setAllocCallback(slaballocator_callback);
	pagemappingslaballoc.setAllocCallback(slaballocator_callback);
	tasklistslaballoc.setAllocCallback(slaballocator_callback);

	// request all pages from sigma0 in decreasing size.

	sigma0id = getSigma0Id();

	for (unsigned int pagelog2 = 22; pagelog2 >= 12; pagelog2--)
	{
		L4_Fpage_t fp;

		while(1)
		{
			fp = L4_Sigma0_GetAny(sigma0id, pagelog2, L4_CompleteAddressSpace);

			int mt = rangeclassifier.get(L4_Address(fp));
			if (mt != 1) {
				printf("Got special page: 0x%08lx - size %ld - type %d\n",
					   L4_Address(fp), L4_Size(fp), mt);
			}

			if (L4_IsNilFpage(fp)) break;

			buddysystem.insert(fp);
			// buddysystem.test();
			// buddysystem.dump();
		}
    }

	buddysystem.test();
	//buddysystem.dump();

	printf("pager: available anonymous memory %lu bytes\n", buddysystem.available());

	printf("pager: entering idl4 loop\n");
	pager_server();

	panic("pager: idl4 loop terminated!");

    while(1) ;
}


