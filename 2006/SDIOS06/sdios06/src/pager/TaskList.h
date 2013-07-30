// $Id: TaskList.h 146 2006-07-24 16:53:09Z sdi2 $

#ifndef SDI2_PAGER_TASKLIST_H
#define SDI2_PAGER_TASKLIST_H

#include <assert.h>
#include "SlabAllocator.h"
#include "PageMappingList.h"

// task status enumeration
enum taskstatus_t { TS_NEWBORN = 0, TS_RUNNING, TS_ZOMBIE };

/** the node struct of the doubly-linked list of task structures */
struct TaskEntry
{
	struct TaskEntry *prev, *next;
	L4_ThreadId_t	thread;

	// status: newborn, running, zombie
	taskstatus_t	status;

	// heap's upper limit
	L4_Word_t		heaplimit;

	// the task's mapping
	PageMappingList	mapping;

	// the tasks return code when it is a zombie
	L4_Word_t		retcode;

	// if the task is blocked in WaitFor then this is the thread it's waiting
	// for
	L4_ThreadId_t	waitfor;
};

/** TaskList holds a doubly-linked list of task management structgures in a
 * slab allocator. */

class TaskList
{
private:

	/// slab allocator type used by the mapping list
	typedef SlabAllocator<TaskEntry> 	slaballoc_t;

	/// reference to the used slab allocator
	slaballoc_t::Pool*		slaballoc;

	/// the head of the doubly-linked list
	struct TaskEntry	*head;

public:
	/// create a task list backed by the given allocator
	explicit inline TaskList(slaballoc_t::Pool *usedallocator = NULL)
		: slaballoc(usedallocator), head(NULL)
	{
	}

	/// test invariants of the list
	inline void test() const
	{
		if (!head) return;

		class TaskEntry* iter = head;
		assert(iter->prev == iter);

		while(iter->next != iter)
		{
			assert(iter->next->prev == iter);

			assert(iter->thread.raw <= iter->next->thread.raw);

			iter = iter->next;
		}
	}

	/// returns the head of the doubly-linked list
	inline const struct TaskEntry* begin() const
	{ return head; }

	/// returns the head of the doubly-linked list
	inline struct TaskEntry* begin()
	{ return head; }

	/// allocate an empty structure from the backing slab allocator
	inline struct TaskEntry *allocate()
	{
		TaskEntry *newnode = slaballoc->allocate();
		if (!newnode) {
			assert(0);
			return NULL;
		}

		return newnode;
	}

	/// insert a task into the used list
	inline bool insert(struct TaskEntry *te)
	{
		test();

		if (!head)
		{
			// first node
			te->next = te;
			te->prev = te;
			head = te;
		}
		else
		{
			TaskEntry *iter = head;

			while(te->thread.raw >= iter->thread.raw)
			{
				if (iter->next == iter)
				{
					// insert the node at the end. as it has highest threadid
					iter->next = te;
					te->next = te;
					te->prev = iter;

					test();
					return true;
				}

				iter = iter->next;
			}

			// insert in front of the iter node.
			if (iter->prev == iter)
			{
				// insert as first node

				assert(head == iter);

				te->prev = te;
				te->next = iter;

				iter->prev = te;
				head = te;
			}
			else
			{
				// somewhere in the middle
				iter->prev->next = te;
				te->next = iter;

				te->prev = iter->prev;
				iter->prev = te;
			}
		}

		test();

		return true;
	}

	/// remove a task from the list given by the entry ptr
	inline void remove(struct TaskEntry *e)
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

	/// returns the structure for thread ti or NULL if it doesnt exist.
	inline struct TaskEntry *find(L4_ThreadId_t ti)
	{
		if (!head) return NULL;

		TaskEntry *iter = head;

		while(ti.raw > iter->thread.raw)
		{
			if (iter->next == iter)
				break;

			iter = iter->next;
		}

		if (ti.raw != iter->thread.raw) return NULL;

		return iter;
	}

	/// print out the list
	inline void dump() const
	{
		if (!head) {
			printf("TaskList is empty\n");
			return;
		}

		TaskEntry *iter = head;

		printf("TaskList dump:\n");
		while(1)
		{
			printTaskEntry(iter);

			if (iter->next == iter) break;
			iter = iter->next;
		}
	}

	/// print out a page mapping entry
	static inline void printTaskEntry(TaskEntry *pme)
	{
		if (!pme) {
			printf("te: (null)\n");
		}
		else {
			printf("te: thrid 0x%x\n",
				   static_cast<unsigned int>(pme->thread.raw));

		}
	}
};

#endif // SDI2_PAGER_TASKLIST_H
