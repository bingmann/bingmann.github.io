/*********************************************************************
 *                
 * Copyright (C) 2001-2004,  Karlsruhe University
 *                
 * File path:     sigma0.cc
 * Description:   sigma0 implementation
 *                
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *                
 * $Id: sigma0.cc,v 1.18.2.10 2004/03/02 21:52:11 skoglund Exp $
 *                
 ********************************************************************/
#include <l4/kip.h>
#include <l4/ipc.h>
#include <l4/misc.h>
#include <l4/kdebug.h>
#include <l4io.h>

//#define OLD_STYLE_MEMORY_REGIONS

#ifndef NULL
#define NULL ((void *) 0)
#endif



/**
 * Verbose level for sigma0 output.
 */
int verbose = 0;


/**
 * Pointer to sigma0's kernel interface page.
 */
L4_KernelInterfacePage_t * kip;


/**
 * Minimum page size (log2) supported by architecture/kernel
 * (initialized from kernel interface page).
 */
L4_Word_t min_pgsize = 10;


/**
 * Helpers for rounding pages up and down according to min_pgsize
 */
L4_INLINE L4_Word_t page_start (L4_Word_t page)
{
    return page & ~((1UL << min_pgsize) - 1);
}

L4_INLINE L4_Word_t page_end (L4_Word_t page)
{
    return (page + ((1UL << min_pgsize) - 1)) & ~((1UL << min_pgsize) - 1);
}

/*
 * Some API defined thread IDs.
 */
L4_ThreadId_t kernel_id;	// Lowes possible system thread
L4_ThreadId_t sigma0_id;
L4_ThreadId_t sigma1_id;
L4_ThreadId_t rootserver_id;


void init_mempool (void);
void dump_mempools (void);
bool allocate_page (L4_ThreadId_t tid, L4_Word_t addr, L4_Word_t log2size,
		    L4_MapItem_t & map, bool only_conventional = false);
bool allocate_page (L4_ThreadId_t tid, L4_Word_t log2size, L4_MapItem_t & map);


extern "C" __attribute__ ((weak)) void *
memcpy (void * dst, const void * src, unsigned int len)
{
    unsigned char *d = (unsigned char *) dst;
    unsigned char *s = (unsigned char *) src;

    while (len-- > 0)
	*d++ = *s++;

    return dst;
}


/*
 * Encoding for label field of message tag.
 */
#define L4_REQUEST_MASK		( ~((~0UL) >> ((sizeof (L4_Word_t) * 8) - 20)))
#define L4_PAGEFAULT		(-2UL << 20)
#define L4_SIGMA0_RPC		(-6UL << 20)
#define L4_SIGMA0_EXT		(-1001UL << 20)


/*
 * Encoding for MR1 of extended sigma0 protocol.
 */
#define L4_S0EXT_VERBOSE	(1)
#define L4_s0EXT_DUMPMEM	(2)


/**
 * Check if thread is a kernel thread.
 * @param t	thread id to check
 * @return true if thread is a kernel thread, false otherwise
 */
bool is_kernel_thread (L4_ThreadId_t t)
{
    return (t.raw < sigma0_id.raw) && (t.raw >= kernel_id.raw);
}


/**
 * Do printout if indicated verboseness exceeds or equals current
 * verbose level.
 *
 * @param v		verbose level to match
 * @param args...	arguments passed on to l4_printf()
 */
#define dprintf(v, args...)	do { if (verbose > (v)) l4_printf (args); } while(0)



/**
 * Main sigma0 loop.
 */
