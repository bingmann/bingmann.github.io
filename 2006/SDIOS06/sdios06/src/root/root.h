//
// $Id$
//
// Description: Header for root task
//
#ifndef _ROOT_H
#define _ROOT_H

#include <stddef.h>
#include <l4/types.h>
#include <l4/bootinfo.h>

extern L4_ThreadId_t sigma0id;   // sigma0, just in case
extern L4_ThreadId_t pagerid;    // our internal pager
extern L4_ThreadId_t locatorid;  // locator service
extern L4_ThreadId_t loggerid;   // messaging service
extern L4_ThreadId_t syscallServerId;	// syscall server

extern void locator_server();
extern void logger_server();
extern void ramdisk_server();
extern void pager_loop();
extern void syscall_server();

const L4_BootInfo_t* get_bootinfo();

#endif
