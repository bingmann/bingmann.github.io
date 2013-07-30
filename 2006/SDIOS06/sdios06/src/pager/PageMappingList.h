// $Id: PageMappingList.h 146 2006-07-24 16:53:09Z sdi2 $

#ifndef SDI2_PAGER_PAGEMAPPINGLIST_H
#define SDI2_PAGER_PAGEMAPPINGLIST_H

#include <assert.h>
#include "SlabAllocator.h"

/** the node struct of the doubly-linked list of page mappings */
struct PageMappingEntry
{
	struct PageMappingEntry *prev, *next;
	L4_Fpage_t	userpage;	// the virtual memory fpage in the thread's space
	L4_Fpage_t	backing;	// the backed physical memory fpage
};

/** PageMappingList holds a doubly-linked list of fpage mapping in a slab
 * allocator. The used pages are sorted by user addresses in ascending
 * order. */

class PageMappingList
{
private:

	/// slab allocator type used by the mapping list
	typedef SlabAllocator<PageMappingEntry> 	slaballoc_t;

	/// reference to the used slab allocator
	slaballoc_t::Pool*		slaballoc;

	/// the head of the doubly-linked list
	struct PageMappingEntry	*head;

	/// constructed from l4's headers, calculates the last valid address of an
	/// fpage
	static inline L4_Word_t __L4_EndAddress(L4_Fpage_t f)
	{
		return f.raw | ((1UL << f.X.s) - 1);
	}

	/// calculates the first address after an fpage
	static inline L4_Word_t __L4_NextAddress(L4_Fpage_t f)
	{
		return L4_Address(f) + L4_Size(f);
	}

public:
	/// create a mapping list backed by the given allocator
	explicit inline PageMappingList(slaballoc_t::Pool *usedallocator = NULL)
		: slaballoc(usedallocator), head(NULL)
	{
	}

	/// return all entries to the slab allocator
	inline ~PageMappingList()
	{
		clear();
	}

	/// needed by the TaskList for deferred initialization
	inline void setSlabAllocator(slaballoc_t::Pool &sa)
	{
		slaballoc = &sa;
	}

	/// remove all entries
	inline void clear()
	{
		if (!head) return;
	
		class PageMappingEntry* iter = head;

		while(iter->next != iter)
		{
			class PageMappingEntry* iternext = iter->next;
			
			slaballoc->freeobj(iter);

			iter = iternext;
		}

		slaballoc->freeobj(iter);
		head = NULL;
	}

	/// test invariants of the list
	inline void test() const
	{
		if (!head) return;

		class PageMappingEntry* iter = head;
		assert(iter->prev == iter);

		while(iter->next != iter)
		{
			assert(iter->next->prev == iter);

			assert(L4_Address(iter->userpage) <= L4_Address(iter->next->userpage));
			assert(L4_Address(iter->userpage) + L4_Size(iter->userpage) <= L4_Address(iter->next->userpage));

			assert(L4_SizeLog2(iter->userpage) == L4_SizeLog2(iter->backing));

			iter = iter->next;
		}
	}

	/// returns the head of the doubly-linked list
	inline const struct PageMappingEntry* begin() const
	{ return head; }

	/// insert a mapping into the used list
	inline struct PageMappingEntry* insert(L4_Fpage_t _userpage, L4_Fpage_t _backing)
	{
		test();

		// create a new node and insert it into the linked list at the right
		// place.
		PageMappingEntry *newnode = slaballoc->allocate();
		if (!newnode) {
			assert(0);
			return NULL;
		}

		newnode->userpage = _userpage;
		newnode->backing = _backing;

		if (!head)
		{
			// first node
			newnode->next = newnode;
			newnode->prev = newnode;
			head = newnode;

			return head;
		}
		else
		{
			PageMappingEntry *iter = head;

			L4_Word_t findaddr = L4_Address(newnode->userpage);
			
			while(findaddr >= L4_Address(iter->userpage))
			{
				if (iter->next == iter)
				{
					// insert the node at the end. as it has highest address
					iter->next = newnode;
					newnode->next = newnode;
					newnode->prev = iter;

					test();
					return newnode;
				}

				iter = iter->next;
			}

			// insert the address in front of the iter node.
			if (iter->prev == iter)
			{
				// insert as first node

				assert(head == iter);

				newnode->prev = newnode;
				newnode->next = iter;

				iter->prev = newnode;
				head = newnode;
			}
			else
			{
				// somewhere in the middle
				iter->prev->next = newnode;
				newnode->next = iter;

				newnode->prev = iter->prev;
				iter->prev = newnode;
			}

			test();
			return newnode;
		}
	}

