// $Id: SlabAllocator.h 126 2006-07-23 07:09:38Z sdi2 $

#ifndef SDI2_PAGER_SLABALLOCATOR_H
#define SDI2_PAGER_SLABALLOCATOR_H

#include <new>
#include <assert.h>

/** template SlabAllocator which is used by the pager for it's dynamic data
 * structures, because the pager has no heap. */

template <typename Object, unsigned int _pagesize = 4096>
class SlabAllocator
{
public:

    /// forward declarations
	class Pool;

	// *** typedefs
	typedef Object	Data;

	typedef Data*	DataPtr;

	// *** calculations to get the number of objects per slab page right

	static const unsigned int pagesize = _pagesize;

	static const unsigned int pageheadersize = 2 * sizeof(class Page*)
		+ sizeof(unsigned int)
		+ sizeof(Data*);
	
	static const unsigned int datarestsize = pagesize - pageheadersize;

	static const unsigned int objperpage =  datarestsize / sizeof(Data);

	static const unsigned int remainingbytes = datarestsize
	    - objperpage * sizeof(Data);

    /** SlabAllocator::Page is a class sized to _pagesize bytes which contains
	 * n objects of the same size. The class holds a free list to quickly
	 * allocate new objects from the slab page. */

	class Page
	{
	protected:
		// *** header structure

		/// pointers to the previous and next slab page
		class Page		*prev, *next;

		friend class Pool;

		/// number of used objects in this slab
		unsigned int	usednum;

		/// total number of objects in a slab
		static const unsigned int objnum = objperpage;

		/// pointer to the first free object in this slab
		Data*			firstfree;
		
		// *** body data
		
		Data			body[objperpage];

		// *** include remaining bytes so the sizeof(Page) == pagesize

		// this does not work with g++-4.0.2. reason unknown.
		// Matze: seems to work fine with my g++ 4.1.1 here...
		// char			remaining[remainingbytes];
	
		// *** functions

		/// returns a re-cast body object which only contains a pointer when
		/// free
		inline Data** bodyfreeptr(unsigned int i)
		{
			return reinterpret_cast<Data**>(&body[i]);
		}

		/// returns the pointer in a free body object.
		static inline Data* freeptr(Data *data)
		{
			return *reinterpret_cast<Data**>(data);
		}

	public:
		// initializes the freelist
		inline Page()
			: prev(NULL), next(NULL),
			  usednum(0), firstfree(NULL)
		{
			firstfree = &body[0];

			for(unsigned int i = 0; i < objnum-1; i++)
			{
				*bodyfreeptr(i) = &body[i+1];
			}

			*bodyfreeptr(objnum-1) = NULL;
		}

		/// return the number of used objects
		inline unsigned int used() const
		{ return usednum; }

		/// return the number of free objects
		inline unsigned int available() const
		{ return objnum - usednum; }
		
		/// checks if the page can contain the given object address
		inline bool contains(Data *obj) const
		{
			return (&body[0] <= obj && obj <= &body[objnum-1]);
		}

		/// allocate a free object from the freelist
		inline Data* allocate()
		{
			if (firstfree == NULL) { assert(0); return NULL; }

			Data* obj = firstfree;

			// advance free pointer
			firstfree = freeptr(firstfree);
			usednum++;

			// call object constructor
			new (obj) Data();

			return obj;
		}

		/// readd an object to the free list
		inline void freeobj(Data *obj)
		{
			assert(contains(obj));
			assert(usednum > 0);
			assert(!isFree(obj));

			// call destructor
			obj->~Data();

			// reinsert the object as the first in the free list

			*reinterpret_cast<Data**>(obj) = firstfree;

			firstfree = obj;

			usednum--;
		}

		/// checks that the given object is on the free list
		inline bool isFree(Data* obj) const
		{
			Data* iter = firstfree;

			while(iter != NULL)
			{
				if (obj == iter) return true;

				iter = freeptr(iter);
			}

			return false;
		}

		/// test invariants
		inline void test() const
		{
			// count free objects.
			unsigned int fonum = 0;
			
			Data* fo = firstfree;
			while(fo != NULL) {
				fonum++;
				fo = freeptr(fo);
			}
			
			assert(fonum == available());
		}
	};

	/** SlabAllocator::Pool manages the doubly linked list of pages and has
	 * front-end functions to allocate and free objects from the slab pages. */

	class Pool
	{
	private:
        /// points to the head of the doubly linked list
		class Page		*head;

		/// allocation callback typedef
		typedef void*	(*alloccallback_t)(unsigned int pagesize);

		/// backing allocator if more slab pages are needed.
		alloccallback_t	alloccallback;
		
		/// request a pagea from the backing allocator
		bool requestMoreMemory()
		{
			void *newmem = alloccallback(pagesize);
			if (!newmem) return false;

			class Page* newpage = static_cast<class Page*>(newmem);

			// in-place construction
			new (newpage) Page();
			
			addPage(newpage);
			return true;
		}
		
	public:
		/// initialize an empty slab page pool
		Pool()
			: head(NULL), alloccallback(NULL)
		{
			assert(sizeof(Page) <= _pagesize);
		}
		
		/// initialize an empty slab page pool and insert add an array of pages
		Pool(class Page* pfirst, unsigned int pnum=1)
			: head(NULL), alloccallback(NULL)
		{
			assert(sizeof(Page) <= _pagesize);
			for(unsigned int pi = 0; pi < pnum; pi++) {
				addPage(pfirst+pi);
			}
		}

		/// set a callback function when more slab pages are required. the
		/// callback should allocate page.
		void setAllocCallback(alloccallback_t acb)
		{
			alloccallback = acb;
		}
		
		/// add a slab page to the pool
		void addPage(class Page* p)
		{
			// put the new page into the first position

			if (head) {
				p->next = head;
				head->prev = p;
			}
			else {
				p->next = p;
			}

			p->prev = p;
			
			head = p;
		}

		/// test doubly linked list
		void test() const
		{
			if (!head) return;

			class Page* i = head;
			assert(i->prev == i);
			
			while(1)
			{
				assert(i->next->prev = i);
				
				i->test();

				if (i->next == i) break;
				i = i->next;
			}
		}

		/// get a free object from one of the slab pages. returns NULL if no
		/// object can be allocated.
		Data* allocate()
		{
			class Page* iter = head;
			if (!iter) {
				panic("pager: out of slab memory - no pages");
				return NULL;
			}

			while(iter->available() == 0)
			{
				if (iter->next == iter) {
					// no slab page has a free entry
					if (alloccallback)
					{
						if (!requestMoreMemory()) return NULL;
						return allocate(); // restart function
					}
					panic("pager: out of slab memory");
					return NULL;
				}
				iter = iter->next;
			}

			return iter->allocate();
		}

		/// return an object into the free list of the slab
		bool freeobj(Data* obj)
		{
			class Page* iter = head;
			if (!iter) return false;

			while(!iter->contains(obj))
			{
				if (iter->next == iter) return false;
				iter = iter->next;
			}

			iter->freeobj(obj);
			return true;
		}
	};
};

#endif // SDI2_PAGER_SLABALLOCATOR_H
