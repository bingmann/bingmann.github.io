// $Id: pager-impl.h 182 2006-07-26 21:23:10Z sdi2 $
//
// File:  src/pager/pager-impl.h
//
// Description: Implementation of the IDL Interface created from
// pager-template.cc

// this file is included from pager.cc and all the pager's data structures are
// available.

#include "pager-server.h"

#include <l4/kdebug.h>

IDL4_INLINE void pager_pagefault_implementation(CORBA_Object _caller, const CORBA_long address, const CORBA_long ip, const CORBA_long privileges, idl4_fpage_t *page, idl4_server_environment *_env)
{
	pdbg1("pager: page fault on 0x%x ip 0x%x by thread 0x%lx\n",
		  address, ip, _caller.raw);
	
	struct TaskEntry *te = tasklist.find(_caller);
	if (!te) {
		panic("pager: page fault on unmanaged task.\n");
	}

	PageMappingEntry *pme = te->mapping.findAddress(address);

	if (!pme)
	{
		// figure out what to do: segfault or map anonymous mem
		int vmac = vmaddressclass.get(address);

		if (vmac == VMA_PROGRAM) // dynamic limit of the heap
		{
			vmac = ((L4_Word_t)address <= te->heaplimit) ? VMA_ANONYMOUS : VMA_SEGFAULT;

			// use brk limit when the malloc supports it.
			// For Matze: comment this out when heap management calls brk().
			// Matze: when I comment this out then I get an instant segfault on
			// task startup...
			vmac = VMA_ANONYMOUS;
		}

		// do what vmac says

		if (vmac == VMA_SEGFAULT)
		{
			pdbg0("pager: segfault of thread 0x%lx at ip 0x%x accessing 0x%x\n",
				  _caller.raw, ip, address);

			panic("pager: segmentation fault");
		}
		else if (vmac == VMA_ANONYMOUS)
		{
			pdbg1("pager: allocating anonymous memory for page fault\n");

			L4_Fpage_t backing = buddysystem.allocate(4096);
			L4_Fpage_t userpage = L4_FpageLog2(address & ~0x0fff, 12);

			zeroFpage(backing);

			pme = te->mapping.insert(userpage, backing);
		}
		else {
			panic("Invalid virtual memory class");
		}
	}

	assert((L4_Word_t)address >= L4_Address(pme->userpage));
	assert((L4_Word_t)address < L4_Address(pme->userpage) + L4_Size(pme->userpage));

	// send backing page into the right place

	pdbg1("pager: sending backing 0x%lx size 0x%lx -> 0x%lx\n", 
		  L4_Address(pme->backing), L4_Size(pme->backing),
		  L4_Address(pme->userpage));

	idl4_fpage_set_mode(page, IDL4_MODE_MAP);
	idl4_fpage_set_page(page, pme->backing);
	idl4_fpage_set_base(page, L4_Address(pme->userpage));
	idl4_fpage_set_permissions(page, IDL4_PERM_FULL);
}

IDL4_PUBLISH_PAGER_PAGEFAULT(pager_pagefault_implementation);

IDL4_INLINE void pager_GetPageGrant_implementation(CORBA_Object _caller, const L4_Word_t size, idl4_fpage_t *page, idl4_server_environment *_env)
{
	// NOT TESTED!

	pdbg2("pager: GetPageGrant(%lu) called by thread 0x%lx\n", size, _caller.raw);

	// fetch a page of the given size from the buddy system

	L4_Fpage_t fp = buddysystem.allocate(size);
	if (L4_IsNilFpage(fp)) {
		pdbg0("pager: out of memory in GetPageGrant\n");
		CORBA_exception_set(_env, ex_IF_MEMORY_out_of_memory, NULL);
	}
	else {
		zeroFpage(fp);
	}

	idl4_fpage_set_page(page, fp);
	idl4_fpage_set_mode(page, IDL4_MODE_GRANT);
}

IDL4_PUBLISH_PAGER_GETPAGEGRANT(pager_GetPageGrant_implementation);

IDL4_INLINE void pager_GetSpecialPhysicalPage_implementation(CORBA_Object _caller, const L4_Fpage_t *phyaddr, idl4_fpage_t *page, idl4_server_environment *_env)
{
	panic("Not implemented.");
}

