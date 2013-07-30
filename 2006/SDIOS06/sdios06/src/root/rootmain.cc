// $Id$
//
// File:  src/root/main.cc
//
// Description: a VERY simple root task for sdi
//
#include <config.h>

#include <l4/thread.h>
#include <l4/space.h>
#include <l4/message.h>
#include <l4/ipc.h>
#include <l4/sigma0.h>
#include <l4/bootinfo.h>

#include <new>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sdi/log.h>
#include <sdi/locator.h>
#include <sdi/util.h>
#include <sdi/panic.h>
#include <sdi/elfexec.h>

#include "root.h"

#include <idl4glue.h>
#include <if/iflocator.h>

#include <elf.h>

/* local threadids */
L4_ThreadId_t roottaskid = L4_nilthread;
L4_ThreadId_t sigma0id = L4_nilthread;

L4_ThreadId_t loggerid = L4_nilthread;
L4_ThreadId_t syscallServerId = L4_nilthread;
L4_ThreadId_t ramdiskid = L4_nilthread;

L4_ThreadId_t new_locatorid = L4_nilthread;
L4_ThreadId_t sigma1id = L4_nilthread;

L4_Word_t pagesize = 0;
L4_Word_t utcbsize = 0;

static L4_KernelInterfacePage_t* KIP;

L4_Fpage_t kiparea;
L4_Fpage_t utcbarea;

/* helperstuff */

extern char __elf_start;
extern char __elf_end;

char **environ = NULL; // not defined in our crt0

static const size_t DEFAULT_STACK_SIZE = 4096;

char logger_stack[DEFAULT_STACK_SIZE];
char ramdisk_stack[DEFAULT_STACK_SIZE];
char syscall_stack[DEFAULT_STACK_SIZE];
char elfexec_stack[DEFAULT_STACK_SIZE];
char sigma1_stack[DEFAULT_STACK_SIZE];

extern void pager_main();

static const size_t MAX_RAMDISK_COUNT = 10;

int nextid = 1;

typedef void (*function_ptr) ();

// macro to calculate new utcb addresses which are adjacent to the one of the
// roottask's first thread
#define UTCBaddress(x) ((void*)(((L4_Word_t)L4_MyLocalId().raw + utcbsize * (x)) & ~(utcbsize - 1)))

/// start a new thread within the root task's address space
static L4_ThreadId_t start_thread(function_ptr function, void* stack, L4_ThreadId_t newthreadid = L4_nilthread)
{
	void* utcblocation = UTCBaddress(nextid);
	if(L4_IsNilThread(newthreadid))
		newthreadid = L4_GlobalId ( L4_ThreadNo (L4_Myself ()) + nextid, 1);
	nextid++;

	L4_Word_t ip = reinterpret_cast<L4_Word_t> (function);
	L4_Word_t sp = reinterpret_cast<L4_Word_t> (stack);
	/* do the ThreadControl call */
	if (!L4_ThreadControl(newthreadid,	// new thread id
	                      L4_Myself(),	// address space
	                      L4_Myself(),	// scheduler
	                      sigma0id,	// pager
	                      (void*)utcblocation))
	{
		panic("ThreadControl failed in start_thread");
	}

	/* set thread on our code */
	L4_Start(newthreadid, sp, ip);
	
	return newthreadid;
}

