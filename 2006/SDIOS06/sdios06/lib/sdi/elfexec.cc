// $Id: elfexec.cc 199 2006-07-27 08:12:03Z sdi2 $
//
// Implements functions to load an ELF binary image into a new address space
// and start the task.
//
// Author: Timo

#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>

#include <elf.h>

#include <sdi/log.h>
#include <sdi/panic.h>
#include <sdi/elfexec.h>

#include <idl4glue.h>
#include <if/ifmemory.h>

// #define SHOW_INFO

namespace SDIELFLoader {
	
static inline void dbgout(const char *str)
{
	LogMessage("%s", str);
}

// the main difficulty while reading this function is that there are two
// address spaces involved: the current one and the target one. the stack in
// the current address space in a shared page, in the destination the stack
// begins at 0xC0000000
static L4_Word_t makenewstack(L4_ThreadId_t pagerid, L4_ThreadId_t newthread,
							  const char *const argv[], const char *const envp[])
{
	typedef L4_Word_t addr_t;

	// 16 kb maximum environment and arguments. raise if needed. using this
	// limit saves from doing argument wrappings on fpage boundaries
	static const unsigned int maxenvstacksize = 0x8000;

	static addr_t stackend = 0xC0000000;

	// request the stack fpage from the pager
	CORBA_Environment env (idl4_default_environment);

	if (L4_Pager() != pagerid) {
		// set receive window: this is evil as it has to take some
		// arbitrary position in memory
		idl4_set_rcv_window(&env, L4_Fpage(0x90000000, maxenvstacksize));
	} else {
		// much nicer: let our pager decide where to place the shared page
		idl4_set_rcv_window(&env, L4_CompleteAddressSpace);
	}

	idl4_fpage_t stackpage;
	memset(&stackpage, 0, sizeof(stackpage));

	IF_MEMORY_GetSharedRegion(pagerid,
							  &newthread,
							  stackend - maxenvstacksize,
							  maxenvstacksize,
							  &stackpage,
							  &env);

	if (env._major != CORBA_NO_EXCEPTION)
	{
		LogMessage("elfexec: pager call GetSharedRegion failed for stack, code: %d", CORBA_exception_id(&env));
		return 0x00000000;
	}
	if (L4_IsNilFpage(idl4_fpage_get_page(stackpage)))
	{
		LogMessage("elfexec: pager call GetSharedRegion return nilpage.");
		return 0x00000000;
	}

	L4_Word_t stackpageaddr = 0x90000000;

	if (L4_Pager() == pagerid) {
		stackpageaddr = idl4_fpage_get_base(stackpage);
	}

	// ** construct stack

	if (!argv) {
		static const char* argvnull[] = { NULL };
		argv = argvnull;
	}
	if (!envp) {
		static const char* envpnull[] = { NULL };
		envp = envpnull;
	}

	addr_t top = 0xC0000000;

	// evil: heavily depends on wrapping of unsigned ints
	char *stack = reinterpret_cast<char*>(stackpageaddr) + maxenvstacksize - top;

	// count arguments and environment
	unsigned int argvnum = 0;
	while(argv[argvnum]) argvnum++;

	unsigned int envpnum = 0;
	while(envp[envpnum]) envpnum++;

	// allocate buffers for string pointers on stack
	addr_t argvptr[argvnum+1];
	addr_t envpptr[envpnum+1];

	// put a zero word at the bottom of the stack
	top -= sizeof(uint32_t);
	*(uint32_t*)(stack + top) = 0x00000000;

	// put the environment variables on the stack
	for(unsigned int ei = 0; envp[ei]; ei++)
	{
		unsigned int el = strlen(envp[ei]) + 1; // with null char

		top -= el;
		memcpy(stack + top, envp[ei], el);

		envpptr[ei] = top;
	}

	envpptr[envpnum] = (addr_t)NULL;

	// put command line parameters on the stack
	for(unsigned int ai = 0; argv[ai]; ai++)
	{
		unsigned int al = strlen(argv[ai]) + 1;
		
		top -= al;
		memcpy(stack + top, argv[ai], al);

		argvptr[ai] = top;
	}

	argvptr[argvnum] = (addr_t)NULL;

	// align stack pointer to 4 bytes
	top -= ((unsigned int)top % 4);

	// add pointer arrays for environment and argv
	top -= sizeof(char*) * (envpnum+1);
	memcpy(stack + top, envpptr, sizeof(char*) * (envpnum+1));
	addr_t envpstart = top;

	top -= sizeof(char*) * (argvnum+1);
	memcpy(stack + top, argvptr, sizeof(char*) * (argvnum+1));
	addr_t argvstart = top;
	
	// construct main()'s parameters:

	top -= sizeof(char*);
	*(addr_t*)(stack+top) = envpstart;		// envp

	top -= sizeof(char*);
	*(addr_t*)(stack+top) = argvstart;		// argv

	top -= sizeof(int);
	*(int*)(stack+top) = argvnum;			// argc;

	// return stack fpage

	L4_Fpage_t stackfpage = L4_Fpage(stackpageaddr, maxenvstacksize);
	IF_MEMORY_FreeSharedRegion(pagerid, &newthread, stackend - maxenvstacksize, &stackfpage, &env);
	if (env._major != CORBA_NO_EXCEPTION)
	{
		LogMessage("elfexec: pager call FreeSharedRegion failed, code: %d", CORBA_exception_id(&env));
		return 0x00000000;
	}

	// trying to work-around a bug in L4 which happens after numerous mapping
	// of different size on the same region
	L4_Flush(L4_Fpage(stackpageaddr, maxenvstacksize) + L4_FullyAccessible);

	// return esp of new task
	return top;
}

static bool check_elfheader(const Elf32_Ehdr* ehdr, unsigned int datalen)
{
	// check magic bytes
	if ((ehdr->e_ident[EI_MAG0] !=  ELFMAG0) || 
		(ehdr->e_ident[EI_MAG1] !=  ELFMAG1) || 
		(ehdr->e_ident[EI_MAG2] !=  ELFMAG2) ||
		(ehdr->e_ident[EI_MAG3] !=  ELFMAG3))
	{
		LogMessage("elfexec: invalid elf header: wrong magic bytes.\n");
		return false;
	}
	if (ehdr->e_ident[EI_CLASS] != ELFCLASS32) {
		LogMessage("elfexec: invalid elf header: not 32 bit image.\n");
		return false;
	}
	if (ehdr->e_ident[EI_VERSION] != EV_CURRENT) {
		LogMessage("elfexec: invalid elf header: invalid elf version.\n");
		return false;
	}

	// check file type
	if (ehdr->e_type != ET_EXEC) {
		LogMessage("elfexec: invalid elf header: not an executable elf binary.\n");
		return false;
	}
	if (ehdr->e_machine != EM_386) {
		LogMessage("elfexec: invalid elf header: does not contain Intel 80386 code.\n");
		return false;
	}
	if (ehdr->e_version != EV_CURRENT) {
		LogMessage("elfexec: invalid elf header: invalid elf version.\n");
		return false;
	}
	if (ehdr->e_flags != 0) {
		LogMessage("elfexec: invalid elf header: unknown processor flags.\n");
		return false;
	}

#ifdef SHOW_INFO
	LogMessage("elfexec: Entry point: 0x%x", ehdr->e_entry);
#endif

	if (ehdr->e_phoff + (ehdr->e_phnum * sizeof(Elf32_Phdr)) > datalen) {
		LogMessage("elfexec: invalid elf file: program header table is beyond file end.\n");
		return false;
	}

	if (ehdr->e_phnum == 0) {
		LogMessage("elfexec: invalid elf file: no program header in file.\n");
		return false;
	}

	return true;
}

L4_ThreadId_t elfexec(L4_ThreadId_t pagerid, L4_ThreadId_t wishthreadid, 
					  const void* data, size_t datalen,
					  const char *const argv[], const char *const envp[])
{
	if (datalen < sizeof(Elf32_Ehdr)) {
		LogMessage("elfexec: invalid elf header: too short\n");
		return L4_nilthread;
	}

	const Elf32_Ehdr* ehdr = static_cast<const Elf32_Ehdr*>(data);

	if (!check_elfheader(ehdr, datalen))
		return L4_nilthread;

	const Elf32_Phdr* phdr =
		reinterpret_cast<const Elf32_Phdr*>(reinterpret_cast<const char*>(ehdr) + ehdr->e_phoff);

	// *** create new task

	if(L4_IsNilThread(pagerid)) {
		pagerid = L4_Pager();
	}

	CORBA_Environment env (idl4_default_environment);

	L4_ThreadId_t newthread = wishthreadid;

	IF_MEMORY_CreateTask(pagerid, &newthread, &env);
	if (env._major != CORBA_NO_EXCEPTION)
	{
		LogMessage("elfexec: pager call CreateTask failed, code: %d", CORBA_exception_id(&env));
		return L4_nilthread;
	}

	// load each program header's data into a page

	for (int phi = 0; phi < ehdr->e_phnum; phi++)
	{
		if (phdr[phi].p_type != PT_LOAD)
			continue;

		if(phdr[phi].p_memsz == 0)
			continue;

		// figure out the start of the program image in the "file"
		L4_Word_t fstart = (L4_Word_t)ehdr + phdr[phi].p_offset;

		// the start of the image in virtual memory
		L4_Word_t mstart = phdr[phi].p_vaddr;

		// the size of the fpage we are going to fill, not the size of the
		// program image
		L4_Word_t msize = phdr[phi].p_filesz;
		
		// load the image blockwise and try to align blocks to this modulo
		const L4_Word_t mblock = 0x8000;

		if ((mstart & (mblock - 1)) != 0) {
			LogMessage("elf warning: program image (0x%lx) is not aligned to 0x%lx bytes",
					   mstart, mblock);
			panic("elfexec: program image is not aligned to n bytes");
		}

		for(L4_Word_t moff = 0; moff < msize; moff += mblock)
		{
			// request a shared fpage from the pager

			if (L4_Pager() != pagerid) {
				// set receive window: this is evil as it has to take some
				// arbitrary position in memory
				idl4_set_rcv_window(&env, L4_Fpage(0x90000000, mblock));
			} else {
				// much nicer: let our pager decide where to place the shared page
				idl4_set_rcv_window(&env, L4_CompleteAddressSpace);
			}
		
			idl4_fpage_t mpage;
			memset(&mpage, 0, sizeof(mpage));

			IF_MEMORY_GetSharedRegion(pagerid, &newthread, mstart + moff, mblock, &mpage, &env);

			if (env._major != CORBA_NO_EXCEPTION)
			{
				LogMessage("elfexec: pager call GetSharedRegion failed, code: %d", CORBA_exception_id(&env));
				return L4_nilthread;
			}
			if (L4_IsNilFpage(idl4_fpage_get_page(mpage)))
			{
				LogMessage("elfexec: pager call GetSharedRegion return nilpage.");
				return L4_nilthread;
			}
			
			L4_Word_t mpageaddr = (L4_Pager() == pagerid) ? idl4_fpage_get_base(mpage) : 0x90000000;
			L4_Fpage_t mfpage = L4_Fpage(mpageaddr, mblock);

			L4_Word_t mcopylen = mblock;
			if (moff + mcopylen > msize) mcopylen = msize - moff;

#ifdef SHOW_INFO
			 LogMessage("elfexec: copying: file 0x%lx len 0x%lx -> vmem: 0x%lx at shared 0x%lx\n",
						fstart + moff, mcopylen, mstart + moff, mpageaddr);
#endif

			// copy the image from the file. use idl4's memcpy which has numdwords as 3rd param.
			idl4_memcpy((void*)mpageaddr,
						(void*)(fstart + moff),
						mcopylen / 4);

			IF_MEMORY_FreeSharedRegion(pagerid, &newthread, mstart + moff, &mfpage, &env);
			if (env._major != CORBA_NO_EXCEPTION)
			{
				LogMessage("elfexec: pager call FreeSharedRegion failed, code: %d", CORBA_exception_id(&env));
				return L4_nilthread;
			}

			// trying to work-around a bug in L4 which happens after numerous
			// mapping of different size on the same region
			L4_Flush(L4_Fpage(mpageaddr, mblock) + L4_FullyAccessible);
		}

		// the following .bss section is automaticly zeroed by the pager
	}

	// kick start task

	L4_Word_t esp = makenewstack(pagerid, newthread, argv, envp);
	if (esp == 0x00000000) {
		panic("elfexec: invalid stack frame");
	}

	IF_MEMORY_StartTask(pagerid, &newthread, ehdr->e_entry, esp, &env);
	if (env._major != CORBA_NO_EXCEPTION)
	{
		LogMessage("elfexec: pager call StartTask failed, code: %d", CORBA_exception_id(&env));
		return L4_nilthread;
	}

	return newthread;
}

L4_ThreadId_t elfexecf(L4_ThreadId_t pagerid,
					   const char* path,
					   const char *const argv[], const char *const envp[])
{
	FILE* fimage = fopen(path, "r");
	if (!fimage) {
		LogMessage("elfexecf: invalid path name: file does not exist?\n");
		return L4_nilthread;
	}

	Elf32_Ehdr ehdr;

	if (fread(&ehdr, 1, sizeof(ehdr), fimage) != sizeof(ehdr)) {
		LogMessage("elfexecf: invalid elf header: file too short\n");
		return L4_nilthread;
	}

	fseek(fimage, 0, SEEK_END);
	unsigned int fsize = ftell(fimage);

	if (!check_elfheader(&ehdr, fsize))
		return L4_nilthread;

	// *** create new task

	if(L4_IsNilThread(pagerid)) {
		pagerid = L4_Pager();
	}

	CORBA_Environment env (idl4_default_environment);

	L4_ThreadId_t newthread = L4_nilthread;

	IF_MEMORY_CreateTask(pagerid, &newthread, &env);
	if (env._major != CORBA_NO_EXCEPTION)
	{
		LogMessage("elfexec: pager call CreateTask failed, code: %d", CORBA_exception_id(&env));
		return L4_nilthread;
	}

	// load each program header's data into a page

	for (int phi = 0; phi < ehdr.e_phnum; phi++)
	{
		Elf32_Phdr phdr;

		fseek(fimage, ehdr.e_phoff + phi * sizeof(phdr), SEEK_SET);
		
		if (fread(&phdr, 1, sizeof(phdr), fimage) != sizeof(phdr)) {
			LogMessage("elfexecf: invalid elf header: file too short\n");
		    return L4_nilthread;
		}

		if (phdr.p_type != PT_LOAD) continue;

		if(phdr.p_memsz == 0)
			continue;

		// figure out the start of the program image in the file
		L4_Word_t fstart = phdr.p_offset;

		// the start of the image in virtual memory
		L4_Word_t mstart = phdr.p_vaddr;

		// the size of the fpage we are going to fill, not the size of the
		// program image
		L4_Word_t msize = phdr.p_filesz;
		
		// load the image blockwise and try to align blocks to this modulo
		const L4_Word_t mblock = 0x8000;

		if ((mstart & (mblock - 1)) != 0) {
			LogMessage("elf warning: program image (0x%lx) is not aligned to 0x%lx bytes",
					   mstart, mblock);
			panic("elfexec: program image is not aligned to n bytes");
		}

		for(L4_Word_t moff = 0; moff < msize; moff += mblock)
		{
			// request a shared fpage from the pager

			if (L4_Pager() != pagerid) {
				// set receive window: this is evil as it has to take some
				// arbitrary position in memory
				idl4_set_rcv_window(&env, L4_Fpage(0x90000000, mblock));
			} else {
				// much nicer: let our pager decide where to place the shared page
				idl4_set_rcv_window(&env, L4_CompleteAddressSpace);
			}
		
			idl4_fpage_t mpage;
			memset(&mpage, 0, sizeof(mpage));

			IF_MEMORY_GetSharedRegion(pagerid, &newthread, mstart + moff, mblock, &mpage, &env);

			if (env._major != CORBA_NO_EXCEPTION)
			{
				LogMessage("elfexec: pager call GetSharedRegion failed, code: %d", CORBA_exception_id(&env));
				return L4_nilthread;
			}
			if (L4_IsNilFpage(idl4_fpage_get_page(mpage)))
			{
				LogMessage("elfexec: pager call GetSharedRegion return nilpage.");
				return L4_nilthread;
			}

			L4_Word_t mpageaddr = (L4_Pager() == pagerid) ? idl4_fpage_get_base(mpage) : 0x90000000;
			L4_Fpage_t mfpage = L4_Fpage(mpageaddr, mblock);

			L4_Word_t mcopylen = mblock;
			if (moff + mcopylen > msize) mcopylen = msize - moff;

#ifdef SHOW_INFO
			LogMessage("elfexec: fread file 0x%lx len 0x%lx -> vmem: 0x%lx at shared 0x%lx\n",
					   fstart + moff, mcopylen, mstart + moff, mpageaddr);
#endif

			fseek(fimage, fstart + moff, SEEK_SET);

			if (fread((void*)mpageaddr, 1, mcopylen, fimage) != mcopylen) {
				LogMessage("elfexecf: could not read from file.");
				return L4_nilthread;
			}

			IF_MEMORY_FreeSharedRegion(pagerid, &newthread, mstart + moff, &mfpage, &env);
			if (env._major != CORBA_NO_EXCEPTION)
			{
				LogMessage("elfexec: pager call FreeSharedRegion failed, code: %d", CORBA_exception_id(&env));
				return L4_nilthread;
			}

			// trying to work-around a bug in L4 which happens after numerous
			// mapping of different size on the same region
			L4_Flush(L4_Fpage(mpageaddr, mblock) + L4_FullyAccessible);
		}

		// the following .bss section is automaticly zeroed by the pager
	}

	fclose(fimage);

	// kick start task

	L4_Word_t esp = makenewstack(pagerid, newthread, argv, envp);
	if (esp == 0x00000000) {
		panic("elfexec: invalid stack frame");
	}

	IF_MEMORY_StartTask(pagerid, &newthread, ehdr.e_entry, esp, &env);
	if (env._major != CORBA_NO_EXCEPTION)
	{
		LogMessage("elfexec: pager call StartTask failed, code: %d", CORBA_exception_id(&env));
		return L4_nilthread;
	}

	return newthread;
}

} // namespace SDIELFLoader