IDL4_PUBLISH_PAGER_GETSPECIALPHYSICALPAGE(pager_GetSpecialPhysicalPage_implementation);

IDL4_INLINE void pager_CreateTask_implementation(CORBA_Object _caller, L4_ThreadId_t *thread, idl4_server_environment *_env)
{
	pdbg2("pager: CreateTask() called by thread 0x%lx\n",
		  _caller.raw);

	L4_ThreadId_t newthread = *thread;

	if (L4_IsNilThread(newthread))
	{
		// pick new threadid for the task

		static int pick_newthreadid = 100;

		newthread =  L4_GlobalId(pick_newthreadid++, 1);

		if (tasklist.find(newthread) != NULL)
		{
			pdbg0("pager: CreateTask called for already managed task.\n");
			CORBA_exception_set(_env, ex_IF_MEMORY_generic, 0);
			return;
		}
	}
	else
	{
		// try to use wished threadid

		if (tasklist.find(newthread) != NULL)
		{
			pdbg0("pager: CreateTask called for already managed task.\n");
			CORBA_exception_set(_env, ex_IF_MEMORY_generic, 0);
			return;
		}
	}

	// use syscallServer to create address space

	CORBA_Environment env (idl4_default_environment);

	pdbg1("pager: calling Sigma1ThreadControl\n");

	L4_ThreadId_t scheduler = L4_Myself();
	L4_ThreadId_t mynilthread = L4_nilthread;

	IF_SYSCALL_Sigma1ThreadControl(syscallServerId,
								   &newthread,		// destination thread id
								   &newthread,		// address space id (same)
								   &scheduler,		// scheduler
								   &mynilthread,	// no pager -> start inactive
								   -1UL,			// set utcb later
								   &env);

	if (env._major != CORBA_NO_EXCEPTION)
	{
		pdbg0("pager: call to Sigma1ThreadControl failed, code: %d", CORBA_exception_id(&env));
		CORBA_exception_set(_env, ex_IF_MEMORY_generic, 0);
		return;
	}

	assert(L4_UtcbAreaSizeLog2(KIP) <= 14);

	L4_Fpage_t kiparea = L4_FpageLog2(0x02000000, L4_KipAreaSizeLog2(KIP));
	L4_Fpage_t utcbarea = L4_FpageLog2(0x02010000, 14);

	IF_SYSCALL_Sigma1SpaceControl(syscallServerId,
								  &newthread,		// address space to configure
								  0,				// some flag
								  &kiparea,			// KIP Location
								  &utcbarea,		// UTCB Location
								  &mynilthread,		// redirector
								  &env);

	if (env._major != CORBA_NO_EXCEPTION)
	{
		pdbg0("pager: call to Sigma1SpaceControl failed, code: %d", CORBA_exception_id(&env));
		CORBA_exception_set(_env, ex_IF_MEMORY_generic, 0);
		return;
	}

    // allocate new tashentry for the page mapping list

	TaskEntry *te = tasklist.allocate();

	te->thread = newthread;
	te->status = TS_NEWBORN;
	te->heaplimit = 0x90000000;
	te->mapping.setSlabAllocator(pagemappingslaballoc);
	te->waitfor = L4_nilthread;

	tasklist.insert(te);

	// return new address space

	*thread = newthread;
}

IDL4_PUBLISH_PAGER_CREATETASK(pager_CreateTask_implementation);