extern "C" void sigma0_main (void)
{
    L4_Word_t api_version, api_flags, kernelid;
    L4_MsgTag_t tag;

    dprintf (0, "s0: This is Sigma0\n");

    // Get kernel interface page.
    kip = (L4_KernelInterfacePage_t *)
	L4_KernelInterface (&api_version, &api_flags, &kernelid);

    dprintf (0, "s0: KIP @ %p (0x%lx, 0x%lx, 0x%lx)\n", 
	     kip, api_version, api_flags, kernelid);

    // Calculate API defined thread IDs.
    kernel_id = L4_GlobalId (L4_ThreadIdSystemBase (kip), 1);
    sigma0_id = L4_GlobalId (L4_ThreadIdUserBase (kip), 1);
    sigma1_id = L4_GlobalId (L4_ThreadIdUserBase (kip) + 1, 1);
    rootserver_id = L4_GlobalId (L4_ThreadIdUserBase (kip) + 2, 1);

    init_mempool ();

    L4_ThreadId_t tid;
    L4_Accept (L4_UntypedWordsAcceptor);

    tag = L4_Wait (&tid);

    for (;;)
    {
	L4_Msg_t msg;
	L4_MapItem_t map;
	L4_Fpage_t fpage;

	if (L4_UntypedWords (tag) != 2 || L4_TypedWords (tag) != 0)
	{
	    dprintf (0, "s0: malformed request from %p (tag=%p)\n", 
		     (void *) tid.raw, (void *) tag.raw);
	    L4_KDB_Enter ("s0: malformed request");
	    tag = L4_Wait (&tid);
	    continue;
	}
	
	L4_Store (tag, &msg);

	dprintf (1, "s0: got msg from %p, (0x%lx, %p, %p)\n", 
		 (void *) tid.raw, (long) L4_Label (tag),
		 (void *) L4_Get (&msg, 0), (void *) L4_Get (&msg, 1));

	/*
	 * Dispatch IPC according to protocol.
	 */

	switch (tag.raw & L4_REQUEST_MASK)
	{
	case L4_PAGEFAULT:
	{
	    if (! allocate_page (tid, L4_Get (&msg, 0), min_pgsize, map, true))
		dprintf (0, "s0: unhandled pagefault from %p @ %p, ip: %p\n",
			 (void *) tid.raw, (void *) L4_Get (&msg, 0),
			 (void *) L4_Get (&msg, 1));
	    break;
	}

	case L4_SIGMA0_RPC:
	{
	    fpage.raw = L4_Get (&msg, 0);
	    L4_Word_t addr = L4_Address (fpage);
	    L4_Word_t attributes = L4_Get (&msg, 1);

	    if (is_kernel_thread (tid))
	    {
		if (L4_Size (fpage) == 0)
		    // Request recomended kernel memory.
		    L4_KDB_Enter ("s0: recomended kernel mem");
		else
		    // Request kernel memory.
		    L4_KDB_Enter ("s0: request kernel memory");

		tag = L4_Wait (&tid);
		continue;
	    }
	    else
	    {
		if ((fpage.raw >> 10) == (~0UL >> 10))
		{
		    // Allocate from arbitrary location.
		    if (! allocate_page (tid, L4_SizeLog2 (fpage), map))
			dprintf (0, "s0: unable to allocate page of size %p"
				 " to %p\n", (void *) L4_Size (fpage),
				 (void *) tid.raw);
		}
		else
		{
		    // Allocate from specific location.
		    if (! allocate_page (tid, addr, L4_SizeLog2 (fpage),
			    	    	 map))
			dprintf (0, "s0: unable to allocate page %p of "
				 "size %p to %p\n", (void *) addr,
				 (void *) L4_Size (fpage),
				 (void *) tid.raw);
		}

		if (attributes != 0)
		{
		    // Set memory attributes before mapping.
		    if (! L4_Set_PageAttribute (L4_SndFpage (map), attributes))
		    {
			dprintf (1, "s0: memory control failed (%ld) setting page "
				 "%p with attributes %p",
				 L4_ErrorCode(), 
				 (void *) L4_Address (L4_SndFpage (map)),
				 (void *) attributes);
			map = L4_MapItem (L4_Nilpage, 0);
			L4_KDB_Enter ("s0: memory control failed!");
		    }
		}
	    } 
	    break;
	}

	case L4_SIGMA0_EXT:
	{
	    bool reply = false;
	    switch (L4_Get (&msg, 0))
	    {
	    case L4_S0EXT_VERBOSE:
		verbose = L4_Get (&msg, 1);
		break;
	    case L4_s0EXT_DUMPMEM:
		dump_mempools ();
		if (L4_Get (&msg, 1) != 0)
		    reply = true;
		break;
	    }

	    if (! reply)
		tag = L4_Wait (&tid);
	    else
	    {
		L4_Set_MsgTag (L4_Niltag);
		tag = L4_ReplyWait (tid, &tid);
	    }
	    continue;
	}

	default:
	    dprintf (0, "s0: unknown sigma0 reguest from %p, (%p, %p, %p)\n",
		     (void *) tid.raw, (void *) tag.raw,
		     (void *) L4_Get (&msg, 0), (void *) L4_Get(&msg, 1));
	    map = L4_MapItem (L4_Nilpage, 0);

	    tag = L4_Wait (&tid);
	    continue;
	}

	L4_Put (&msg, 0, 0, (L4_Word_t *) 0, 2, &map);
	L4_Load (&msg);
	tag = L4_ReplyWait (tid, &tid);
    }
}


/**
 * Descriptor for a memory region.  A memory region has a start
 * address, an end address, and an owner.
 */
