// $Id: elfexec.h 199 2006-07-27 08:12:03Z sdi2 $
//
// Implements functions to load an ELF binary image into a new address space
// and start the task.
//
// Author: Timo

#ifndef SDI__EXEC__H
#define SDI__EXEC__H

#include <stdlib.h>

extern "C"
{
	/**
	 * Loads an elf file from a memory block, and setups and starts a new task
	 * for it.
	 * 
	 * @param data      pointer to the datablock
	 * @param datalen   length of the datablock in bytes
	 * @param pagerid   ThreadId of the pager that should be used or nilthread
	 *                  to reuse the current pager
	 * @param threadid  ThreadId for the new task or nilthread to get the next
	 *                  free threadid.
	 */
	extern L4_ThreadId_t sdi_elfexec(const void* data, size_t datalen,
									 L4_ThreadId_t pagerid, L4_ThreadId_t newthreadid);

	extern L4_ThreadId_t sdi_elfexecve(const void* data, size_t datalen,
									   L4_ThreadId_t pager, L4_ThreadId_t newthreadid,
									   const char *const argv[], const char *const envp[]);
	
	extern L4_ThreadId_t sdi_elfexecfv(const char* path,
									   const char *const argv[]);

	extern L4_ThreadId_t sdi_elfexecfve(const char* path,
										const char *const argv[], const char *const envp[]);

	extern L4_ThreadId_t sdi_elfexecf(L4_ThreadId_t pagerid,
									  const char* path,
									  const char *const argv[], const char *const envp[]);

	// *** Functions to wait for a process to terminate.

	/// checks if the process is finished (zombie status) and get it's return
	/// code if done. otherwise the function returns false.
	bool sdi_waitfor(L4_ThreadId_t thread, L4_Word_t flags, L4_Word_t *retcode);

	/// blocks the caller until the task terminates and gets it's return code.
	bool sdi_waitforever(L4_ThreadId_t thread, L4_Word_t *retcode);
}

#ifdef __cplusplus
static inline L4_ThreadId_t sdi_elfexec(const void* data, size_t datalen)
{
	return sdi_elfexec(data, datalen, L4_nilthread, L4_nilthread);
}
	
static inline L4_ThreadId_t sdi_elfexec(const void* data, size_t datalen, L4_ThreadId_t pagerid)
{
	return sdi_elfexec(data, datalen, pagerid, L4_nilthread);
}
#endif

#endif // SDI__EXEC__H
