/*
 * $Id: filesystem.h 172 2006-07-26 15:00:01Z sdi2 $
 */
#ifndef __FILESYSTEM_H__
#define __FILESYSTEM_H__

#include <stdint.h>
#include "objectstore.h"

/**
 * Adds file/directory semantics to a minixfs object store. Creates and
 * manages hirarchical directory structures.
 * 
 * @author matze
 */
class FileSystem
{
public:
	FileSystem(Disk* disk);
	~FileSystem();

	// directory/file management
	INodeWrapper* followPath(const INodeWrapper* from, const char* path);
	ErrorCode getDirectoryEntry(const INodeWrapper* dir, INodeWrapper* entry, const char* name);
	ErrorCode linkINode(INodeWrapper* dir, const char* name, INodeWrapper* inode);
	ErrorCode createDirectory(INodeWrapper* parentdir, const char* name);
	void listDirectory(INodeWrapper* inode);

	ObjectStore store;
};

#endif