class memregion_t
{
public:

    L4_Word_t		low;
    L4_Word_t		high;
    L4_ThreadId_t	owner;

    memregion_t		*prev;
    memregion_t		*next;

    memregion_t (void) {}
    memregion_t (L4_Word_t low, L4_Word_t high, L4_ThreadId_t owner);

    void * operator new (L4_Size_t size);

    void swap (memregion_t * n);
    void remove (void);
    bool is_adjacent (const memregion_t & r);
    bool concatenate (memregion_t * reg);

    bool can_allocate (L4_Word_t addr, L4_Word_t log2size,
		       L4_ThreadId_t tid = L4_anythread);
    L4_Fpage_t allocate (L4_Word_t addr, L4_Word_t log2size,
			 L4_ThreadId_t tid = L4_anythread);
    L4_Fpage_t allocate (L4_Word_t log2size,
			 L4_ThreadId_t tid = L4_anythread);
} __attribute__ ((packed));


/**
 * Opaque class for holding a memregion_t structure.
 */
class memregion_listent_t
{
    memregion_t	reg;

public:

    memregion_t * memreg (void)
	{ return &reg; }

    memregion_listent_t * next (void)
	{ return *(memregion_listent_t **) this; }

    void set_next (memregion_listent_t * n)
	{ *(memregion_listent_t **) this = n; }
} __attribute__ ((packed));


/**
 * List of memory regions.
 */
class memregion_list_t
{
    memregion_listent_t * list;

public:

    void add (L4_Word_t addr, L4_Word_t size);
    L4_Word_t contents (void);
    memregion_t * alloc (void);
    void free (memregion_t * r);
};


/**
 * A region pool is set of memory regions.
 */
class memregion_pool_t
{
    memregion_t first;
    memregion_t last;
    memregion_t * ptr;

public:

    void init (void);
    void dump (void);
    void insert (memregion_t * r);
    void remove (memregion_t * r);
    void insert (L4_Word_t low, L4_Word_t high, L4_ThreadId_t owner);
    void remove (L4_Word_t low, L4_Word_t high);
    void reset (void);
    memregion_t * next (void);
};



/**
 * List of free memregion_t structures.
 */
memregion_list_t memreg_list;


/**
 * Memory regions of conventional memory available for allocation.
 */
memregion_pool_t conv_memory_pool;


/**
 * Memory regions of non-conventional memory available for allocation.
 */
memregion_pool_t memory_pool;


/**
 * Memory regions already allocated.
 */
memregion_pool_t alloc_pool;





/* ================================================================
**
**			memregion_t
**
*/

memregion_t::memregion_t (L4_Word_t l, L4_Word_t h, L4_ThreadId_t o)
{
    low = l;
    high = h;
    owner = o;
}

void * memregion_t::operator new (L4_Size_t size)
{
    return (void *) memreg_list.alloc ();
}


/**
 * Swap memregion_t backing store.  The whole memregion_t structure is
 * copied and pointers for surrounding region structures are relocated
 * to the new location.
 *
 * @param r	location of memregion_t to use instead of current memory
 */
void memregion_t::swap (memregion_t * r)
{
    *r = *this;
    r->prev->next = r->next->prev = r;
}


/**
 * Remove memory region.  Region is first removed from the pool which
 * it is allocated to before its memory is freed.  Region must not be
 * accessed after it has been removed.
 */
void memregion_t::remove (void)
{
    prev->next = next;
    next->prev = prev;
    memreg_list.free (this);
}


/**
 * Check if supplied memory region is adjacent to current one.
 * @param r	memory region to check against
 * @return true it memory regions are adjacent, false otherwise
 */
bool memregion_t::is_adjacent (const memregion_t & r)
{
    return (low == r.high + 1 && low != 0) ||
	(high + 1 == r.low && r.low != 0);
}


/**
 * Concatenate supplied memory region with current one.
 *
 * @param r	memory region to concatenate with
 *
 * @return true if concatenation was successful, false otherwise
 */
bool memregion_t::concatenate (memregion_t * r)
{
    if (owner != r->owner)
	return false;

    if (low == r->high + 1 && low != 0)
	low = r->low;
    else if (high + 1 == r->low && r->low != 0)
	high = r->high;
    else
	return false;

    return true;
}


/**
 * Try allocating part from memory region.  If allocation is
 * successful, current memregion_t might be deleted or split up into
 * multiple regions.
 *
 * @param log2size	size of region to allocate
 * @param tid		thread id to use for allocation
 *
 * @return fpage for allocated region if successful, nilpage otherwise
 */
