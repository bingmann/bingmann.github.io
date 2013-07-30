/*
 * $Id$
 */

#undef DEBUG_MALLOC

#include <config.h>

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <sdi/panic.h>
#include <sdi/util.h>

#ifdef TESTHEAP

#define malloc sdi_malloc
#define free sdi_free
#define realloc sdi_realloc
#define sbrk sdi_sbrk

#else

#include <idl4glue.h>
#include <if/ifmemory.h>

#endif

/* Heap Managment
 * 
 * Datastructures:
 * Space on the heap is divided into blocks which are all prefixed with the
 * MemoryBlock struct as header. Additionally free blocks are kept in a linked
 * list...
 * 
 * TODO: call sbrk and shrink memory area on free...
 * TODO: Not threadsafe yet
 */
extern "C" {
	
// define if you want slower but error detecting heap management
//#define DEBUG_HEAP
// define if you want to see what allocation decisions are taken
//#define SHOW_INFO

static const uint32_t FREE_MAGIC = 0x7f8a;
static const uint32_t ALLOCATED_MAGIC = 0x9ea6;

static const char FREE_FILLBYTE = 0xea;
static const char ALLOCATED_FILLBYTE = 0xbf;
	
struct MemoryBlock
{
	/// should be FREE_MAGIC or ALLOCATED_MAGIC
	uint32_t magic;
	/// size of the block (including this header structure)
	size_t size;
	/// used as pointer to next free block
	MemoryBlock* next;
	/// used as pointer to previous free block (for allocated blocks
	/// this points to imediate free block before the allocated block if there
	/// is any
	MemoryBlock* prev;
};

extern char* __heap_start_ptr;

static char* heap_start;
static char* heap_end;

#ifdef DEBUG_HEAP
bool heap_initialized = false;
#endif
MemoryBlock* firstFreeBlock = NULL;

void show_heap_structure();

#ifdef TESTHEAP
void sbrk(char* new_heap_end);
#else

static void sbrk(char* new_heap_end)
{
	L4_Word_t addr = reinterpret_cast<L4_Word_t> (new_heap_end);

	CORBA_Environment env (idl4_default_environment);
	IF_MEMORY_brk(L4_Pager(), addr, &env);
	if(env._major != CORBA_NO_EXCEPTION) {
		fprintf(stderr, "No heap left! ABORTING\n");
		abort();
	}
}
#endif

void __init_heap()
{
	L4_Word_t addr = reinterpret_cast<L4_Word_t> (__heap_start_ptr);
	heap_start = reinterpret_cast<char*> (aroundUp(addr, 4096));
#ifdef SHOW_INFO
	printf("ElfEnd %p Heapstart %p\n", __heap_start_ptr, heap_start);
#endif
	
	// Get 4k initial heap
	heap_end = heap_start + 4096;
	sbrk(heap_end);
	
	size_t heap_size = heap_end - heap_start;
#ifdef SHOW_INFO
	printf("Initializing HeapManager, start: %p end: %p (Size %u)\n",
	       heap_start, heap_end, heap_size);	
#endif
	
	// to be extra sure...
	assert(heap_size >= sizeof(MemoryBlock));
	
#ifdef DEBUG_HEAP
	memset(heap_start, FREE_FILLBYTE, heap_size);
#endif
	
	firstFreeBlock = reinterpret_cast<MemoryBlock*> (heap_start);
	firstFreeBlock->magic = FREE_MAGIC;
	firstFreeBlock->size = heap_size;
	firstFreeBlock->next = NULL;
	firstFreeBlock->prev = NULL;

#ifdef DEBUG_HEAP
	heap_initialized = true;
#endif
}

/*
 * Increases heap size so that the last free memory block in the heap has at
 * least size_needed bytes. Returns this last memory block
 */
static MemoryBlock* increase_heap_size(size_t size_needed)
{
	// search free block that is last in heap
	MemoryBlock* last;
	for(last = firstFreeBlock; last != NULL; last = last->next) {
		char* addr = reinterpret_cast<char*> (last);
		if(addr + last->size >= heap_end) {
			assert(addr + last->size == heap_end);
			break;
		}
	}
		
	size_t freeatend = 0;
	if(last != NULL) {
		freeatend = last->size;
	}
		
	size_t additional_heap_needed = size_needed - freeatend;
	L4_Word_t addr = reinterpret_cast<L4_Word_t> (heap_end) + additional_heap_needed;
	char* new_heap_end = reinterpret_cast<char*> (aroundUp(addr, 16384));
	
#ifdef SHOW_INFO
	printf("Increasing heap end from %p to %p\n", heap_end, new_heap_end);
#endif
	sbrk(new_heap_end);
		
	size_t newspace_size = new_heap_end - heap_end;
#ifdef DEBUG_HEAP
	memset(heap_end, FREE_FILLBYTE, newspace_size);
#endif

	MemoryBlock* result;
	if(last != NULL) {
		last->size += newspace_size;
		result = last;
	} else {
		result = reinterpret_cast<MemoryBlock*> (heap_end);
		result->magic = FREE_MAGIC;
		result->size = newspace_size;
		result->next = firstFreeBlock;
		result->prev = NULL;
		firstFreeBlock = result;
	}
	
	heap_end = new_heap_end;
	return result;
}

void* malloc(size_t size)
{    
#ifdef DEBUG_HEAP
	assert(heap_initialized);
#endif

	if(size == 0)
		return NULL;

	// align size to 4 bytes so that we only give out aligned memory blocks
	if(size % 4 != 0)
		size += 4 - (size % 4);		
	
	// split free block as soon as we have more than 8 bytes too much
	const size_t SPLIT_SIZE = sizeof(MemoryBlock) + 8;
	
	// we need size bytes and the space for the header
	size_t size_needed = size + sizeof(MemoryBlock);
	
	// search for a memory block with enough free space
	MemoryBlock* block;
	for(block = firstFreeBlock; block != NULL; block = block->next) {
		assert(block->magic == FREE_MAGIC);
		if(block->size >= size_needed) {
			break;
		}
	}
	if(block == NULL) {
		block = increase_heap_size(size_needed);
	}

	// this is the start of the memory block
	char* memory = reinterpret_cast<char*> (block);

	// should we use the whole free block or split it?
	MemoryBlock* next_free_block;
	MemoryBlock* old_next = block->next;
	MemoryBlock* old_prev = block->prev;
	size_t old_size = block->size;
	if(block->size - size_needed >= SPLIT_SIZE) {
		next_free_block = reinterpret_cast<MemoryBlock*> (memory + size_needed);
		next_free_block->magic = FREE_MAGIC;
		next_free_block->size = old_size - size_needed;
		next_free_block->next = old_next;
		if(old_next != NULL)
			old_next->prev = next_free_block;
#ifdef SHOW_INFO
		printf("Splitting free block at %p (size %u) to new free block %p size %u\n",
			memory, old_size, next_free_block, next_free_block->size);
#endif

	} else {
#ifdef SHOW_INFO
		printf("Reusing memory at %p (size %u) for allocation of size %u\n",
				memory, old_size, size);
#endif
		next_free_block = old_next;
	}
	// modify freelist
	if(old_prev == NULL) {
		firstFreeBlock = next_free_block;
		if(next_free_block != NULL)
			next_free_block->prev = NULL;
	} else {
		old_prev->next = next_free_block;
		if(next_free_block != NULL)
			next_free_block->prev = old_prev;
	}

	// adjust prev pointer in allocated block after us
	char* after = memory + old_size;
	MemoryBlock* after_block = reinterpret_cast<MemoryBlock*> (after);
	if(after < heap_end) {
		assert(after_block->magic == ALLOCATED_MAGIC);
		if(next_free_block == old_next) {
			// we used the whole free block so there is no free block in
			// front of this allocated block anymore
			after_block->prev = NULL;
		} else {
			// there's still a free block (but with a different address) in
			// front of this allocated block
			after_block->prev = next_free_block;
		}
	}
	
#ifdef DEBUG_HEAP
	// make sure noone has written into our free memory
	for(char* p = memory + sizeof(MemoryBlock); p < memory + size_needed; ++p) {
		// if you hit this assert, then you have probably written into
		// unallocated heap memory
		assert(*p == FREE_FILLBYTE);	
	}

	// clear memory (because this helps detecting bugs), should be disabled
	// if this is a performance problem later
	memset(memory, ALLOCATED_FILLBYTE, size_needed);
#endif

	// Construct AllocateMemoryBlock structure
	MemoryBlock* alloc_block = reinterpret_cast<MemoryBlock*> (memory);
	alloc_block->magic = ALLOCATED_MAGIC;
	if(next_free_block == old_next) {
		// reusing a slightly bigger block?
		alloc_block->size = old_size;
	} else {
		// splitted a bigger free block
		alloc_block->size = size_needed;
	}
	// there is no free block in front of the allocated block (yet)
	alloc_block->next = NULL;
	alloc_block->prev = NULL;
	
	// that's it
	char* result = memory + sizeof(MemoryBlock);
	return result;
}

void free(void* freeptr)
{
#ifdef DEBUG_HEAP
	assert(heap_initialized);
#endif

	if(freeptr == NULL)
		return;

	char* memory = static_cast<char*> (freeptr) - sizeof(MemoryBlock);
	
	// the allocation header should be a few bytes in front of our block
	MemoryBlock* alloc_block = reinterpret_cast<MemoryBlock*> (memory);

	// if you hit this assert, then you probably tried to free an already
	// freed memory block or some random invalid pointer
	assert(alloc_block->magic == ALLOCATED_MAGIC);

#ifdef SHOW_INFO
	printf("Freeing memory at %p (blocksize %u)\n", memory, alloc_block->size);
#endif
	
	size_t blocksize = alloc_block->size;
	MemoryBlock* prev_block = alloc_block->prev;
	
	MemoryBlock* free_block = reinterpret_cast<MemoryBlock*> (memory);
	// do we have a free block before us?
	if(prev_block != NULL) {
		// merge with that block before us
		free_block = prev_block;
		free_block->size += blocksize;
	} else {
		// create a new block
		free_block->magic = FREE_MAGIC;
		free_block->size = blocksize;
		free_block->next = firstFreeBlock;
		free_block->prev = NULL;
		
		firstFreeBlock->prev = free_block;
		firstFreeBlock = free_block;
	}

	// look at the block after us
	char* after = memory + blocksize;
	if(after < heap_end) {
		MemoryBlock* after_block = reinterpret_cast<MemoryBlock*> (after);
		if(after_block->magic == FREE_MAGIC) {
			// there's a free memory block behind us, merge with it
			free_block->size += after_block->size;

			// remove that after_block from the list...
			if(after_block->prev != NULL) {
				after_block->prev->next = after_block->next;
				if(after_block->next != NULL)
					after_block->next->prev = after_block->prev;
			} else {
				firstFreeBlock = after_block->next;
				firstFreeBlock->prev = NULL;
			}

			// adjust the allocated block behind that block
			after = reinterpret_cast<char*> (free_block) + free_block->size;
			if(after < heap_end) {
				after_block = reinterpret_cast<MemoryBlock*>(after);
				assert(after_block->magic == ALLOCATED_MAGIC);
				after_block->prev = free_block;
			}
		} else if(after_block->magic == ALLOCATED_MAGIC) {
			assert(after_block->prev == NULL);
			// an allocated block behind us, we become the free block before it
			after_block->prev = free_block;
		} else {
			// corrupted heap... neither free nor allocated block behind us
			assert(false);
		}
	}

	// clear memory (to catch bugs) disable if the performance hit is too
	// big for you
#ifdef DEBUG_HEAP
	memory = reinterpret_cast<char*> (free_block);
	memset(memory + sizeof(MemoryBlock), FREE_FILLBYTE,
	       free_block->size - sizeof(MemoryBlock));
#endif
}

void* realloc(void* ptr, size_t size)
{
	if(ptr == NULL)
		return malloc(size);

	const char* mem = reinterpret_cast<const char*> (ptr);
	const MemoryBlock* block = reinterpret_cast<const MemoryBlock*> (mem - sizeof(MemoryBlock));
	assert(block->magic == ALLOCATED_MAGIC);
	assert(block->size > sizeof(MemoryBlock));

	// poor mans realloc for now... improve later	
	void* newbuffer = malloc(size);
	memcpy(newbuffer, ptr, min(size, block->size - sizeof(MemoryBlock)) );
	free(ptr);
	
	return newbuffer;
}

void* calloc(size_t nmemb, size_t size)
{
	size_t complete_size = nmemb * size;
	void* result = malloc(complete_size);
	if(result == NULL)
		return NULL;
	
	memset(result, 0, complete_size);
	return result;
}

/**
 * debugging function that displays and verifies heap structure
 */
void show_heap_structure()
{
	char* memory = heap_start;
	MemoryBlock* last_free = 0;
	
	while(memory < heap_end) {
		MemoryBlock* block = reinterpret_cast<MemoryBlock*> (memory);
		if(block->magic == ALLOCATED_MAGIC) {
			//printf("%p - %p Allocated Block of size %u, next %p, prev %p\n", memory, memory + block->size, block->size, block->next, block->prev);
			if(block->prev != last_free) {
				printf("Invalid prev link set %p (should be %p)\n",
						block->prev, last_free);
			}
			last_free = NULL;
		} else if(block->magic == FREE_MAGIC) {
			//printf("%p - %p Free Block of size %u, next %p, prev %p\n", memory, memory + block->size, block->size, block->next, block->prev);
			last_free = block;
		} else {
			printf("Corrupted heap (neither allocated nor free block) at address %p\n", memory);
			printf("giving up\n");
			return;
		}
		memory += block->size;
	}

	/*printf("Verifying freelist...");
	fflush(stdout);*/
	MemoryBlock* block = firstFreeBlock;
	MemoryBlock* last = NULL;
	while(block != NULL) {
		if(block->magic != FREE_MAGIC) {
			printf("Bad magic %x for block at %p pointed to by %p\n",
			       block->magic, block, last);
			printf("aborting\n");
			return;
		}
		if(block->prev != last) {
			printf("prev link corrupted at block %p, prev %p should be %p\n",
			       block, block->prev, last);
			return;
		}
		last = block;
		block = block->next;
	}
	//printf("ok\n");
}

// *** Some replacement functions to debug programs

void* malloc_debug(size_t size, const char* function, const char* file, unsigned int line)
{
	printf("malloc(%8u) called by %s (%s:%u)\n", size, function, file, line);
	void *mem = malloc(size);
	printf("malloc(%8u) returns %p\n", size, mem);
	show_heap_structure();
	return mem;
}

void* calloc_debug(size_t nmemb, size_t size, const char* function, const char* file, unsigned int line)
{
	printf("calloc(%8u, %8u) called by %s (%s:%u)\n", nmemb, size, function, file, line);
	void *mem = calloc(nmemb, size);
	printf("calloc(%8u, %8u) returns %p", nmemb, size, mem);
	show_heap_structure();
	return mem;

}

void free_debug(void* ptr, const char* function, const char* file, unsigned int line)
{
	printf("free(%p) called by %s (%s:%u)\n", ptr, function, file, line);
	free(ptr);
	show_heap_structure();
}

void* realloc_debug(void* ptr, size_t size, const char* function, const char* file, unsigned int line)
{
	printf("realloc(%p,%8u) called by %s (%s:%u)\n", ptr, size, function, file, line);
	void *mem = realloc(ptr, size);
	printf("realloc(%p,%8u) returns %p\n", ptr, size, mem);
	show_heap_structure();
	return mem;
}

}
