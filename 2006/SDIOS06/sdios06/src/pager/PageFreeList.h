// $Id: PageFreeList.h 126 2006-07-23 07:09:38Z sdi2 $

#ifndef SDI2_PAGER_PAGEFREELIST_H
#define SDI2_PAGER_PAGEFREELIST_H

#include <assert.h>
#include "SlabAllocator.h"

/** the node struct of the doubly-linked free list used to represent a page */
struct PageFreeEntry
{
	struct PageFreeEntry *prev, *next;
	L4_Fpage_t	page;
};

/** PageFreeList holds a doubly-linked list of free fpages in a slab
 * allocator. The free pages are sorted by addresses in ascending order. */

class PageFreeList
{
private:

	/// slab allocator type used by the free list
	typedef SlabAllocator<PageFreeEntry> 	slaballoc_t;

	/// reference to the used slab allocator
	slaballoc_t::Pool*		slaballoc;

	/// the head of the doubly-linked free list
	struct PageFreeEntry	*head;

public:
	/// create a free list backed by the given allocator
	explicit inline PageFreeList(slaballoc_t::Pool *usedallocator = NULL)
		: slaballoc(usedallocator), head(NULL)
	{
	}

	/// needed by the BuddySystem
	inline void setSlabAllocator(slaballoc_t::Pool &sa)
	{
		slaballoc = &sa;
	}

	/// test invariants of the list
	inline void test() const
	{
		if (!head) return;

		class PageFreeEntry* iter = head;
		assert(iter->prev == iter);

		while(iter->next != iter)
		{
			assert(iter->next->prev == iter);

			assert(L4_Address(iter->page) <= L4_Address(iter->next->page));

			iter = iter->next;
		}
	}

	/// returns the head of the doubly-linked list
	inline const struct PageFreeEntry* begin() const
	{ return head; }

	/// insert an fpage into the free list
	inline bool insert(L4_Fpage_t p)
	{
		test();

		// create a new node and insert it into the linked list at the right
		// place.
		PageFreeEntry *newnode = slaballoc->allocate();
		if (!newnode) {
			assert(0);
			return false;
		}

		newnode->page = p;

		if (!head)
		{
			// first node
			newnode->next = newnode;
			newnode->prev = newnode;
			head = newnode;
		}
		else
		{
			PageFreeEntry *iter = head;

			L4_Word_t findaddr = L4_Address(p);
			
			while(findaddr >= L4_Address(iter->page))
			{
				if (iter->next == iter)
				{
					// insert the node at the end. as it has highest address
					iter->next = newnode;
					newnode->next = newnode;
					newnode->prev = iter;

					test();
					return true;
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
		}

		test();

		return true;
	}

	/// optimized insert if the value from findNearest can be provided
	inline bool insertNear(L4_Fpage_t p, struct PageFreeEntry *iter)
	{
		// create a new node and insert it into the linked list using iter: the
		// correct position is either to the left or the right of iter.
		PageFreeEntry *newnode = slaballoc->allocate();
		if (!newnode) {
			assert(0);
			return false;
		}

		newnode->page = p;

		if (!iter)
		{
			// insert as first item in the list
			head = newnode;
			newnode->next = newnode;
			newnode->prev = newnode;
		}
		else if (iter->next == iter && L4_Address(iter->page) <= L4_Address(p))
		{
			// insert the node at the end. as it has highest address

			assert(L4_Address(p) >= L4_Address(iter->page));

			iter->next = newnode;
			newnode->next = newnode;
			newnode->prev = iter;
		}
		else if (iter->prev == iter)
		{
			// insert as first node

			assert(L4_Address(p) <= L4_Address(iter->page));

			assert(head == iter);

			newnode->prev = newnode;
			newnode->next = iter;

			iter->prev = newnode;
			head = newnode;
		}
		else
		{
			// somewhere in the middle
			assert(L4_Address(p) < L4_Address(iter->page));
			assert(L4_Address(p) >= L4_Address(iter->prev->page));

			iter->prev->next = newnode;
			newnode->next = iter;

			newnode->prev = iter->prev;
			iter->prev = newnode;
		}

		test();

		return true;
	}

	/// remove an fpage from the free list given by the entry ptr
	inline void remove(struct PageFreeEntry *e)
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

	/// returns the first entry > than the given page, or the last page if none
	/// can be found. returns NULL if the list is empty.
	inline struct PageFreeEntry *findNearest(L4_Fpage_t p)
	{
		if (!head) return NULL;

		PageFreeEntry *iter = head;

		L4_Word_t findaddr = L4_Address(p);
			
		while(findaddr >= L4_Address(iter->page))
		{
			if (iter->next == iter)
				return iter;

			iter = iter->next;
		}

		return iter;
	}

	/// return the first and remove it from the list
	inline L4_Fpage_t getFirstPage()
	{
		if (!head) return L4_Nilpage;

		PageFreeEntry *use = head;

		if (head->next == head) {
			head = NULL;
		}
		else {
			head->next->prev = head->next;
			head = head->next;
		}

		L4_Fpage_t p = use->page;

		slaballoc->freeobj(use);

		return p;
	}

	/// print out the freelist
	inline void dump() const
	{
		if (!head) {
			printf("PageFreeList is empty\n");
			return;
		}

		PageFreeEntry *iter = head;

		while(1)
		{
			printf("pfl: addr 0x%08x size %d\n",
				   static_cast<unsigned int>(L4_Address(iter->page)),
				   static_cast<unsigned int>(L4_Size(iter->page)));

			if (iter->next == iter) break;
			iter = iter->next;
		}
	}
};

#endif // SDI2_PAGER_PAGEFREELIST_H
