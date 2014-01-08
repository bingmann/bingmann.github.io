/*************************************************
* Memory Locking Functions Source File           *
* (C) 1999-2007 The Botan Project                *
*************************************************/

#include "botan-1.6/include/util.h"
#include <windows.h>

namespace Enctain {
namespace Botan {

/*************************************************
* Lock an area of memory into RAM                *
*************************************************/
void lock_mem(void* ptr, u32bit bytes)
   {
   VirtualLock(ptr, bytes);
   }

/*************************************************
* Unlock a previously locked region of memory    *
*************************************************/
void unlock_mem(void* ptr, u32bit bytes)
   {
   VirtualUnlock(ptr, bytes);
   }

}
}