/// create a new address space with the root task as pager and start a thread
static L4_ThreadId_t start_task(function_ptr function, void* stack, L4_ThreadId_t newthreadid = L4_nilthread)
{
	void* utcblocation = UTCBaddress(nextid);
	if(L4_IsNilThread(newthreadid))
		newthreadid = L4_GlobalId ( L4_ThreadNo (L4_Myself ()) + nextid, 1);
	nextid++;

	L4_Word_t ip = reinterpret_cast<L4_Word_t> (function);
	L4_Word_t sp = reinterpret_cast<L4_Word_t> (stack);	
	
	/* first ThreadControl to setup initial thread */
	if (!L4_ThreadControl(newthreadid,		// new thread id
						  newthreadid,		// new address space
						  L4_Myself(),		// scheduler
						  L4_nilthread,		// pager -> inactive thread
						  (void*)-1UL))
	{
		panic("ThreadControl failed in start_task\n");
	}
	
	L4_Word_t dummy;
	
	if (!L4_SpaceControl(newthreadid,		// address space
						 0,					// always 0
						 kiparea,			// same KIP page as the root task
						 L4_FpageLog2((L4_Word_t)utcblocation, 14),	// UTCB fpage
						 L4_anythread,		// redirector
						 &dummy))
	{
		printf("SpaceControl: L4_ErrorCode() = %lu\n", L4_ErrorCode());
		panic("SpaceControl failed in start_task\n");
	}
	
	/* Second ThreadControl, activate thread */
	if (!L4_ThreadControl(newthreadid,			// new thread id
						  newthreadid,			// new address space
						  L4_nilthread,			// scheduler (unchanged)
						  L4_Myself(), 			// pager
						  utcblocation))
	{
		panic("ThreadControl failed\n");
	}
	
	/* send startup IPC */
	L4_Msg_t msg;
	L4_Clear(&msg);
	L4_Append(&msg, ip);
	L4_Append(&msg, sp);
	L4_Load(&msg);
	L4_Send(newthreadid);
	
	return newthreadid;
}

/// request / pin a page from sigma0
static inline L4_Bool_t request_page(L4_Word_t addr)
{
    return !(L4_IsNilFpage( L4_Sigma0_GetPage(sigma0id, L4_Fpage(addr, pagesize)) ) );
}

/// list all boot modules and pin memory
static void pin_module_memory()
{
	const L4_BootInfo_t* bootinfo = get_bootinfo();
	L4_BootRec_t* bootrec = L4_BootInfo_FirstEntry(bootinfo);

	for (unsigned int i=0; i < L4_BootInfo_Entries(bootinfo); i++)
	{
		printf("Module: start %lx size %lx type: %d\n", 
			   L4_Module_Start(bootrec),
			   L4_Module_Size(bootrec),
			   (int)L4_Type(bootrec) );
               
		/* Bring in the memory from sigma0 */
		if(L4_Type(bootrec) == L4_BootInfo_Module)
		{
			for (L4_Word_t addr = L4_Module_Start(bootrec); 
				 addr < L4_Module_Start(bootrec) + L4_Module_Size(bootrec); 
				 addr += pagesize)
			{
				if (!request_page(addr)) {
					panic("root: could not get module pages from sigma0");
				}
			}
		}
               
		bootrec = L4_Next(bootrec);
	}
}

/// load a boot module as an elf image using the elfexec function which creates
/// a virtual address space on sigma1
static void elfexec_module(const L4_BootRec_t* mod, L4_ThreadId_t threadid = L4_nilthread)
{
	char* argv[] = { "testargv", NULL };
	char* envp[] =
		{ "SDI=way-cool",
		  "HOME=/minixfs0",
		  "CWD=/",
		  "USER=sdi",
		  "GROUPS=users",
		  "OSTYPE=SDI/L4",
		  NULL };	

    // thrinc will be deleted when the task server provides unique ids.
	void* start = reinterpret_cast<void*> (L4_Module_Start(mod));
	size_t len = static_cast<size_t> (L4_Module_Size(mod));
	L4_ThreadId_t newthr = sdi_elfexecve(start, len, sigma1id, threadid, argv, envp);

	printf("Module elfexeced as thread %lx\n", newthr.raw);
}