IDL4_INLINE void pager_GetSharedRegion_implementation(CORBA_Object _caller, const L4_ThreadId_t *destthread, const L4_Word_t destaddress, const L4_Word_t pagesize, idl4_fpage_t *page, idl4_server_environment *_env)
{
	pdbg2("pager: GetSharedRegion(0x%lx, 0x%lx, 0x%lx) called by thread 0x%lx\n",
		  destthread->raw, destaddress, pagesize, _caller.raw);

	struct TaskEntry *te = tasklist.find(*destthread);
	if (!te) {
		pdbg0("pager: GetSharedRegion received for unmanaged thread!\n");
		idl4_fpage_set_page(page, L4_Nilpage);
		return;
	}

	if (te->status != TS_NEWBORN) {
		pdbg0("pager: GetSharedRegion received for running thread!\n");
		idl4_fpage_set_page(page, L4_Nilpage);
		return;
	}

	L4_Fpage_t newfp;
	PageMappingEntry *pme = te->mapping.findAddress(destaddress);

	if (pme)
	{
		// put existing mapping of destination addrspace into caller
		newfp = pme->backing;
		if (L4_Size(newfp) < pagesize) {
			panic("pager: shared fpage is re-requested as larger area.");
		}
	}
	else
	{
		// fetch a new page from the buddy system and put it into the destination
		// address space

		newfp = buddysystem.allocate(pagesize);
		if (L4_IsNilFpage(newfp)) {
			panic("pager: out of memory in GetSharedRegion\n");
		}
	
		zeroFpage(newfp);
		newfp = L4_FpageAddRights(newfp, L4_FullyAccessible);

		te->mapping.insert(L4_Fpage(destaddress, pagesize), newfp);
	}

	pdbg1("pager: sharing 0x%lx - %lu - 0x%lx - raw 0x%lx\n", L4_Address(newfp), L4_Size(newfp), L4_Rights(newfp), newfp.raw);

	struct TaskEntry *callte = tasklist.find(_caller);

	if (callte)
	{
		// pick an address for the new page
		L4_Word_t pick = callte->mapping.findSpace(0xA0000000, pagesize);
		assert(pick >= 0xA0000000 && pick <= 0xB8000000);
		
		// insert into mapping of managed address space
		callte->mapping.insert(L4_Fpage(pick, pagesize), newfp);

		// return fpage via MapItem
		
		idl4_fpage_set_page(page, newfp);
		idl4_fpage_set_base(page, pick);
		idl4_fpage_set_permissions(page, L4_FullyAccessible);
		idl4_fpage_set_mode(page, IDL4_MODE_MAP);
	}
	else
	{
		// return new fpage in mapping

		idl4_fpage_set_page(page, newfp);
		idl4_fpage_set_base(page, 0x0);
		idl4_fpage_set_permissions(page, L4_FullyAccessible);
		idl4_fpage_set_mode(page, IDL4_MODE_MAP);
	}

	pdbg1("pager: sharing done\n");
}

IDL4_PUBLISH_PAGER_GETSHAREDREGION(pager_GetSharedRegion_implementation);

IDL4_INLINE void pager_FreeSharedRegion_implementation(CORBA_Object _caller, const L4_ThreadId_t *destthread, const L4_Word_t destaddress, L4_Fpage_t *page, idl4_server_environment *_env)

{
	pdbg2("pager: FreeSharedRegion(0x%lx, 0x%lx, 0x%lx) called by thread 0x%lx\n",
		  destthread->raw, destaddress, L4_Address(*page), _caller.raw);

	struct TaskEntry *te = tasklist.find(*destthread);
	if (!te) {
		pdbg0("pager: FreeSharedRegion received for unmanaged thread!\n");
		return;
	}

	if (te->status != TS_NEWBORN) {
		pdbg0("pager: FreeSharedRegion received for running thread!\n");
		return;
	}

	PageMappingEntry *pme = te->mapping.findAddress(destaddress);
	if (!pme) {
		pdbg0("pager: FreeSharedRegion received illegal fpage to unmap!\n");
		return;
	}

	// TODO: This is evil: there is no check that the unmapped fpage is a shared one.
	L4_Fpage_t ufp = pme->backing + L4_FullyAccessible;

	pdbg2("pager: unmapping fpage 0x%lx - size %ld - raw 0x%lx\n", L4_Address(ufp), L4_Size(ufp), ufp.raw);

	L4_UnmapFpage(ufp);

	struct TaskEntry *callte = tasklist.find(_caller);

	if (callte)
	{
		pdbg1("pager: removing shared page from managed address space\n");
		PageMappingEntry *callerpme = callte->mapping.findAddress(L4_Address(*page));

		if (!callerpme) {
			pdbg0("pager: FreeSharedRegion tried to unmap different fpage on caller\n");
		}
		else
		{
			callte->mapping.remove(callerpme);
		}
	}
}

IDL4_PUBLISH_PAGER_FREESHAREDREGION(pager_FreeSharedRegion_implementation);

