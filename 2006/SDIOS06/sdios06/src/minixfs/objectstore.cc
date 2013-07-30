/*
 * $Id: objectstore.cc 194 2006-07-27 00:36:14Z sdi2 $
 */
#include <config.h>

#include "objectstore.h"

#include <new>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include <sdi/log.h>

#include "disk.h"

ObjectStore::ObjectStore(Disk* new_disk)
	: disk(new_disk)
{
	blockSize = disk->getBlockSize();
	
	char block[blockSize];
	disk->readBlock(block, 1);
	assert(blockSize >= sizeof(SuperBlock));
	memcpy(static_cast<SuperBlock*> (&superBlock), block, sizeof(SuperBlock));

	superBlock.print();

	if(superBlock.magic != MAGIC) {
		printf("Bad FS magic %x should be %x\n", superBlock.magic, MAGIC);
		exit(1);
	}
	
	firstIMapBlock = 2;
	firstZMapBlock = firstIMapBlock + superBlock.iMapBlocks;
	firstINodeBlock = firstZMapBlock + superBlock.zMapBlocks;
	
	inodes_per_block = blockSize / sizeof(INode);
}

ObjectStore::~ObjectStore()
{
}

ErrorCode
ObjectStore::getRootINode(INodeWrapper* inode) const
{
	return readINode(inode, 1);
}

size_t
ObjectStore::zone2Block(size_t zone) const
{
	return zone << superBlock.logZoneSize;
}

void
ObjectStore::readZone(void* _data, size_t num) const
{
	char* data = (char*) _data;
	for(int i = 0; i < (1 << superBlock.logZoneSize); ++i) {
		disk->readBlock(data + (blockSize*i), zone2Block(num) + i);
	}
}

void
ObjectStore::writeZone(const void* _data, size_t num)
{
	const char* data = (const char*) _data;
	for(int i = 0; i < (1 << superBlock.logZoneSize); ++i) {
		disk->writeBlock(data + (blockSize*i), zone2Block(num) + i);
	}
}

ErrorCode
ObjectStore::readINode(INodeWrapper* inode, size_t num) const
{
	if(num == 0 || num > superBlock.iNodeCount) {
		return ERROR;
	}
	
	// TODO cache this, otherwise this will be REALLY slow
	size_t blocknum = firstINodeBlock + ((num-1) / inodes_per_block);
	char block[blockSize];
	
	disk->readBlock(block, blocknum);
	
	memcpy(static_cast<INode*> (inode),
		   block + ((num-1) % inodes_per_block) * sizeof(INode), sizeof(INode));
	inode->num = num;
	
	return NO_ERROR;
}

ErrorCode
ObjectStore::writeINode(const INodeWrapper* inode)
{
	if(inode->num == 0 || inode->num > superBlock.iNodeCount)
		return ERROR;

	// TODO cache this, otherwise this will be REALLY slow
	size_t blocknum = firstINodeBlock + ((inode->num-1) / inodes_per_block);
	char block[blockSize];
	
	disk->readBlock(block, blocknum);
	
	memcpy(block + ((inode->num-1) % inodes_per_block) * sizeof(INode),
		   static_cast<const INode*> (inode), sizeof(INode));

	disk->writeBlock(block, blocknum);
	
	return NO_ERROR;
}

size_t
ObjectStore::getZoneSize() const
{
    return blockSize << superBlock.logZoneSize;
}

void
ObjectStore::initDataPointer(DataPointer& pointer, INodeWrapper* inode)
{
    pointer.inode = inode;
    pointer.zone = 0;
    pointer.zonePos = 0;
    pointer.filePos = 0;
    pointer.zoneBuf = 0;
    pointer.zoneDirty = false;
}

void
ObjectStore::nextZone(DataPointer& pointer)
{
    if(pointer.zoneDirty) {
        writeZone(pointer.zoneBuf, getFileZone(pointer.inode, pointer.zone));
        pointer.zoneDirty = false;
    }
    pointer.zone++;
    pointer.zonePos = 0;
    readZone(pointer.zoneBuf, getFileZone(pointer.inode, pointer.zone));
}