extern "C"
{
	L4_ThreadId_t sdi_elfexec(const void* data, size_t datalen,
							  L4_ThreadId_t pager, L4_ThreadId_t newthreadid)
	{
		return SDIELFLoader::elfexec(pager, newthreadid, data, datalen, NULL, NULL);
	}

	L4_ThreadId_t sdi_elfexecve(const void* data, size_t datalen,
								L4_ThreadId_t pager, L4_ThreadId_t newthreadid,
								const char *const argv[], const char *const envp[])
	{
		return SDIELFLoader::elfexec(pager, newthreadid, data, datalen, argv, envp);
	}

	L4_ThreadId_t sdi_elfexecf(L4_ThreadId_t pagerid,
							   const char* path,
							   const char *const argv[], const char *const envp[])
	{
		return SDIELFLoader::elfexecf(pagerid, path, argv, environ);
	}

	L4_ThreadId_t sdi_elfexecfv(const char* path, const char *const argv[])
	{
		return SDIELFLoader::elfexecf(L4_Pager(), path, argv, environ);
	}

	L4_ThreadId_t sdi_elfexecfve(const char* path,
								 const char *const argv[], const char *const envp[])
	{
		return SDIELFLoader::elfexecf(L4_Pager(), path, argv, envp);
	}

	bool sdi_waitfor(L4_ThreadId_t thread, L4_Word_t flags, L4_Word_t *retcode)
	{
		CORBA_Environment env (idl4_default_environment);

		bool r = IF_MEMORY_WaitFor(L4_Pager(), &thread, flags, retcode, &env);

		if (env._major != CORBA_NO_EXCEPTION)
		{
			LogMessage("elfexec: pager call WaitFor failed, code: %d", CORBA_exception_id(&env));
		}

		return r;
	}

	bool sdi_waitforever(L4_ThreadId_t thread, L4_Word_t *retcode)
	{
		return sdi_waitfor(thread, IF_MEMORY_WAIT_FOREVER, retcode);
	}

}