L4_Fpage_t memregion_t::allocate (L4_Word_t log2size, L4_ThreadId_t tid)
{
    L4_Word_t size = 1UL << log2size;
    L4_Fpage_t ret;

    // Low and high address of region within mwmregion when they are
    // aligned according to log2size.  Note that these values might
    // overflow and must as such be handled with care.
    L4_Word_t low_a = (low + size - 1) & ~(size-1);
    L4_Word_t high_a = ((high + 1) & ~(size-1)) - 1;

    if (low_a > high_a			// Low rounded up to above high
	|| low > low_a			// Low wrapped around
	|| high < size-1		// High wrapper around
	|| (high_a - low_a) < size-1	// Not enough space in memregion
	|| (owner != tid && owner != L4_anythread))
    {
	// Allocation failed
	ret = L4_Nilpage;
    }
    else if (low_a == low)
    {
	// Allocate from start of region
	ret = L4_FpageLog2 (low, log2size) + L4_FullyAccessible;
	if (low + size == high + 1)
	    remove ();
	else
	    low += size;
    }
    else if (high_a == high)
    {
	// Allocate from end of region
	ret = L4_FpageLog2 (high_a - size + 1, log2size) + L4_FullyAccessible;
	high -= size;
    }
    else
    {
	// Allocate from middle of region
	ret = L4_FpageLog2 (low_a, log2size) + L4_FullyAccessible;
	memregion_t * r = new memregion_t (low_a + size, high, owner);
	r->next = next;
	r->prev = this;
	r->next->prev = next = r;
	high = low_a - 1;
    }

    return ret;
}


/**
 * Try allocating part from memory region.  If allocation is
 * successful, current memregion_t might be deleted or split up into
 * multiple regions.
 *
 * @param addr		location of region to allocate
 * @param log2size	size of region to allocate
 * @param tid		thread id to use for allocation
 *
 * @return fpage for allocated region if successful, nilpage otherwise
 */
L4_Fpage_t memregion_t::allocate (L4_Word_t addr, L4_Word_t log2size,
				  L4_ThreadId_t tid)
{
    L4_Word_t size = 1UL << log2size;
    L4_Fpage_t ret;

    // Low and high address of region within mwmregion when they are
    // aligned according to log2size.  Note that these values might
    // overflow and must as such be handled with care.
    L4_Word_t low_a = (low + size - 1) & ~(size-1);
    L4_Word_t high_a = ((high + 1) & ~(size-1)) - 1;

    if (addr < low_a			// Address range below low
	|| (addr + size - 1) > high_a	// Address range above high
	|| (high_a - low_a) < size-1	// Not enough space in memregion
	|| low > low_a			// Low wrapped around
	|| high < size-1		// High wrapper around
	|| (owner != tid && owner != L4_anythread))
    {
	// Allocation failed
	ret = L4_Nilpage;
    }
    else if (low_a == low && addr == low)
    {
	// Allocate from start of region
	ret = L4_FpageLog2 (low, log2size) + L4_FullyAccessible;
	if (low + size == high + 1)
	    remove ();
	else
	    low += size;
    }
    else if (high_a == high && (addr + size - 1) == high)
    {
	// Allocate from end of region
	ret = L4_FpageLog2 (high_a - size + 1, log2size) + L4_FullyAccessible;
	high -= size;
    }
    else
    {
	// Allocate from middle of region
	ret = L4_FpageLog2 (addr, log2size) + L4_FullyAccessible;
	memregion_t * r = new memregion_t (addr + size, high, owner);
	r->next = next;
	r->prev = this;
	r->next->prev = next = r;
	high = addr - 1;
    }

    return ret;
}


/**
 * Check if it is possible to allocate from memory region.
 *
 * @param addr		location of region to allocate
 * @param log2size	size of region to allocate
 * @param tid		thread id to use for allocation
 *
 * @return true if allocation is possible, false otherwise
 */
bool memregion_t::can_allocate (L4_Word_t addr, L4_Word_t log2size,
				L4_ThreadId_t tid)
{
    L4_Word_t size = 1UL << log2size;

    // Low and high address of region within mwmregion when they are
    // aligned according to log2size.  Note that these values might
    // overflow and must as such be handled with care.
    L4_Word_t low_a = (low + size - 1) & ~(size-1);
    L4_Word_t high_a = ((high + 1) & ~(size-1)) - 1;

    if (addr < low_a			// Address range below low
	|| (addr + size - 1) > high_a	// Address range above high
	|| (high_a - low_a) < size-1	// Not enough space in memregion
	|| low > low_a			// Low wrapped around
	|| high < size-1		// High wrapper around
	|| (owner != tid && owner != L4_anythread))
	return false;
    else
	return true;
}