IDL4_INLINE void pager_StartTask_implementation(CORBA_Object _caller, const L4_ThreadId_t *thread, const L4_Word_t ip, const L4_Word_t sp, idl4_server_environment *_env)
{
	pdbg2("pager: StartTask(0x%lx,0x%lx,0x%lx) called by thread 0x%lx\n",
		  thread->raw, ip, sp, _caller.raw);
	
	struct TaskEntry *te = tasklist.find(*thread);
	if (!te || te->status != TS_NEWBORN) {
		pdbg0("pager: StartTask received for unmanaged thread!\n");
		CORBA_exception_set(_env, ex_IF_MEMORY_generic, 0);
		return;
	}

	// unmap all fpages from the elfloading task
	const PageMappingEntry *pme = te->mapping.begin();
	while(1)
	{
		L4_Fpage_t ufp = pme->backing;

		pdbg1("pager: unmapping fpage 0x%lx - size %ld\n", L4_Address(ufp), L4_Size(ufp));

		ufp = L4_FpageAddRights(ufp, L4_FullyAccessible);

		L4_UnmapFpage(ufp);

		if (pme == pme->next) break;
		pme = pme->next;
	}

	// activate thread

	CORBA_Environment env (idl4_default_environment);

	L4_ThreadId_t mynilthread = L4_nilthread;
	L4_ThreadId_t mypagerid = L4_Myself();

	IF_SYSCALL_Sigma1ThreadControl(syscallServerId,
								   thread,			// destination thread id
								   thread,			// address space id (same)
								   &mynilthread,	// scheduler
								   &mypagerid,		// set sigma1 pager -> activates thread
								   0x02010000,		// set utcb location
								   &env);

	if (env._major != CORBA_NO_EXCEPTION)
	{
		pdbg0("pager: call to Sigma1ThreadControl failed, code: %d", CORBA_exception_id(&env));
		CORBA_exception_set(_env, ex_IF_MEMORY_generic, 0);
		return;
	}

	te->status = TS_RUNNING;

	// send kick-start ipc

	L4_Msg_t msg;
	L4_Clear(&msg);
	L4_Append(&msg, ip);
	L4_Append(&msg, sp);
	L4_Load(&msg);
	L4_Send(*thread);
}

IDL4_PUBLISH_PAGER_STARTTASK(pager_StartTask_implementation);

static bool pager_NotifyWaiters(L4_ThreadId_t termtask, L4_Word_t retcode)
{
	unsigned int waiters = 0;
	struct TaskEntry* te = tasklist.begin();
	while(1)
	{
		if (te->waitfor == termtask)
		{
			pdbg0("pager: NotifyWaiters notifying thread 0x%lx that 0x%lx terminated.\n",
				  te->thread.raw, termtask.raw);

			// send asynchronous reply to waiting task
			IF_MEMORY_WaitFor_reply(te->thread, &retcode, true);

			te->waitfor = L4_nilthread;
			waiters++;
		}

		if (te == te->next) break;
		te = te->next;
	}

	return (waiters > 0);
}

IDL4_INLINE void pager_KillTask_implementation(CORBA_Object _caller, const L4_ThreadId_t *thread, idl4_server_environment *_env)
{
	pdbg2("pager: KillTask(0x%lx) called by thread 0x%lx\n",
		  thread->raw, _caller.raw);

	struct TaskEntry *te = tasklist.find(*thread);
	if (!te) {
		pdbg0("pager: KillTask received for unmanaged thread!\n");
		CORBA_exception_set(_env, ex_IF_MEMORY_unknown_task, NULL);
		return;
	}

	// unmap all fpages from the task and put them into the free list
	const PageMappingEntry *pme = te->mapping.begin();
	while(1)
	{
		L4_Fpage_t ufp = pme->backing;

		pdbg1("pager: unmapping fpage 0x%lx - size %ld\n", L4_Address(ufp), L4_Size(ufp));

		ufp = L4_FpageAddRights(ufp, L4_FullyAccessible);
		L4_UnmapFpage(ufp);

		buddysystem.insert(pme->backing);

		if (pme == pme->next) break;
		pme = pme->next;
	}

	te->status = TS_ZOMBIE;
	te->retcode = -1LU;

	if (pager_NotifyWaiters(te->thread, -1LU)) {
		tasklist.remove(te);
	}
}

