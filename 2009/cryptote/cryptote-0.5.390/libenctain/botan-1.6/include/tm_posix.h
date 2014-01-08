/*************************************************
* POSIX Timer Header File                        *
* (C) 1999-2007 The Botan Project                *
*************************************************/

#ifndef BOTAN_EXT_TIMER_POSIX_H__
#define BOTAN_EXT_TIMER_POSIX_H__

#include "botan-1.6/include/timers.h"

namespace Enctain {
namespace Botan {

/*************************************************
* POSIX Timer                                    *
*************************************************/
class POSIX_Timer : public Timer
   {
   public:
      u64bit clock() const;
   };

}
}

#endif