/* ================================================================
**
**			memregion_list_t
**
*/


/**
 * Add more memory to be used for memregion_t structures.
 *
 * @param addr	location of memory to add
 * @param size	amount of memory to add
 */
void memregion_list_t::add (L4_Word_t addr, L4_Word_t size)
{
    if (addr == 0)
    {
	// Avoid inserting a NULL pointer into the list.
	addr += sizeof (memregion_listent_t);
	size -= sizeof (memregion_listent_t);
    }

    memregion_listent_t * m = (memregion_listent_t *) addr;

    for (; (L4_Word_t) (m+1) < addr + size; m++)
	m->set_next (m+1);

    m->set_next (list);
    list = (memregion_listent_t *) addr;
}


/**
 * @return number of memregion_t structures in pool
 */
L4_Word_t memregion_list_t::contents (void)
{
    L4_Word_t n = 0;
    for (memregion_listent_t * m = list; m != NULL; m = m->next ())
	n++;
    return n;
}


/**
 * Allocate a memregion_t structure.
 * @return newly allocated structure
 */
memregion_t * memregion_list_t::alloc (void)
{
    if (! list)
    {
	// We might need some memory for allocating memory.
	memregion_t tmp;
	add ((L4_Word_t) &tmp, sizeof (tmp));

	// Allocate some memory to sigma0.
	L4_MapItem_t dummy;
	if (! allocate_page (sigma0_id, min_pgsize, dummy))
	{
	    l4_printf ("s0: Unable to allocate memory.\n");
	    for (;;)
		L4_KDB_Enter ("s0: out of memory");
	}

	bool was_alloced = (list == NULL);
	if (! was_alloced)
	    list = (memregion_listent_t *) NULL;

	// Add newly allocated memory to pool.
	add (L4_Address (L4_SndFpage (dummy)), (1UL << min_pgsize));

	if (was_alloced)
	    // Swap temorary structure with a newly allocated one.
	    tmp.swap (alloc ());
    }

    // Remove first item from free list.
    memregion_listent_t * r = list;
    list = r->next ();

    return r->memreg ();
}


/**
 * Free a memregion_t structure.
 * @param r	memregion structure to free
 */
void memregion_list_t::free (memregion_t * r)
{
    memregion_listent_t * e = (memregion_listent_t *) r;
    e->set_next (list);
    list = e;
}


/* ================================================================
**
**			memregion_pool_t
**
*/


/**
 * Initialize the memory pool structure.  Must be done prior to any
 * insertions into the pool.
 */
void memregion_pool_t::init (void)
{
    first.next = first.prev = &last;
    last.next = last.prev = &first;
    first.low = first.high = 0;
    last.low = last.high = ~0UL;
    first.owner = last.owner = L4_nilthread;
    ptr = &last;
}


/**
 * Insert region into region pool.  Concatenate region with existing
 * regions if possible.
 *
 * @param r	region to insert into pool
 */
void memregion_pool_t::insert (memregion_t * r)
{
    memregion_t * p = &first;
    memregion_t * n = first.next;

    // Find correct insert location
    while (r->low > n->high)
    {
	p = n;
	n = n->next;
    }

    if (p->concatenate (r))
    {
	// Region concatenated previous one
	memreg_list.free (r);
	if (p->concatenate (n))
	    remove (n);
    }
    else if (n->concatenate (r))
    {
	// Region concatenated to next one
	memreg_list.free (r);
    }
    else
    {
	// No concatenation possible.  Insert into list
	r->next = n;
	r->prev = p;
	p->next = n->prev = r;
    }
}


/**
 * Remove region from region pool.  It is assumed that the region is
 * indeed contained in the pool.  Region must not be accessed after it
 * has been removed from pool.
 */
void memregion_pool_t::remove (memregion_t * r)
{
    r->remove ();
}


/**
 * Insert specified region into memory pool.  Concatenate with
 * existing regions if possible.
 *
 * @param low		lower limit of memory region
 * @param high		upper limit of memory region
 * @param owner		owner of memory region
 */
void memregion_pool_t::insert (L4_Word_t low, L4_Word_t high,
			       L4_ThreadId_t owner)
{
    insert (new memregion_t (low, high, owner));
}


/**
 * Remove specified region from memory pool.
 *
 * @param low		lower limit of memory region to remove
 * @param high		upper limit of memory region to remove
 */