IDL4_PUBLISH_PAGER_KILLTASK(pager_KillTask_implementation);

IDL4_INLINE void IF_pager_KillMe_implementation(CORBA_Object _caller, const L4_Word_t retcode, idl4_server_environment *_env)
{
	pdbg2("pager: KillMe() called by thread 0x%lx\n",
		  _caller.raw);

	struct TaskEntry *te = tasklist.find(_caller);
	if (!te) {
		pdbg0("pager: KillMe received for unmanaged thread!\n");
		CORBA_exception_set(_env, ex_IF_MEMORY_unknown_task, NULL);
		return;
	}

	// delete thread and address space

	CORBA_Environment env (idl4_default_environment);

	L4_ThreadId_t mynilthread = L4_nilthread;
	L4_ThreadId_t mypagerid = L4_Myself();

	IF_SYSCALL_Sigma1ThreadControl(syscallServerId,
								   &_caller,		// thread id
								   &mynilthread,	// destination: delete thread
								   &mynilthread,	// scheduler
								   &mynilthread,	// set sigma1 pager -> activates thread
								   -1LU,		    // set utcb location
								   &env);

	if (env._major != CORBA_NO_EXCEPTION)
	{
		pdbg0("pager: call to Sigma1ThreadControl failed, code: %d", CORBA_exception_id(&env));
		CORBA_exception_set(_env, ex_IF_MEMORY_generic, 0);
		return;
	}

	// unmap all fpages from the task and put them into the free list
	const PageMappingEntry *pme = te->mapping.begin();
	while(1)
	{
		L4_Fpage_t ufp = pme->backing;

		pdbg1("pager: unmapping fpage 0x%lx - size %ld\n", L4_Address(ufp), L4_Size(ufp));

		L4_UnmapFpage(ufp + L4_FullyAccessible);

		buddysystem.insert(pme->backing);

		if (pme == pme->next) break;
		pme = pme->next;
	}

	te->mapping.clear();

	te->status = TS_ZOMBIE;
	te->retcode = retcode;

	// do something with return code
	pdbg0("pager: Thread 0x%lx killed. return code %ld\n", _caller.raw, retcode);

	if (pager_NotifyWaiters(te->thread, retcode)) {
		tasklist.remove(te);
	}

	idl4_set_no_response(_env);
}

IDL4_PUBLISH_PAGER_KILLME(IF_pager_KillMe_implementation);

IDL4_INLINE CORBA_boolean pager_WaitFor_implementation(CORBA_Object _caller, const L4_ThreadId_t *thread, const L4_Word_t flags, L4_Word_t *retcode, idl4_server_environment *_env)
{
	pdbg2("pager: WaitFor(0x%lx, %lu) called by thread 0x%lx\n",
		  thread->raw, flags, _caller.raw);

	struct TaskEntry *te = tasklist.find(*thread);
	if (!te) {
		pdbg0("pager: WaitFor received for unmanaged thread!\n");
		CORBA_exception_set(_env, ex_IF_MEMORY_unknown_task, NULL);
		return false;
	}

	if (te->status == TS_ZOMBIE)	// task is finished. set return code
	{
		*retcode = te->retcode;

		tasklist.remove(te);		// clean up task entry

		return true;
	}
	
	if (flags & IF_MEMORY_WAIT_FOREVER)
	{
		struct TaskEntry *cte = tasklist.find(_caller);
		if (!cte) {
			pdbg0("pager: WaitFor must be called by a thread managed by the pager!\n");
			CORBA_exception_set(_env, ex_IF_MEMORY_unknown_task, NULL);
			return false;
		}

		assert(L4_IsNilThread(cte->waitfor));
		
		// get notification when the thread terminated
		cte->waitfor = *thread;
		
		// block caller till we reply.
		idl4_set_no_response(_env);
		return false;
	}
	else
	{
		return false;
	}
}

IDL4_PUBLISH_PAGER_WAITFOR(pager_WaitFor_implementation);

IDL4_INLINE void pager_GetPageAny_implementation(CORBA_Object _caller, const L4_Word_t size, L4_Fpage_t *page, idl4_server_environment *_env)