size_t
ObjectStore::read(DataPointer& pointer, void* buf, size_t len)
{
    size_t remaining = pointer.inode->size - pointer.filePos;
    if(remaining < len)
        len = remaining;

    if(len == 0)
        return 0;

    if(pointer.zoneBuf == 0) {
        pointer.zoneBuf = new char[getZoneSize()];
        readZone(pointer.zoneBuf, getFileZone(pointer.inode, pointer.zone));
    }
    size_t len1 = getZoneSize() - pointer.zonePos;
    if(len1 >= len) {
        memcpy(buf, pointer.zoneBuf + pointer.zonePos, len);
        pointer.zonePos += len;
        pointer.filePos += len;
        return len;
    }
    memcpy(buf, pointer.zoneBuf + pointer.zonePos, len1);
    pointer.filePos += len1;
    nextZone(pointer);
    
    return len1 + read(pointer, ((char*) buf + len1), len - len1);
}

size_t
ObjectStore::write(DataPointer& pointer, const void* buf, size_t len)
{
    if(len == 0)
        return 0;

	if(pointer.filePos + len >= pointer.inode->size) {
    	ErrorCode res = resize(pointer.inode, pointer.filePos + len);
    	if(res != NO_ERROR) {
    		return 0;
    	}
	}

	if(pointer.zoneBuf == 0) {
		pointer.zoneBuf = new char[getZoneSize()];                              
		readZone(pointer.zoneBuf, getFileZone(pointer.inode, pointer.zone));
	}
	
	size_t len1 = getZoneSize() - pointer.zonePos;
	if(len1 >= len) {
		memcpy(pointer.zoneBuf + pointer.zonePos, buf, len);
		pointer.zonePos += len;
		pointer.filePos += len;
		pointer.zoneDirty = true;
		return len;
	}
	
	memcpy(pointer.zoneBuf + pointer.zonePos, buf, len1);
	pointer.zoneDirty = true;
	pointer.filePos += len1;
	nextZone(pointer);
	
	return len1 + write(pointer, ((const char*) buf + len1), len - len1);
}

ErrorCode
ObjectStore::seek(DataPointer& pointer, size_t pos)
{
    if(pos > pointer.inode->size)
        return ERROR;

    size_t zoneSize = getZoneSize();
    size_t newZone = pos / zoneSize;
    if(newZone != pointer.zone) {
        if(pointer.zoneBuf == 0) {
            pointer.zoneBuf = new char[getZoneSize()];
        }
        readZone(pointer.zoneBuf, getFileZone(pointer.inode, newZone));
        pointer.zone = newZone;
    }
    pointer.zonePos = pos % zoneSize;
    pointer.filePos = pos;

    return NO_ERROR;
}

void
ObjectStore::close(DataPointer& pointer)
{
    if(pointer.zoneDirty) {
        writeZone(pointer.zoneBuf, getFileZone(pointer.inode, pointer.zone));
        pointer.zoneDirty = false;        
    }
    delete[] pointer.zoneBuf;
    pointer.zoneBuf = 0;
}

size_t
ObjectStore::getFileZone(const INodeWrapper* inode, size_t zonenum) const
{
    if(zonenum < 7) {
        return inode->zone[zonenum];
    } else if((zonenum -= 7) < 512) {
        // indirection
        uint16_t zone[getZoneSize() / sizeof(uint16_t)];
        readZone((char*) zone, inode->zone[7]);
        return zone[zonenum];
    } else {
		zonenum -= 512;
        // double indirection
        uint16_t zone[getZoneSize() / sizeof(uint16_t)];
        readZone((char*) zone, inode->zone[8]);
        readZone((char*) zone, zone[zonenum >> 9]);
        return zone[zonenum & 511];
    }
}

