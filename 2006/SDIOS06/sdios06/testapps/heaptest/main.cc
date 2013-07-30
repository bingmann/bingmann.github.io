#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

extern "C" {

void init_heap();
void* sdi_malloc(size_t size);
void sdi_free(void* ptr);
void* sdi_realloc(void* ptr, size_t size);
void show_heap_structure();

char* __heap_start_ptr = NULL;

}

const size_t HEAPSIZE = 10200000;
char* end = NULL;

extern "C" 
{

void sdi_sbrk(char* new_end)
{
	printf("NewEnd: %p ElfEnd: %p\n", new_end, __heap_start_ptr);
	if(new_end - __heap_start_ptr > (int) HEAPSIZE) {
		fprintf(stderr, "Out of heap\n");
		abort();
	}
	
	if(new_end > end) {
		for(char* p = end; p < __heap_start_ptr + HEAPSIZE; ++p) {
			if(*p != 42) {
				printf("Memory outside heap was touched!\n");
				abort();
			}
		}
	} else {
		memset(new_end, 42, new_end - end);
	}
	printf("New Heap End: %p\n", new_end);
	end = new_end;
}

}

int main()
{
	const int ALLOCATION_COUNT = 1000;
	const int MAX_ALLOCATION_SIZE = 200;
	srand(243);
	
	__heap_start_ptr = (char*) malloc(HEAPSIZE);
	end = __heap_start_ptr;
	memset(__heap_start_ptr, 42, HEAPSIZE);
	init_heap();

	printf("Initial heap layout:\n");
	show_heap_structure();

	printf("Doing %d allocations...\n", ALLOCATION_COUNT);
	void* allocations[ALLOCATION_COUNT];
	// do 100 random allocations
	for(int i = 0; i < ALLOCATION_COUNT; ++i) {
		size_t size = rand() % MAX_ALLOCATION_SIZE;
		allocations[i] = sdi_malloc(size);
		//show_heap_structure();
	}

	printf("Heap layout:\n");
	show_heap_structure();

	const int randomops = 2000000;
	printf("Doing %d random ops...\n", randomops);
	for(int i = 0; i < randomops; ++i) {
		//show_heap_structure();
		int f = rand() % ALLOCATION_COUNT;
		if(allocations[f] != NULL) {
			sdi_free(allocations[f]);
			allocations[f] = NULL;
		} else {
			allocations[f] = sdi_malloc(rand() % MAX_ALLOCATION_SIZE);
		}
	}

	printf("Heap layout:\n");
	show_heap_structure();

	printf("Freeing all...\n");
	for(int i = 0; i < ALLOCATION_COUNT; ++i) {
		if(allocations[i] != NULL) {
			sdi_free(allocations[i]);
			allocations[i] = NULL;
		}
	}
	printf("Heap Layout:\n");
	show_heap_structure();
	printf("Heap allocated: from %p to %p -> %d bytes\n", __heap_start_ptr, end, end - __heap_start_ptr);
    
    return 0;
}