{
	panic("pager: GetPageAny not implemented.");
}

IDL4_PUBLISH_PAGER_GETPAGEANY(pager_GetPageAny_implementation);

IDL4_INLINE void pager_brk_implementation(CORBA_Object _caller, const L4_Word_t heapend, idl4_server_environment *_env)

{
	pdbg1("pager: brk(0x%lx) called by thread 0x%lx\n",
		  heapend, _caller.raw);

	struct TaskEntry *te = tasklist.find(_caller);
	if (!te) {
		pdbg0("pager: brk received for unmanaged thread!\n");
		CORBA_exception_set(_env, ex_IF_MEMORY_unknown_task, NULL);
		return;
	}

	if (te->heaplimit > heapend && te->heaplimit != 0x90000000) {
		pdbg1("pager: brk is reducing the heap size!\n");
	}

	te->heaplimit = heapend;

	if (te->heaplimit >= 0x90000000) {
		pdbg0("pager: brk is trying to set the heap limit beyond the valid heap range.\n");
		te->heaplimit = 0x8fffffff;
	}
	if (te->heaplimit < 0x08408000) {
		pdbg0("pager: brk trying to set the heap limit lower than the .text start address.\n");
		te->heaplimit = 0x08408000;
	}
}

IDL4_PUBLISH_PAGER_BRK(pager_brk_implementation);

// include idl4 functions handling name service and virtual files
#include "pager-files.h"

void *pager_vtable_4[PAGER_DEFAULT_VTABLE_SIZE] = PAGER_DEFAULT_VTABLE_4;
void *pager_vtable_5[PAGER_DEFAULT_VTABLE_SIZE] = PAGER_DEFAULT_VTABLE_5;
void *pager_vtable_6[PAGER_DEFAULT_VTABLE_SIZE] = PAGER_DEFAULT_VTABLE_6;
void *pager_vtable_10[PAGER_DEFAULT_VTABLE_SIZE] = PAGER_DEFAULT_VTABLE_10;
void *pager_vtable_discard[PAGER_DEFAULT_VTABLE_SIZE] = PAGER_DEFAULT_VTABLE_DISCARD;
void **pager_itable[16] = { pager_vtable_discard, pager_vtable_discard, pager_vtable_discard, pager_vtable_discard, pager_vtable_4, pager_vtable_5, pager_vtable_6, pager_vtable_discard, pager_vtable_discard, pager_vtable_discard, pager_vtable_10, pager_vtable_discard, pager_vtable_discard, pager_vtable_discard, pager_vtable_discard, pager_vtable_discard };
void *pager_ktable[PAGER_DEFAULT_KTABLE_SIZE] = PAGER_DEFAULT_KTABLE;

void pager_server()
{
	L4_ThreadId_t partner;
	L4_MsgTag_t msgtag;
	idl4_msgbuf_t msgbuf;
	long cnt;

	idl4_msgbuf_init(&msgbuf);
	
	// for (cnt = 0;cnt < PAGER_STRBUF_SIZE;cnt++)
    //    idl4_msgbuf_add_buffer(&msgbuf, malloc(8000), 8000);

	// add one static string buffer
	static char msgbuf8000[8000];
	idl4_msgbuf_add_buffer(&msgbuf, msgbuf8000, 8000);

	while (1)
    {
		partner = L4_nilthread;
		msgtag.raw = 0;
		cnt = 0;

		while (1)
        {
			idl4_msgbuf_sync(&msgbuf);

			idl4_reply_and_wait(&partner, &msgtag, &msgbuf, &cnt);

			if (idl4_is_error(&msgtag))
				break;

			if (IDL4_EXPECT_FALSE(idl4_is_kernel_message(msgtag)))
				idl4_process_request(&partner, &msgtag, &msgbuf, &cnt, pager_ktable[idl4_get_kernel_message_id(msgtag) & PAGER_KID_MASK]);
            else idl4_process_request(&partner, &msgtag, &msgbuf, &cnt, pager_itable[idl4_get_interface_id(&msgtag) & PAGER_IID_MASK][idl4_get_function_id(&msgtag) & PAGER_FID_MASK]);
        }
    }
}

void pager_discard()
{
	panic("pager: ipc message discarded");
}
