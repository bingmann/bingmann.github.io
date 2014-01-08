/*************************************************
* Pthread Mutex Header File                      *
* (C) 1999-2007 The Botan Project                *
*************************************************/

#ifndef BOTAN_EXT_MUTEX_PTHREAD_H__
#define BOTAN_EXT_MUTEX_PTHREAD_H__

#include "botan-1.6/include/mutex.h"

namespace Enctain {
namespace Botan {

/*************************************************
* Pthread Mutex Factory                          *
*************************************************/
class Pthread_Mutex_Factory : public Mutex_Factory
   {
   public:
      Mutex* make();
   };

}
}

#endif
