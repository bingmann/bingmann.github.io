/*************************************************
* S2K Source File                                *
* (C) 1999-2007 The Botan Project                *
*************************************************/

#include "botan-1.6/include/s2k.h"
#include "botan-1.6/include/rng.h"

namespace Enctain {
namespace Botan {

/*************************************************
* Derive a key from a passphrase                 *
*************************************************/
OctetString S2K::derive_key(u32bit key_len,
                            const MemoryRegion<byte>& passphrase) const
   {
   return derive(key_len, passphrase, salt, salt.size(), iterations());
   }

/*************************************************
* Set the number of iterations                   *
*************************************************/
void S2K::set_iterations(u32bit i)
   {
   iter = i;
   }

/*************************************************
* Change the salt                                *
*************************************************/
void S2K::change_salt(const byte new_salt[], u32bit length)
   {
   salt.set(new_salt, length);
   }

/*************************************************
* Change the salt                                *
*************************************************/
void S2K::change_salt(const MemoryRegion<byte>& new_salt)
   {
   change_salt(new_salt.begin(), new_salt.size());
   }

/*************************************************
* Create a new random salt                       *
*************************************************/
void S2K::new_random_salt(u32bit length)
   {
   salt.create(length);
   Global_RNG::randomize(salt, length);
   }

}
}
