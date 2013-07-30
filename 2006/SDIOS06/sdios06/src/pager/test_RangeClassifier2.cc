// $Id: test_RangeClassifier2.cc 52 2006-07-16 15:16:11Z sdi2 $ von Timo

// Tests the RangeClassifier class using example ranges from a KIP printout

#include <l4/types.h>

#include "RangeClassifier.h"

int main()
{
	RangeClassifier rc;

	rc.test();

	rc.set(0x00000000, 0xffffffff, 2);
	rc.set(0x00000000, 0x0009f7ff, 1);
	rc.set(0x0009f800, 0x0009ffff, 2);

	rc.dump();
	rc.test();

	for(unsigned long long i = 0; i <= 0xffffffff; i += 0x100)
	{
		//printf("g: %08x - %d\n", i, rc.get(i));
		assert( rc.get(i) == rc.get2(i) );
	}
}
