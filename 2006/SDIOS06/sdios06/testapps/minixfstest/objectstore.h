/*
 * $Id: objectstore.h 172 2006-07-26 15:00:01Z sdi2 $
 */
#ifndef OBJECTSTORE_H_
#define OBJECTSTORE_H_

#include <stdint.h>
#include "datastructures.h"

class Disk;

enum ErrorCode {
	NO_ERROR = 0,
	ERROR = 1
	// TODO make this more fine grained
};

/**
 * The object store handles storage of objects on disk. An object is a
 * datablock on disk that is representeted with an inode.
 * The inode also contains some metainformation about the datablock.
 * You can allocate,free and read/write to blocks. Files and directories
 * are not handled here, but in a higher layer.
 * 
 * @author matze
 */
class ObjectStore
{
public:
	ObjectStore(Disk* disk);
	~ObjectStore();
	
	class DataPointer
	{
	public:
		~DataPointer();
		
		INodeWrapper* inode;
		size_t zone;
		size_t zonePos;
		size_t filePos;
		char* zoneBuf;
		bool zoneDirty;
	};
	
	ErrorCode getRootINode(INodeWrapper* inode) const;	
	void initDataPointer(DataPointer& pointer, INodeWrapper* inode);
	size_t read(DataPointer& pointer, void* buf, size_t len);
	size_t write(DataPointer& pointer, const void* buf, size_t len);
	ErrorCode seek(DataPointer& pointer, size_t pos);
	void close(DataPointer& pointer);
	ErrorCode resize(INodeWrapper* inode, size_t newsize);

	/* unfortunately metadata and object space/allocation is mixed in the
	 * INode structure. So we have to make the following 3 functions public
	 */
	INodeWrapper* allocINode(uint16_t mode);
	ErrorCode freeINode(const INodeWrapper* inode);
	ErrorCode readINode(INodeWrapper* inode, size_t num) const;
	ErrorCode writeINode(const INodeWrapper* inode);	
	    
private:
	void nextZone(DataPointer& pointer);
	
	size_t zone2Block(size_t zone) const;
	size_t getZoneSize() const;
	size_t getFileZone(const INodeWrapper* inode, size_t zonenum) const;
	
	ErrorCode allocBit(size_t firstblock, size_t maxcount, size_t* result);
	void freeBit(size_t firstblock, size_t bitnum);
	
	ErrorCode allocZone(size_t* zone);
	void freeZone(size_t zone);
	
	void readZone(void* data, size_t num) const;
	void writeZone(const void* data, size_t num);
	
	ErrorCode addZones(INodeWrapper* inode, size_t newZoneCount);
	
	SuperBlock superBlock;
	
	int firstIMapBlock, firstZMapBlock, firstINodeBlock;
	
	Disk* disk;
	size_t blockSize;
	int inodes_per_block;
};

#endif
