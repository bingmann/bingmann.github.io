// $Id: test_SlabAllocator.cc 44 2006-07-13 19:58:06Z sdi2 $ von Timo

// Tests the SlabAllocator class using some example struct

#include <l4/types.h>

#include <stdio.h>

#include "SlabAllocator.h"

struct SlabObj
{
	unsigned int firstaddr, lastaddr;
};

int main()
{
	typedef SlabAllocator<SlabObj> MySlab;
	MySlab::Pool pool;

	MySlab::Page sp;

	pool.test();
	pool.addPage(&sp);

	pool.test();

	assert(sizeof(sp) == 4096);

	struct SlabObj *ob = pool.allocate();

	pool.test();

	pool.free(ob);

	pool.test();
}
