/*
 * $Id: datastructures.h 19 2006-07-09 23:49:21Z sdi2 $
 */
#ifndef __DATASTRUCTURES_H__
#define __DATASTRUCTURES_H__

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

namespace {
	const uint16_t MAGIC = 0x138f;		// minix fs, 30 char names
	const uint16_t MAGIC_V2 = 0x2478;	// minix v2 fs, 30 char names
	
	const size_t MAXNAME = 30;
}

/** SuperBlock structure on disk */
struct SuperBlock
{
	uint16_t iNodeCount;
	uint16_t zoneCount;
	uint16_t iMapBlocks;
	uint16_t zMapBlocks;
	uint16_t firstDataZone;
	uint16_t logZoneSize; /* log2 of blocks per zone */
	uint32_t maxSize;
	uint16_t magic;
	uint16_t state;
	uint32_t zones;
	
    void print() {
    	printf("*** SuperBlock ***\n");
    	printf("INodeCount: %d\n", iNodeCount);
    	printf("ZoneCount: %d\n", zoneCount);
    	printf("IMapBlocks: %d\n", iMapBlocks);
    	printf("ZMapBlocks: %d\n", zMapBlocks);
    	printf("firstDataZone: %d\n", firstDataZone);
    	printf("Log of Blocks/Zone: %d\n", logZoneSize);
    	printf("MaxSize: %u\n", maxSize);
    	printf("Magic: %x\n", magic);
    	printf("State: %d\n", state);
    	printf("Zones: %u\n", zones);
    }
} __attribute__((packed));

/** INode structure on disk */
struct INode
{
	uint16_t mode;
	uint16_t uid;
	uint32_t size;
	uint32_t time;
	uint8_t gid;
	uint8_t nlinks;
	uint16_t zone[9];
	
	void print() {
		printf("*** INode ***\n");
		printf("Mode %d\n", mode);
		printf("UID %d\n", uid);
		printf("Size: %u\n", size);
		printf("Time: %u\n", time);
		printf("GID: %d\n", gid);
		printf("Links: %d\n", nlinks);
		for(int i = 0; i < 9; ++i) {
			printf("Zone%d: %d\n", i, zone[i]);
		}
	}

	enum {
		TYPE_DIR    = 0x04000,
		TYPE_FILE   = 0x08000
	};

	bool isDir() const
	{
		return mode & TYPE_DIR;
	}

	bool isFile() const
	{
		return mode & TYPE_FILE;
	}
} __attribute__((packed));

class INodeWrapper : public INode
{
public:
	uint16_t num;	
};

/** Directory Entry on disk */
struct DirectoryEntry
{
	uint16_t inode;
	char name[MAXNAME];

	void print() {
		printf("Entry: ");

		for(int i = 0; i < 30 && name[i] != 0; ++i) {
			printf("%c", name[i]);
		}
		printf(" (INode %04d)\n", inode);
	}
} __attribute__((packed));

#endif

