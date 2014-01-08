/*************************************************
* Win32 Mutex Source File                        *
* (C) 1999-2007 The Botan Project                *
*************************************************/

#include "botan-1.6/include/mux_win32.h"
#include "botan-1.6/include/exceptn.h"
#include <windows.h>

namespace Enctain {
namespace Botan {

/*************************************************
* Win32 Mutex Factory                            *
*************************************************/
Mutex* Win32_Mutex_Factory::make()
   {
   class Win32_Mutex : public Mutex
      {
      public:
         void lock() { EnterCriticalSection(&mutex); }
         void unlock() { LeaveCriticalSection(&mutex); }

         Win32_Mutex() { InitializeCriticalSection(&mutex); }
         ~Win32_Mutex() { DeleteCriticalSection(&mutex); }
      private:
         CRITICAL_SECTION mutex;
      };

   return new Win32_Mutex();
   }

}
}