static bool is_elffile(const L4_BootRec_t* bootrec)
{
	/* Check type of module */
	if(L4_Type (bootrec) != L4_BootInfo_Module)
		return false;
		
	if(L4_Module_Size(bootrec) < sizeof(Elf32_Ehdr))
		return false;
		
	L4_Word_t addr = L4_Module_Start(bootrec);
	Elf32_Ehdr* hdr = reinterpret_cast<Elf32_Ehdr*> (addr);
	if ((hdr->e_ident[EI_MAG0] !=  ELFMAG0) || 
		(hdr->e_ident[EI_MAG1] !=  ELFMAG1) || 
		(hdr->e_ident[EI_MAG2] !=  ELFMAG2) ||
		(hdr->e_ident[EI_MAG3] !=  ELFMAG3)) {
		return false;
	}
	
	return true;
}

const L4_BootInfo_t* get_bootinfo()
{
	static const L4_BootInfo_t* bootinfo = NULL;
	
	if(bootinfo != NULL)
		return bootinfo;
		
	bootinfo = reinterpret_cast<const L4_BootInfo_t*> (L4_BootInfo(KIP));
	
	/* We just bring the in the memory of the bootinfo page */
	if (!request_page((L4_Word_t) bootinfo)) {
		// no bootinfo, no chance, no future. Break up
		panic ("Was not able to get bootinfo");
	}
	
	if (!L4_BootInfo_Valid(bootinfo))
		panic("Bootinfo not found");	
	
	return bootinfo;
}

/// entry point of the elfexec thread which loads initial servers from the boot
/// modules into virtual address spaces managed by sigma1.
static void elfexec_thread()
{
	const L4_BootInfo_t* bootinfo = get_bootinfo();
	
	bool islocator = true;
	const L4_BootRec_t* bootrec = L4_BootInfo_FirstEntry(bootinfo);
	for(L4_Word_t i = 0; i < L4_BootInfo_Entries(bootinfo); ++i,
	    bootrec = L4_BootRec_Next(bootrec)) {
	    	
		if(!is_elffile(bootrec))
			continue;
			
		printf("Module %lu is an elffile\n", i);
		// we assume the first loaded module is our locator...
		if(islocator) {
			elfexec_module(bootrec, new_locatorid);
			islocator = false;
		} else {
			elfexec_module(bootrec);
		}

		sleep(1);
	}
	
	printf("elfexec_thread finished loading boot modules\n");

	L4_Stop(L4_Myself());
	panic("This shouldn't happen");
}


void showlogo()
{
#define S(x)	printf("%s", x)
	S("\33[32m      ___           ___                                ___           ___\n");
	S("     /\\  \\         /\\  \\          ___                 /\\  \\         /\\  \\\n");
	S("    /::\\  \\       /::\\  \\        /\\  \\               /::\\  \\       /::\\  \\\n");
	S("   /:/\\ \\  \\     /:/\\:\\  \\       \\:\\  \\             /:/\\:\\  \\     /:/\\ \\  \\\n");
	S("  _\\:\\~\\ \\  \\   /:/  \\:\\__\\      /::\\__\\           /:/  \\:\\  \\   _\\:\\~\\ \\  \\\n");
	S(" /\\ \\:\\ \\ \\__\\ /:/__/ \\:|__|  __/:/\\/__/          /:/__/ \\:\\__\\ /\\ \\:\\ \\ \\__\\\n");
	S(" \\:\\ \\:\\ \\/__/ \\:\\  \\ /:/  / /\\/:/  /             \\:\\  \\ /:/  / \\:\\ \\:\\ \\/__/\n");
	S("  \\:\\ \\:\\__\\    \\:\\  /:/  /  \\::/__/               \\:\\  /:/  /   \\:\\ \\:\\__\\\n");
	S("   \\:\\/:/  /     \\:\\/:/  /    \\:\\__\\                \\:\\/:/  /     \\:\\/:/  /\n");
	S("    \\::/  /       \\::/__/      \\/__/                 \\::/  /       \\::/  /\n");
	S("     \\/__/         ~~                                 \\/__/         \\/__/\n");
	S("\n");
	S("   \33[31m(c) 2006    Timo Bingmann, Matthias Braun, Torsten Geiger, Andreas Maehler\n");
	S("\33[00m\n");
	S("\n");
}
	    
