// $Id: test_TaskList.cc 60 2006-07-17 09:55:43Z sdi2 $ von Timo

#include <l4/types.h>

#include <stdio.h>

#include "TaskList.h"

int main()
{
	SlabAllocator<TaskEntry>::Page somepage;
	SlabAllocator<TaskEntry>::Pool slab (&somepage);

	TaskList tl (&slab);

	struct TaskEntry *te = tl.allocate();
	te->thread = L4_GlobalId (80, 1);
	tl.insert(te);

	te = tl.allocate();
	te->thread = L4_GlobalId (82, 1);
	tl.insert(te);

	struct TaskEntry *te2 = tl.find(L4_GlobalId (82, 1));

	printf("5: %lx => %lx\n", te2, te2 ? te2->thread.raw : 0);
	tl.dump();
	tl.test();
}
