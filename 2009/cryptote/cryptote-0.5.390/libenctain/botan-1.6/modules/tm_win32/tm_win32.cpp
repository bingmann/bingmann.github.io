/*************************************************
* Win32 Timer Source File                        *
* (C) 1999-2007 The Botan Project                *
*************************************************/

#include "botan-1.6/include/tm_win32.h"
#include <windows.h>

namespace Enctain {
namespace Botan {

/*************************************************
* Get the timestamp                              *
*************************************************/
u64bit Win32_Timer::clock() const
   {
   LARGE_INTEGER tv;
   ::QueryPerformanceCounter(&tv);
   return tv.QuadPart;
   }

}
}