ErrorCode
ObjectStore::allocBit(size_t firstblock, size_t maxcount, size_t* result)
{
	size_t block = firstblock;
	size_t block_pos = 0;
	uint8_t block_data[blockSize];
	disk->readBlock(block_data, block);
	size_t i;
	for(i = 0; i < maxcount; ++i) {
	    if(block_data[block_pos] != 0xff)
	        break;
	    
	    block_pos++;
	    if(block_pos >= blockSize) {
	        block++;
	        disk->readBlock(block_data, block);
	        block_pos = 0;
	    }
	}
	if(i >= maxcount)
		return ERROR;
	
	int bit;
	uint8_t c = block_data[block_pos];
	if((c & 0x01) == 0) {
	    bit = 0;
	} else if((c & 0x02) == 0) {
	    bit = 1;
	} else if((c & 0x04) == 0) {
	    bit = 2;
	} else if((c & 0x08) == 0) {
	    bit = 3;
	} else if((c & 0x10) == 0) {
	    bit = 4;
	} else if((c & 0x20) == 0) {
	    bit = 5;
	} else if((c & 0x40) == 0) {
	    bit = 6;
	} else if((c & 0x80) == 0) {
	    bit = 7;
	} else {
	    printf("N: %d.\n", c);
	    return ERROR;
	}
	block_data[block_pos] |= (1 << bit);
	disk->writeBlock(block_data, block);
	
	*result = i*8 + bit;
	return NO_ERROR;
}

void
ObjectStore::freeBit(size_t firstblock, size_t bitnum)
{
	char block_data[blockSize];
	int block = firstblock + (bitnum/8)/blockSize;
	disk->readBlock(block_data, block);
	int block_pos = (bitnum/8) - block*blockSize;
	char c = block_data[block_pos];
	
	assert(c & (1 << bitnum % 8));
	
	c &= ~(1 << bitnum % 8);
	disk->writeBlock(block_data, block);
}

INodeWrapper*
ObjectStore::allocINode(uint16_t mode)
{
	size_t num;
	ErrorCode res = allocBit(firstIMapBlock, superBlock.iNodeCount, &num);
	if(res != NO_ERROR)
		return NULL;
	
	INodeWrapper* inode = new INodeWrapper();
	inode->mode = mode | 0x01ff;
	inode->uid = 0;
	inode->size = 0;
	inode->time = 0;
	inode->gid = 0;
	inode->nlinks = 0;
	
	inode->num = static_cast<uint16_t> (num);
	
	return inode;
}

ErrorCode
ObjectStore::freeINode(const INodeWrapper* inode)
{
	size_t num = inode->num;
	if(num >= superBlock.iNodeCount)
		return ERROR;
	freeBit(firstIMapBlock, num);
	
	return NO_ERROR;
}

ErrorCode
ObjectStore::allocZone(size_t* result)
{
	size_t num;
	if(!allocBit(firstZMapBlock, superBlock.zoneCount, &num))
		return ERROR;
	
	printf("Allocated Zone: %d.\n", num + superBlock.firstDataZone - 1);
	
	*result = num + superBlock.firstDataZone - 1;
	return NO_ERROR;
}

void
ObjectStore::freeZone(size_t num)
{
	freeBit(firstZMapBlock, num - superBlock.firstDataZone + 1);
}

