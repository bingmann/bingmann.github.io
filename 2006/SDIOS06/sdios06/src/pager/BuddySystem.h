// $Id: BuddySystem.h 155 2006-07-24 20:21:28Z sdi2 $

#ifndef SDI2_PAGER_BUDDYSYSTEM_H
#define SDI2_PAGER_BUDDYSYSTEM_H

#include "PageFreeList.h"
#include "pdebug.h"

/** Implements a simple buddy system allocator for fpages by using 12
 * PageFreeLists. It splits and coalesces the pages correctly. */

class BuddySystem
{
public:
	// the pager will support size requests from 2^12 - 2^24 (4K - 16M)

	static const unsigned int minpagesizelog2 = 12;
	static const unsigned int maxpagesizelog2 = 24;

	static const unsigned int freelistnum = maxpagesizelog2 - minpagesizelog2 + 1;

	static const unsigned int minpagesize = (1 << minpagesizelog2);
	static const unsigned int maxpagesize = (1 << maxpagesizelog2);

 private:

	/// slab allocator type used by the free list
	typedef SlabAllocator<PageFreeEntry> 	slaballoc_t;

	/// reference to the used slab allocator
	slaballoc_t::Pool&		slaballoc;

	/// freelists for each of the supported page sizes. freelist[i] contains
	/// pages of size 2^{12+i}
	PageFreeList	freelist[freelistnum];

	/// total available memory in bytes
	L4_Word_t		freemem;

	/// function to calculate the size of the pages contained in a freelist
	static inline unsigned int listpagesize(unsigned int listnum)
	{
		return 1 << (minpagesizelog2 + listnum);
	}

	/// calculate the buddy page of p
	static inline L4_Fpage_t calcBuddyPage(L4_Fpage_t p)
	{
		// flips to lowest significant bit in the address
		p.raw ^= (1 << p.X.s);
		return p;
	}

public:

	/// create a buddy system backed by the given allocator
	explicit inline BuddySystem(slaballoc_t::Pool &usedallocator)
		: slaballoc(usedallocator), freemem(0)
	{
		for(unsigned int i = 0; i < freelistnum; i++)
			freelist[i].setSlabAllocator(slaballoc);
	}

	/// return number of available bytes.
	inline L4_Word_t available() const
	{ return freemem; }

	/// return the number of free lists
	inline unsigned int get_freelistnum() const
	{ return freelistnum; }

	/// return a contained free list
	inline const PageFreeList* get_freelist(unsigned int n) const
	{ return &freelist[n]; }

	/// test invariants of the buddy system
	inline void test() const
	{
		L4_Word_t		totmem = 0;

		for(unsigned int i = 0; i < freelistnum; i++)
		{
			const struct PageFreeEntry *pfe = freelist[i].begin();
			if (!pfe) continue;

			while(1)
			{
				assert(L4_Size(pfe->page) == listpagesize(i));
				totmem += L4_Size(pfe->page);

				if (pfe->next == pfe) break;
				pfe = pfe->next;
			}
		}

		assert(totmem == freemem);
	}

	/// request a memory region of 2^psize bytes
	inline L4_Fpage_t allocateLog2(unsigned int pagesizelog2)
	{
		if (pagesizelog2 > maxpagesizelog2) {
			assert(0);
			return L4_Nilpage;
		}

		if (pagesizelog2 < minpagesizelog2) // allocate at least one 4k page
			pagesizelog2 = minpagesizelog2;

		pdbg2("pager: buddysys allocating page: %lu bytes\n", (1UL << pagesizelog2));

		test();

		unsigned int fl = pagesizelog2 - minpagesizelog2;

		L4_Fpage_t p = freelist[fl].getFirstPage(); // automaticly removes it
													// from the free list

		if (L4_IsNilFpage(p))
		{
			// no pages of the given size available
			if (pagesizelog2 == maxpagesizelog2) {
				// out of continuous memory
				return L4_Nilpage;
			}

			// get a page twice the required size
			p = allocateLog2(pagesizelog2 + 1);

			if (L4_IsNilFpage(p)) {
				// memory is exhausted.
				return p;
			}

			// decrease size by one bit
			p.X.s--;

			// calculate the buddy by flipping the least significant bit
			L4_Fpage_t buddy = calcBuddyPage(p);

			// insert the buddy
			insert(buddy);

			// return the first page
		}
		else
		{
			freemem -= L4_Size(p);
		}

		test();

		return p;
	}

	/// request a memory region of pagesize
	inline L4_Fpage_t allocate(unsigned int pagesize)
	{
		// some magic from L4's header files
		unsigned int pslog2 = __L4_Msb(pagesize); // most significant bit number

		if ((1UL << pslog2) < pagesize) pslog2++;

		return allocateLog2(pslog2);
	}

	/// (re-)insert an fpage to the buddy system. this function can also be named free()
	inline void insert(L4_Fpage_t page)
	{
		pdbg3("pager: buddysys insert 0x%lx - size %lu\n",
			  L4_Address(page), L4_Size(page));

		test();

		unsigned int sl = L4_SizeLog2(page);

		if (sl < minpagesizelog2) {
			assert(0);
			return;
		}

		if (sl > maxpagesizelog2)
		{
			// TODO: write function to break fpage into smaller pages
			assert(0);
			return;
		}

		if (sl == maxpagesizelog2)
		{
			// just insert it
			freelist[maxpagesizelog2 - minpagesizelog2].insert(page);

			freemem += L4_Size(page);
		}
		else
		{
			// try to coalesce the new page with it's buddy.
			unsigned int fl = sl - minpagesizelog2;
			
			PageFreeEntry *nearest = freelist[fl].findNearest(page);

			L4_Fpage_t buddy = calcBuddyPage(page);
			if (nearest && nearest->page.raw == buddy.raw)
			{
				pdbg3("Found greater buddy.\n");

				// remove the buddy, and recursivly insert the combined page
				// into the next larger freelist
				freelist[fl].remove(nearest);
				freemem -= L4_Size(page);

				insert(L4_FpageLog2(L4_Address(page) & ~(1UL << sl), sl+1));
			}
			else if (nearest && nearest->prev->page.raw == buddy.raw)
			{
				pdbg3("Found smaller buddy.\n");

				freelist[fl].remove(nearest->prev);
				freemem -= L4_Size(page);

				// same as above, because we just clear the least significant bit
				insert(L4_FpageLog2(L4_Address(page) & ~(1UL << sl), sl+1));
			}
			else {
				freelist[fl].insertNear(page, nearest);

				freemem += L4_Size(page);
			}
		}

		test();
	}

	/// dump the pages in the buddy system
	inline void dump() const
	{
		printf("Buddysystem dump:\n");
		for(unsigned int fl = freelistnum; fl > 0; fl--)
		{
			if (freelist[fl-1].begin() == NULL) continue;

			pdbg0("Pages size %d\n", listpagesize(fl-1));
			freelist[fl-1].dump();
		}
	}
};

#endif // SDI2_PAGER_BUDDYSYSTEM_H