void memregion_pool_t::remove (L4_Word_t low, L4_Word_t high)
{
    memregion_t * n = first.next;

    while (low > n->high)
	n = n->next;

    while (n != &last)
    {
	if (low <= n->low && high >= n->high)
	{
	    // Remove whole region node.
	    n = n->next;
	    remove (n->prev);
	}
	else if (low <= n->low && high >= n->low)
	{
	    // Only need to modify lower limit.
	    n->low = high + 1;
	    break;
	}
	else if (low > n->low && low <= n->high)
	{
	    // Need to modify upper limit.
	    L4_Word_t old_high = n->high;
	    n->high = low - 1;
	    if (high < old_high)
	    {
		// Must split region into two separate regions.
		insert (high + 1, old_high, n->owner);
		break;
	    }
	    n = n->next;
	}
	else
	    n = n->next;
    }
}


/**
 * Dump contents of memory region pool.
 */
void memregion_pool_t::dump (void)
{
    memregion_t * r;
    reset ();
    while ((r = next ()) != NULL)
    {
	l4_printf ("s0:  %p-%p   %p %s\n",
		(void *) r->low, (void *) r->high,
		(void *) r->owner.raw,
		r->owner == sigma0_id ? "(sigma0)" :
		r->owner == sigma1_id ? "(sigma1)" :
		r->owner == rootserver_id ? "(root server)" :
		is_kernel_thread (r->owner) ? "(kernel)" :
		r->owner == L4_anythread ? "(anythread)" :
		"");
    }
}

void memregion_pool_t::reset (void)
{
    ptr = first.next;
}

memregion_t * memregion_pool_t::next (void)
{
    if (ptr == &last)
	return (memregion_t *) NULL;
    memregion_t * ret = ptr;
    ptr = ptr->next;
    return ret;
}



/* ================================================================
**
**		Memory region allocation interface
**
*/

static memregion_t initial_memregs[32];

void register_memory (L4_Word_t low, L4_Word_t high, L4_ThreadId_t t)
{
    // Align to smallest page size.
    low = page_start (low);
    high = page_end (high);

    L4_MapItem_t map;
    L4_Word_t addr = low;
    while (addr < high)
    {
	if (! allocate_page (t, addr, min_pgsize, map))
	    dprintf (1, "s0: alloc <%p,%p> to %p failed.\n",
		     (void *) low, (void *) high, (void *) t.raw);
	addr += (1UL << min_pgsize);
    }
}

#if defined(OLD_STYLE_MEMORY_REGIONS)

/*
 * Initialization for old style memory regions.
 */