ErrorCode
ObjectStore::addZones(INodeWrapper* inode, size_t newZoneCount)
{
	printf("AddZones: %u.\n", newZoneCount);
	
	size_t newzones[newZoneCount];
	for(size_t i = 0; i < newZoneCount; ++i) {
		ErrorCode res = allocZone(&newzones[i]);
		if(!res) {
			// free already allocated zones
			for(size_t i2 = 0; i2 < i; ++i2)
				freeZone(newzones[i]);
			return res;
		}
	}
	
	size_t zoneSize = getZoneSize();
	size_t zonenum = (inode->size + zoneSize - 1) / zoneSize;
	// place in the indirection block that points to the next indir.
	size_t indirections[2] = { 0, 0xffffffff };
	// cache of the currently selected indirection blocks
	uint16_t* indirectionzones[2] = { 0, 0 };
	for(size_t i = 0; i < newZoneCount; ++i) {
	    size_t zone = zonenum + i;
	    if(zone < 7) {
	        inode->zone[zone] = newzones[i];
	    } else if( (zone -= 7) < (zoneSize / sizeof(uint16_t)) ) {
	        if(indirections[0] != 7) {
	            // allocate space
	            if(indirectionzones[0] == 0) {
	                indirectionzones[0] 
	                    = new uint16_t[zoneSize / sizeof(uint16_t)];
	            }
	            
	            if(inode->zone[7] == 0) {
	                size_t newz;
	                ErrorCode res = allocBit(firstZMapBlock, superBlock.zoneCount, &newz);
	                if(res != NO_ERROR) {
	                	// TODO FIXME free all allocated stuff...
	                	return res;
	                }
	                inode->zone[7] = newz;
	                memset(indirectionzones[0], 0, zoneSize);
	            } else {
	                readZone(indirectionzones[0], inode->zone[7]);
	            }
	            indirections[0] = 7;
	        }
	        indirectionzones[0][zone] = newzones[i];
	    } else {
		    // make sure first indirection block is loaded
		    if(indirections[0] != 8) {
		        // allocate space
		        if(indirectionzones[0] == 0) {
		            indirectionzones[0] 
		                = new uint16_t[zoneSize / sizeof(uint16_t)];
		        }
		        
		        // write back single indirection block
		        if(indirections[0] == 7) {
		            disk->writeBlock(indirectionzones[0], inode->zone[7]);
		        }
		        // write back double indirection block
		        if(indirections[1] != 0xffffffff) {
		            disk->writeBlock(indirectionzones[1],
		                    indirectionzones[0][indirections[1]]);
		        }
		
		        if(inode->zone[8] == 0) {
		            size_t newz;
		            ErrorCode res = allocBit(firstZMapBlock, superBlock.zoneCount, &newz);
		            if(res != NO_ERROR) {
		            	// TODO FIXME free allocated stuff here
		            	return res;
		            }
		            inode->zone[8] = newz;
		            memset(indirectionzones[0], 0, zoneSize);
		        } else {
		            readZone(indirectionzones[0], inode->zone[8]);
		        }
		        indirections[0] = 8;
		        indirections[1] = 0xffffffff;
		    }
		    size_t indir1 = zone >> 9;
		    indirectionzones[0][indir1] = indir1;
		    // make sure 2nd indirection block is loaded
		    if(indirections[1] != indir1) {
		        // allocate space
		        if(indirectionzones[1] == 0) {
		            indirectionzones[1]
		                = new uint16_t[zoneSize / sizeof(uint16_t)];
		            memset(indirectionzones[1], 0, zoneSize);
		        }
		
		        // write back last double indirection block
		        if(indirections[1] != 0xffffffff) {
		            disk->writeBlock(indirectionzones[1],
		                    indirectionzones[0][indirections[1]]);
		        }
		
		        if(indirectionzones[indir1] == 0) {
		            size_t newz;
		            ErrorCode res = allocBit(firstZMapBlock, superBlock.zoneCount, &newz);
		            if(res != NO_ERROR) {
		            	// TODO FIXME free allocated stuff
		            	return res;
		            }
		            indirectionzones[0][indir1] = newz;
		        } else {
		            readZone(indirectionzones[1], indirectionzones[0][indir1]);
		        }
		        indirections[1] = indir1;
		    }
		    size_t indir2 = zone & 511;
		    indirectionzones[1][indir2] = newzones[i];
		}
	}
	
	// write back single indirection block
	if(indirections[0] >= 7) {
	    disk->writeBlock(indirectionzones[0], inode->zone[indirections[0]]);
	}
	// write back double indirection block
	if(indirections[1] != 0xffffffff) {
		disk->writeBlock(indirectionzones[1], indirectionzones[0][indirections[1]]);
	}
	
	return NO_ERROR;
}

ErrorCode
ObjectStore::resize(INodeWrapper* inode, size_t newSize)
{
    size_t zoneSize = getZoneSize();
    int diff = newSize - inode->size;
    if(diff > 0) {
        size_t remaining;
        if(inode->size == 0)
            remaining = 0;
        else
            remaining = zoneSize - (inode->size % zoneSize);

        if((size_t) diff > remaining) {
            size_t newZoneCount = (diff - remaining) / zoneSize + 1;

           	// TODO handle errors
            addZones(inode, newZoneCount);
        }
    } else if(diff < 0) {
        // TODO implement shrinking of files...
    } else {
        return NO_ERROR;
    }

    // adjust inode
    inode->size = newSize;
    writeINode(inode);
    return NO_ERROR;
}

ObjectStore::DataPointer::~DataPointer()
{
	assert(zoneBuf == 0);
	assert(zoneDirty == false);
	delete[] zoneBuf;
}