	/// remove an fpage from the used list given by the entry ptr
	inline void remove(struct PageMappingEntry *e)
	{
		if (e->prev == e) // first entry in list
		{
			assert(head == e);
			
			if (e->next == head) {
				head = NULL;
			}
			else {
				head = e->next;
				head->prev = head;
			}
		}
		else if (e->next == e) // last entry in list
		{
			e->prev->next = e->prev;
		}
		else // somewhere in between
		{
			e->prev->next = e->next;
			e->next->prev = e->prev;
		}

		slaballoc->freeobj(e);
	}

	/// returns the first mapping entry containing the address a. returns NULL
	/// if no mapping matches.
	inline struct PageMappingEntry *findAddress(L4_Word_t a)
	{
		if (!head) return NULL;

		PageMappingEntry *iter = head;

		while(a >= L4_Address(iter->next->userpage))
		{
			if (iter->next == iter)
				break;

			iter = iter->next;
		}

		assert(__L4_EndAddress(iter->userpage) == 
			   L4_Address(iter->userpage) + L4_Size(iter->userpage) - 1);

		if (a < L4_Address(iter->userpage)
			|| a > __L4_EndAddress(iter->userpage))
			return NULL;

		return iter;
	}

	/// finds the first address after firstaddr which can hold an fpage of
	/// fpagesize
	inline L4_Word_t findSpace(const L4_Word_t firstaddr, L4_Word_t fpagesize) const
	{
		assert((firstaddr & (fpagesize-1)) == 0); // firstaddr must be aligned

		if (!head) return firstaddr;

		const PageMappingEntry *iter = head;

		while(firstaddr > L4_Address(iter->userpage))
		{
			if (iter->next == iter)
				break;

			iter = iter->next;
		}

		// iter is now at the first mapping >= the first address
		while(1)
		{
			L4_Word_t prevend = (iter->prev != iter) ? __L4_NextAddress(iter->prev->userpage) : firstaddr;
			if (prevend < firstaddr) prevend = firstaddr;

			L4_Word_t nextbegin = L4_Address(iter->userpage);

			// round up to fpagesize
			if ((prevend & (fpagesize-1)) != 0)
			{
				prevend |= (fpagesize-1);
				prevend++;
			}
			assert((prevend & (fpagesize-1)) == 0);

			nextbegin &= ~(fpagesize-1);

			if (prevend <= nextbegin && prevend + fpagesize <= nextbegin)
				return prevend;

			if (iter->next == iter) break;

			iter = iter->next;
		}

		{
			L4_Word_t lastend = __L4_NextAddress(iter->userpage);
			if (lastend < firstaddr) lastend = firstaddr;

			// round up to fpagesize
			if ((lastend & (fpagesize-1)) != 0)
			{
				lastend |= (fpagesize-1);
				lastend++;
			}
			
			assert((lastend & (fpagesize-1)) == 0);

			return lastend;
		}
	}

	/// print out the list
	inline void dump() const
	{
		if (!head) {
			printf("PageMappingList is empty\n");
			return;	
		}

		PageMappingEntry *iter = head;

		printf("PageMappingList dump:\n");
		while(1)
		{
			printPageMappingEntry(iter);

			if (iter->next == iter) break;
			iter = iter->next;
		}
	}

	/// print out a page mapping entry
	static inline void printPageMappingEntry(PageMappingEntry *pme)
	{
		if (!pme) {
			printf("pme: (null)\n");
		}
		else {
			printf("pme: addr 0x%08lx - 0x%08lx size %d => 0x%08x size %d\n",
				   L4_Address(pme->userpage),
				   __L4_EndAddress(pme->userpage),
				   static_cast<unsigned int>(L4_Size(pme->userpage)),
				   static_cast<unsigned int>(L4_Address(pme->backing)),
				   static_cast<unsigned int>(L4_Size(pme->backing)));
		}
	}
};

#endif // SDI2_PAGER_PAGEMAPPINGLIST_H