int main()
{
	KIP = (L4_KernelInterfacePage_t*)L4_KernelInterface ();

	roottaskid = L4_Myself();
	sigma0id = L4_Pager();

	showlogo();
	
	printf ("Early system infos:\n");
	printf ("Threads: Myself:0x%lx Sigma0:0x%lx\n", L4_Myself().raw, L4_Pager().raw);
	pagesize = 1 << lsBit(L4_PageSizeMask(KIP));
	printf ("Pagesize: %d\n", (int)pagesize);
	kiparea = L4_FpageLog2 ((L4_Word_t)KIP, L4_KipAreaSizeLog2(KIP));
	printf ("KernelInterfacePage: 0x%lx size: %d\n", L4_Address(kiparea), (int)L4_Size(kiparea));
	printf ("Bootinfo: 0x%lx\n", L4_BootInfo(KIP));
	printf ("ELFimage: from %p to %p\n", &__elf_start, &__elf_end);

	utcbsize = L4_UtcbSize(KIP);
	
	utcbarea = L4_FpageLog2((L4_Word_t)L4_MyLocalId().raw,
							L4_UtcbAreaSizeLog2(KIP) + 1);
	
	// *** pin all code and data of the root task by requesting the pages from sigma0
	for (const char* addr = &__elf_start; addr < &__elf_end; addr += pagesize)
	{
		if (!request_page((L4_Word_t)addr)) {
			panic("root: could not get roottask pages from sigma0.");
		}
	}
	
	// request memory for the grub modules
	pin_module_memory();
	
	// special thread ids
	sigma1id = L4_GlobalId(0x3ffff - 1, 1);
	new_locatorid = L4_GlobalId(0x3ffff - 2, 1);
	loggerid = L4_GlobalId(0x3ffff - 3, 1);
        syscallServerId = L4_GlobalId(0x3ffff - 4, 1);

	// *** fire up modules included in the roottask binary

	/* startup our logger, to be able to put messages on the screen */
	/* Generate some threadid */
	loggerid = start_thread (&logger_server,
	                         &logger_stack[sizeof(logger_stack) - 1],
	                         loggerid);
	printf ("Started logger with id %lx\n", loggerid.raw);
	
	/* start sigma1 pager, to create virtual address spaces */
	sigma1id = start_task(&pager_main, 
	                      &sigma1_stack[sizeof(sigma1_stack) - 1],
	                      sigma1id);
	printf ("Started sigma1 pager with id %lx (Stack %p)\n",
			sigma1id.raw, &sigma1_stack[sizeof(sigma1_stack) - 1]);
	
	/* startup our syscall server */
	syscallServerId = start_thread(&syscall_server,
	                               &syscall_stack[sizeof(syscall_stack)-1],
                                       syscallServerId); 
	printf ("Started syscallServer with id %lx\n", syscallServerId.raw);
	
	// start ramdisk server
	ramdiskid = start_thread(&ramdisk_server,
	                         &ramdisk_stack[sizeof(ramdisk_stack)-1]);
	printf ("Started ramdisk with id %lx\n", ramdiskid.raw);	

	// Note: it is not possible to elfexec before the pager_loop()! So we start
	// an extra thread which will load elf images.
	/* startup (init) elfexec thread */
	L4_ThreadId_t elfexecid = start_thread(&elfexec_thread, 
	                                       &elfexec_stack[sizeof(elfexec_stack) - 1]);
	printf ("Started ElfExecThread with id %lx (Stack %p)\n",
	        elfexecid.raw,
	        &elfexec_stack[sizeof(elfexec_stack) - 1]);

	/* now it is time to become the pager for all those threads we 
	   created recently */
	printf("Going into pager loop\n");
	pager_loop();
	
	panic("Unexpected return from PagerLoop()");
}
