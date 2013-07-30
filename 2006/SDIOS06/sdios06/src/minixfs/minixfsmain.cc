/*
 * $Id: minixfsmain.cc 172 2006-07-26 15:00:01Z sdi2 $
 */
#include <config.h>

#include <new>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include <sdi/util.h>
#include <sdi/log.h>
#include <sdi/locator.h>
#include <sdi/panic.h>

#include <l4/schedule.h>

#include <idl4glue.h>
#include <if/iflocator.h>
#include <if/ifblockdev.h>

#include "disk.h"
#include "filesystem.h"
#include "minixfs-server.h"

Disk::Disk(const char* device)
{
	while(GetObject(device, IF_BLOCKDEV_ID, &driver, &devhandle) != OK) {
		LogMessage("Waiting for device '%s'", device);
		L4_Yield();
	} 
	
	CORBA_Environment env (idl4_default_environment);
	idlsize_t result = 0;
	IF_BLOCKDEV_GetBlockSize(driver, devhandle, &result, &env);
	if(env._major != CORBA_NO_EXCEPTION) {
		panic("GetBlockSize failed");
	}
	
	blocksize = result;
	LogMessage("DISK '%s' blocksize %u", device, blocksize);
}
		
size_t Disk::getBlockSize() {
	return blocksize;
}

void Disk::readBlock(void* data, size_t num)
{
	CORBA_Environment env (idl4_default_environment);
	
	buffer_t buffer;
	buffer._buffer = reinterpret_cast<CORBA_char*> (data);
	buffer._length = 0;
	buffer._maximum = blocksize;

	IF_BLOCKDEV_ReadBlock(driver, devhandle, num, &buffer, &env);
	if(env._major != CORBA_NO_EXCEPTION) {
		LogMessage("ReadBlock failed");
		memset(data, 0, blocksize);
	}

    // this is magic to get idl4 to accept strings again after requesting a
    // block of data
	idl4_set_counter_minimum(3);
}
	
void Disk::writeBlock(const void* data, size_t num)
{
	CORBA_Environment env (idl4_default_environment);
	
	buffer_t buffer;
	buffer._buffer = reinterpret_cast<CORBA_char*> (const_cast<void*> (data));
	buffer._length = blocksize;
	buffer._maximum = blocksize;
	IF_BLOCKDEV_WriteBlock(driver, devhandle, num, &buffer, &env);
	if(env._major != CORBA_NO_EXCEPTION) {
		LogMessage("WriteBlock failed");
	}
}

FileSystem* filesystems[10];
unsigned int filesystem_count = 0;

int main()
{
	Disk* disk = new Disk("/ramdisk0");
	FileSystem* fs = new FileSystem(disk);
	
	filesystems[0] = fs;
	filesystem_count = 1;
	
	LogMessage("entering minixfs mainloop");
	minixfs_server();
	
	panic("return from serverloop in minixfs");
}
