// $Id: test_RangeClassifier.cc 44 2006-07-13 19:58:06Z sdi2 $ von Timo

// Tests the RangeClassifier class using example ranges from a KIP printout

#include <l4/types.h>

#include "RangeClassifier.h"

int main()
{
	RangeClassifier rc;

	rc.test();

	rc.set(0x00000000, 0xffffffff, 2);	//	 shared
	rc.set(0x00000000, 0x0009f7ff, 1);	//	 conventional
	rc.set(0x0009f800, 0x0009ffff, 3);	//	 architecture specific (2)
	rc.set(0x000dc000, 0x000fffff, 3);	//	 architecture specific (2)
	rc.set(0x00100000, 0x01eeffff, 1);	//	 conventional			  
	rc.set(0x01ef0000, 0x01efefff, 3);	//	 architecture specific (3)
	rc.set(0x01eff000, 0x01efffff, 3);	//	 architecture specific (4)
	rc.set(0x01f00000, 0x01ffffff, 1);	//	 conventional			  
	rc.set(0xfec00000, 0xfec0ffff, 3);	//	 architecture specific (2)
	rc.set(0xfee00000, 0xfee00fff, 3);	//	 architecture specific (2)
	rc.set(0xfffe0000, 0xffffffff, 3);	//	 architecture specific (2)
	rc.set(0x00ef0000, 0x01eeffff, 5);	//	 reserved				  
	rc.set(0x000a0000, 0x000bffff, 2);	//	 shared	 
	rc.set(0x000c0000, 0x000effff, 2);	//	 shared
	rc.set(0x00020000, 0x000283ff, 4);	//	 bootloader specific (2)
	rc.set(0x00300000, 0x003323ff, 4);	//	 bootloader specific (2)
	rc.set(0x008b1000, 0x008d83ff, 4);	//	 bootloader specific (3)
	rc.set(0x008d9000, 0x009003ff, 4);	//	 bootloader specific (3)
	rc.set(0x00001000, 0x00001fff, 4);	//	 bootloader specific (1)
	rc.set(0x00002000, 0x00002fff, 4);	//	 bootloader specific (1)
	rc.set(0x00100000, 0x0014dfff, 5);	//	 reserved				

	// rc.set(0x00002000, 0xffff0000, 6);

	//rc.dump();
	rc.test();

	for(unsigned long long i = 0; i <= 0xffffffff; i += 0x100)
	{
		//printf("g: %08x - %d\n", i, rc.get(i));
		assert( rc.get(i) == rc.get2(i) );
	}
}