static void init_mempool_from_kip (void)
{
    // Insert conventional memory.
    conv_memory_pool.insert (kip->MainMem.low, kip->MainMem.high - 1,
			     L4_anythread);

    // Insert rest of memory into non-conventional memory pool.
    if (kip->MainMem.low != 0)
	memory_pool.insert (0, kip->MainMem.low, L4_anythread);
    memory_pool.insert (kip->MainMem.high, ~0UL, L4_anythread);

    dprintf (0, "s0: KIP: %-16s = [%p - %p]\n", "MainMem",
	     (void *) kip->MainMem.low, (void *) kip->MainMem.high);

#define reserve_memory(name, t)						\
    if (kip->name.high)							\
    {									\
	dprintf (0, "s0: KIP: %-16s = [%p - %p]\n", #name,		\
		 (void *) kip->name.low, (void *) kip->name.high);	\
	register_memory (kip->name.low, kip->name.high, t);		\
    }									\
    else 								\
	dprintf (0, "s0: KIP: %-16s = [uninitialized]\n", #name)

#define alloc_memory(name, idx, t)				\
    if (((L4_Word_t *) kip)[idx + 1])				\
    {								\
	L4_Word_t * desc = &((L4_Word_t *) kip)[idx];		\
	dprintf (0, "s0: KIP: %-16s = [%p - %p]\n", #name,	\
		 (void *) desc[0], (void *) desc[1]);		\
	register_memory (desc[0], desc[1], t);			\
    }								\
    else 							\
	dprintf (0, "s0: KIP: %-16s = [uninitialized]\n", #name)

    reserve_memory (DedicatedMem[0], L4_anythread);
    reserve_memory (DedicatedMem[1], L4_anythread);
    reserve_memory (DedicatedMem[2], L4_anythread);
    reserve_memory (DedicatedMem[3], L4_anythread);
    reserve_memory (DedicatedMem[4], L4_anythread);

    reserve_memory (ReservedMem[0], kernel_id);
    reserve_memory (ReservedMem[1], kernel_id);

    alloc_memory (Kdebug, 6, kernel_id);
    alloc_memory (Sigma0, 10, sigma0_id);
    alloc_memory (Sigma1, 14, sigma1_id);
    alloc_memory (RootServer, 18, rootserver_id);
}
#else

/*
 * Inialization for new style memory regions.
 */

static void init_mempool_from_kip (void)
{
    L4_MemoryDesc_t * md;

    // Initialize memory pool with complete physical address space.
    memory_pool.insert (0, ~0UL, L4_anythread);

    // Parse through all memory descriptors in kip.
    for (L4_Word_t n = 0; (md = L4_MemoryDesc (kip, n)); n++)
    {
	if (L4_IsVirtual (md))
	    continue;

	L4_Word_t low = page_start (L4_MemoryDescLow (md));
	L4_Word_t high = page_end (L4_MemoryDescHigh (md)) - 1;

	conv_memory_pool.remove (low, high);
	memory_pool.remove (low, high);
	alloc_pool.remove (low, high);

	if ((L4_MemoryDescType (md) &0xf) == L4_BootLoaderSpecificMemoryType ||
	    (L4_MemoryDescType (md) &0xf) == L4_ArchitectureSpecificMemoryType)
	{
	    /*
	     * Boot loader or architecture dependent memory.  Remove
	     * from conventional memory pool and insert into
	     * non-conventional memory pool.
	     */
	    memory_pool.insert (low, high, L4_anythread);
	}
	else
	{
	    switch (L4_MemoryDescType (md))
	    {
	    case L4_UndefinedMemoryType:
		break;
	    case L4_ConventionalMemoryType:
		conv_memory_pool.insert (low, high, L4_anythread);
		break;
	    case L4_ReservedMemoryType:
		alloc_pool.insert (low, high, kernel_id);
		break;
	    case L4_DedicatedMemoryType:
		memory_pool.insert (low, high, L4_anythread);
		break;
	    case L4_SharedMemoryType:
		alloc_pool.insert (low, high, L4_anythread);
		break;
	    default:
		dprintf (0, "s0: Unknown memory type (0x%x)\n",
			 (int) L4_MemoryDescType (md));
		break;
	    }
	}
    }

#define alloc_memory(name, idx, t)				\
    if (((L4_Word_t *) kip)[idx + 1])				\
    {								\
	L4_Word_t * desc = &((L4_Word_t *) kip)[idx];		\
	register_memory (desc[0], desc[1], t);			\
    }

    // Allocate memory to initial servers.
    alloc_memory (Kdebug, 6, kernel_id);
    alloc_memory (Sigma0, 10, sigma0_id);
    alloc_memory (Sigma1, 14, sigma1_id);
    alloc_memory (RootServer, 18, rootserver_id);
}
#endif


/**
 * Initialze the various memory pools.
 */
void init_mempool (void)
{
    L4_Word_t psmask = L4_PageSizeMask (kip);

    if (psmask == 0)
    {
	l4_printf ("s0: Page-size mask in KIP is empty!\n");
	for (;;)
	    L4_KDB_Enter ("s0: no page-size mask");
    }

    // Determine minimum page size
    for (L4_Word_t m = (1UL << min_pgsize);
	 (m & psmask) == 0;
	 m <<= 1, min_pgsize++)
	;

    // Add some initial memregion_t structures to pool
    memreg_list.add ((L4_Word_t) initial_memregs, sizeof (initial_memregs));

    // Initialize memory pools
    conv_memory_pool.init ();
    memory_pool.init ();
    alloc_pool.init ();

    init_mempool_from_kip ();
}


/**
 * Dump contents of memory pools.
 */
void dump_mempools (void)
{
    l4_printf ("s0: Free memregion structures: %d\n",
	    (int) memreg_list.contents ());
    l4_printf ("s0:\ns0: Free pool (conventional memory):\n");
    conv_memory_pool.dump ();
    l4_printf ("s0:\ns0: Free pool (non-conventional memory):\n");
    memory_pool.dump ();
    l4_printf ("s0:\ns0: Alloc pool:\n");
    alloc_pool.dump ();
}


/**
 * Allocate a page located at a specific address.  Address need not be
 * within conventional memory range.
 *
 * @param tid		id of thread doing allocation
 * @param addr		location of page to allocate
 * @param log2size	size of page to allocate
 * @param map		genrated map item corresponding to alloced page
 * @param only_conv	only try to allocation from conventional memory pool
 *
 * @return true upon success, false otherwise
 */
bool allocate_page (L4_ThreadId_t tid, L4_Word_t addr, L4_Word_t log2size,
		    L4_MapItem_t & map, bool only_conventional)
{
    memregion_t * r;
    L4_Fpage_t fp;

    map = L4_MapItem (L4_Nilpage, 0);

    dprintf (2, "s0: allocate_page (tid: 0x%lx, addr: %lx, log2size: %ld)\n",
	     (long) tid.raw, (long) addr, (long) log2size);

    if (log2size < min_pgsize)
	return false;

    // Make sure that addr is properly aligned.
    addr &= ~((1UL << log2size) - 1);
    L4_Word_t addr_high = addr + (1UL << log2size) - 1;

    memregion_pool_t * pools[] = { &conv_memory_pool, &memory_pool,
				   (memregion_pool_t *) NULL };

    // Check if we only want to try the conventional memory pool
    if (only_conventional)
	pools[1] = (memregion_pool_t *) NULL;

    // Try allocating from one of the memory pools.
    for (L4_Word_t i = 0; pools[i] != NULL; i++)
    {
	pools[i]->reset ();
	while ((r = pools[i]->next ()) != NULL)
	{
	    if (r->low > addr_high || r->high < addr)
		continue;

	    fp = r->allocate (addr, log2size);
	    if (! L4_IsNilFpage (fp))
	    {
		map = L4_MapItem (fp, addr);
		alloc_pool.insert 
		    (new memregion_t (addr, addr_high, tid));
		return true;
	    }
	}
    }

    // Check if memory has already been allocated.
    alloc_pool.reset ();
    while ((r = alloc_pool.next ()) != NULL)
    {
	if (r->can_allocate (addr, log2size, tid))
	{
	    map = L4_MapItem
		(L4_FpageLog2 (addr, log2size) + L4_FullyAccessible, addr);
	    return true;
	}
    }

    // If all the above failed we have to try the slow way of
    // allocating parts of memory from different pools.
    memregion_pool_t * allpools[] = { &conv_memory_pool,
				      &alloc_pool,
				      &memory_pool,
				      (memregion_pool_t *) NULL };

    // Check if we only want to try the conventional memory pool
    if (only_conventional)
	allpools[2] = (memregion_pool_t *) NULL;

    // Loop once for checking followed by once for allocating
    for (L4_Word_t phase = 0; phase < 2; phase++)
    {

	// Use smallest page size as stepping
	for (L4_Word_t a = addr; a < addr_high; a += (1UL << min_pgsize))
	{
	    L4_Word_t a_end = a + (1UL << min_pgsize);
	    bool failed = true;

	    // Try the different pools
	    for (L4_Word_t i = 0; failed && allpools[i] != NULL; i++)
	    {
		allpools[i]->reset ();
		while ((r = allpools[i]->next ()) != NULL)
		{
		    if (r->low > a_end || r->high < a)
			continue;

		    // Test if allocation is possible
		    if (r->can_allocate (a, min_pgsize, tid))
		    {
			failed = false;
			if (phase == 1 && allpools[i] != &alloc_pool)
			{
			    // Allocation phase
			    r->allocate (a, min_pgsize, tid);
			    alloc_pool.insert 
				(new memregion_t (a, a_end - 1, tid));
			}
		    }
		}
	    }

	    if (failed)
		return false;
	}
    }

    map = L4_MapItem (L4_FpageLog2 (addr, log2size) + L4_FullyAccessible,
		      addr);

    return true;
}


/**
 * Allocate a page of conventional memory.  Page may be allocated from
 * arbitrary address.
 *
 * @param tid		id of thread doing allocation
 * @param log2size	size of page to allocate
 * @param map		genrated map item corresponding to alloced page
 *
 * @return true upon success, false otherwise
 */
bool allocate_page (L4_ThreadId_t tid, L4_Word_t log2size, L4_MapItem_t & map)
{
    memregion_t * r;
    L4_Fpage_t fp;

    map = L4_MapItem (L4_Nilpage, 0);

    dprintf (2, "s0: allocate_page (tid: 0x%lx, log2size: %ld)\n",
	     (long) tid.raw, (long) log2size);

    if (log2size < min_pgsize)
	return false;

    // Try allocating memory from pool of real memory.
    conv_memory_pool.reset ();
    while ((r = conv_memory_pool.next ()) != NULL)
    {
	fp = r->allocate (log2size);
	if (! L4_IsNilFpage (fp))
	{
	    map = L4_MapItem (fp, L4_Address (fp));
	    alloc_pool.insert
		(new memregion_t (L4_Address (fp), L4_Address (fp) +
				  (1UL << log2size) - 1, tid));
	    return true;
	}
    }

    return false;
}
