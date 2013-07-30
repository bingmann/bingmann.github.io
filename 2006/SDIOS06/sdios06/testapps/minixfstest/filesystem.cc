/*
 * $Id: filesystem.cc 172 2006-07-26 15:00:01Z sdi2 $
 */
#include <config.h>

#include "filesystem.h"

#include <new>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include "datastructures.h"

FileSystem::FileSystem(Disk* disk)
	: store(disk)
{
}

FileSystem::~FileSystem()
{
}

ErrorCode
FileSystem::getDirectoryEntry(const INodeWrapper* dir, INodeWrapper* entry, const char* name)
{
	ObjectStore::DataPointer pointer;
	INodeWrapper dir_inode (*dir);
	store.initDataPointer(pointer, &dir_inode);
	DirectoryEntry dir_entry;
	while(store.read(pointer, &dir_entry, sizeof(dir_entry)) == sizeof(dir_entry)) {
		if(dir_entry.inode == 0)
			continue;
		
		if(strncmp(dir_entry.name, name, MAXNAME) == 0) {
			ErrorCode res = store.readINode(entry, dir_entry.inode);
			store.close(pointer);
			return res;
		}
	}
	store.close(pointer);
	
	return ERROR;		
}

void
FileSystem::listDirectory(INodeWrapper* inode)
{
	ObjectStore::DataPointer pointer;
	store.initDataPointer(pointer, inode);
	DirectoryEntry entry;
	while(store.read(pointer, static_cast<DirectoryEntry*> (&entry),
	      sizeof(DirectoryEntry)) == sizeof(DirectoryEntry)) {
		if(entry.inode != 0) {
			entry.print();
			INodeWrapper entrynode;
			store.readINode(&entrynode, entry.inode);
			entrynode.print();
		}
	}
	store.close(pointer);
}

ErrorCode
FileSystem::linkINode(INodeWrapper* dir, const char* name, INodeWrapper* inode)
{
	ObjectStore::DataPointer pointer;
	store.initDataPointer(pointer, dir);
	
	size_t newentrypos = 0;
	DirectoryEntry entry;
	while(store.read(pointer, static_cast<DirectoryEntry*> (&entry),
	            sizeof(DirectoryEntry)) == sizeof(DirectoryEntry)) {
		if(entry.inode == 0) {
			if(newentrypos == 0)
				newentrypos = pointer.filePos - sizeof(DirectoryEntry);
		} else if(strncmp(entry.name, name, 30) == 0) {
			printf("A file with this name already exists.\n");
			store.close(pointer);
			return ERROR;
		}
	}
	if(newentrypos != 0) {
		store.seek(pointer, newentrypos);
	}
	
	inode->nlinks++;
	store.writeINode(inode);        
	
	entry.inode = inode->num;
	strncpy(entry.name, name, 30);
	if(store.write(pointer, static_cast<DirectoryEntry*> (&entry),
	               sizeof(DirectoryEntry)) != sizeof(DirectoryEntry)) {
		printf("NO space left\n");
		store.close(pointer);
		return ERROR;
	}
	store.close(pointer);
	
	return NO_ERROR;
}

ErrorCode
FileSystem::createDirectory(INodeWrapper* parentdir, const char* name)
{
	INodeWrapper* node = store.allocINode(INode::TYPE_DIR);
	if(node == NULL)
		return ERROR;

	// TODO correctly rollback in case of error...
	ErrorCode res;
	res = linkINode(parentdir, name, node);
	if(res != NO_ERROR)
		return res;
	res = linkINode(node, ".", node);
	if(res != NO_ERROR)
		return res;
	res = linkINode(node, "..", parentdir);
	if(res != NO_ERROR)
		return res;

	return NO_ERROR;
}
