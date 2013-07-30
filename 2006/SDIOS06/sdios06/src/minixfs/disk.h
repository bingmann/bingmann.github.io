/*
 * $Id: disk.h 110 2006-07-20 19:43:31Z sdi2 $
 */
#ifndef DISK_H_
#define DISK_H_

#include <stdlib.h>
#include <idl4glue.h>
#include <if/types.h>

/**
 * Abstraction for a disk
 */
class Disk {
public:
	Disk(const char* device);
	~Disk();

	size_t getBlockSize();
	void readBlock(void* data, size_t num);
	void writeBlock(const void* data, size_t num);
	
private:
	L4_ThreadId_t driver;
	objectid_t devhandle;
	size_t blocksize;
};

#endif
