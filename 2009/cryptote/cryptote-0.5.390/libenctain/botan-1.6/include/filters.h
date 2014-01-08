/*************************************************
* Filters Header File                            *
* (C) 1999-2007 The Botan Project                *
*************************************************/

#ifndef BOTAN_FILTERS_H__
#define BOTAN_FILTERS_H__

#include "botan-1.6/include/pipe.h"
#include "botan-1.6/include/basefilt.h"
#include "botan-1.6/include/data_snk.h"
// #include "botan-1.6/include/pk_filts.h"
#include "botan-1.6/include/base64.h"
#include "botan-1.6/include/hex.h"

namespace Enctain {
namespace Botan {

/*************************************************
* Stream Cipher Filter                           *
*************************************************/
class StreamCipher_Filter : public Keyed_Filter
   {
   public:
      void seek(u32bit position) { cipher->seek(position); }
      bool supports_resync() const { return (cipher->IV_LENGTH != 0); }

      void set_iv(const InitializationVector&);
      void write(const byte[], u32bit);

      StreamCipher_Filter(const std::string&);
      StreamCipher_Filter(const std::string&, const SymmetricKey&);
      ~StreamCipher_Filter() { delete cipher; }
   private:
      SecureVector<byte> buffer;
      StreamCipher* cipher;
   };

/*************************************************
* Hash Filter                                    *
*************************************************/
class Hash_Filter : public Filter
   {
   public:
      void write(const byte input[], u32bit len) { hash->update(input, len); }
      void end_msg();

      Hash_Filter(const std::string&, u32bit = 0);
      ~Hash_Filter() { delete hash; }
   private:
      const u32bit OUTPUT_LENGTH;
      HashFunction* hash;
   };

/*************************************************
* MessageAuthenticationCode Filter               *
*************************************************/
class MAC_Filter : public Keyed_Filter
   {
   public:
      void write(const byte input[], u32bit len) { mac->update(input, len); }
      void end_msg();

      MAC_Filter(const std::string&, u32bit = 0);
      MAC_Filter(const std::string&, const SymmetricKey&, u32bit = 0);
      ~MAC_Filter() { delete mac; }
   private:
      const u32bit OUTPUT_LENGTH;
      MessageAuthenticationCode* mac;
   };

}
}

#endif
