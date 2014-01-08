/*************************************************
* OctetString Source File                        *
* (C) 1999-2007 The Botan Project                *
*************************************************/

#include "botan-1.6/include/symkey.h"
#include "botan-1.6/include/bit_ops.h"
#include "botan-1.6/include/pipe.h"
#include "botan-1.6/include/hex.h"
#include "botan-1.6/include/rng.h"
#include <algorithm>

namespace Enctain {
namespace Botan {

/*************************************************
* Create an OctetString from RNG output          *
*************************************************/
void OctetString::change(u32bit length)
   {
   bits.create(length);
   Global_RNG::randomize(bits, length);
   }

/*************************************************
* Create an OctetString from a hex string        *
*************************************************/
void OctetString::change(const std::string& hex_string)
   {
   SecureVector<byte> hex;
   for(u32bit j = 0; j != hex_string.length(); ++j)
      if(Hex_Decoder::is_valid(hex_string[j]))
         hex.append(hex_string[j]);

   if(hex.size() % 2 != 0)
      throw Invalid_Argument("OctetString: hex string must encode full bytes");
   bits.create(hex.size() / 2);
   for(u32bit j = 0; j != bits.size(); ++j)
      bits[j] = Hex_Decoder::decode(hex.begin() + 2*j);
   }

/*************************************************
* Create an OctetString from a byte string       *
*************************************************/
void OctetString::change(const byte in[], u32bit n)
   {
   bits.create(n);
   bits.copy(in, n);
   }

/*************************************************
* Set the parity of each key byte to odd         *
*************************************************/
void OctetString::set_odd_parity()
   {
   const byte ODD_PARITY[256] = {
      0x01, 0x01, 0x02, 0x02, 0x04, 0x04, 0x07, 0x07, 0x08, 0x08, 0x0B, 0x0B,
      0x0D, 0x0D, 0x0E, 0x0E, 0x10, 0x10, 0x13, 0x13, 0x15, 0x15, 0x16, 0x16,
      0x19, 0x19, 0x1A, 0x1A, 0x1C, 0x1C, 0x1F, 0x1F, 0x20, 0x20, 0x23, 0x23,
      0x25, 0x25, 0x26, 0x26, 0x29, 0x29, 0x2A, 0x2A, 0x2C, 0x2C, 0x2F, 0x2F,
      0x31, 0x31, 0x32, 0x32, 0x34, 0x34, 0x37, 0x37, 0x38, 0x38, 0x3B, 0x3B,
      0x3D, 0x3D, 0x3E, 0x3E, 0x40, 0x40, 0x43, 0x43, 0x45, 0x45, 0x46, 0x46,
      0x49, 0x49, 0x4A, 0x4A, 0x4C, 0x4C, 0x4F, 0x4F, 0x51, 0x51, 0x52, 0x52,
      0x54, 0x54, 0x57, 0x57, 0x58, 0x58, 0x5B, 0x5B, 0x5D, 0x5D, 0x5E, 0x5E,
      0x61, 0x61, 0x62, 0x62, 0x64, 0x64, 0x67, 0x67, 0x68, 0x68, 0x6B, 0x6B,
      0x6D, 0x6D, 0x6E, 0x6E, 0x70, 0x70, 0x73, 0x73, 0x75, 0x75, 0x76, 0x76,
      0x79, 0x79, 0x7A, 0x7A, 0x7C, 0x7C, 0x7F, 0x7F, 0x80, 0x80, 0x83, 0x83,
      0x85, 0x85, 0x86, 0x86, 0x89, 0x89, 0x8A, 0x8A, 0x8C, 0x8C, 0x8F, 0x8F,
      0x91, 0x91, 0x92, 0x92, 0x94, 0x94, 0x97, 0x97, 0x98, 0x98, 0x9B, 0x9B,
      0x9D, 0x9D, 0x9E, 0x9E, 0xA1, 0xA1, 0xA2, 0xA2, 0xA4, 0xA4, 0xA7, 0xA7,
      0xA8, 0xA8, 0xAB, 0xAB, 0xAD, 0xAD, 0xAE, 0xAE, 0xB0, 0xB0, 0xB3, 0xB3,
      0xB5, 0xB5, 0xB6, 0xB6, 0xB9, 0xB9, 0xBA, 0xBA, 0xBC, 0xBC, 0xBF, 0xBF,
      0xC1, 0xC1, 0xC2, 0xC2, 0xC4, 0xC4, 0xC7, 0xC7, 0xC8, 0xC8, 0xCB, 0xCB,
      0xCD, 0xCD, 0xCE, 0xCE, 0xD0, 0xD0, 0xD3, 0xD3, 0xD5, 0xD5, 0xD6, 0xD6,
      0xD9, 0xD9, 0xDA, 0xDA, 0xDC, 0xDC, 0xDF, 0xDF, 0xE0, 0xE0, 0xE3, 0xE3,
      0xE5, 0xE5, 0xE6, 0xE6, 0xE9, 0xE9, 0xEA, 0xEA, 0xEC, 0xEC, 0xEF, 0xEF,
      0xF1, 0xF1, 0xF2, 0xF2, 0xF4, 0xF4, 0xF7, 0xF7, 0xF8, 0xF8, 0xFB, 0xFB,
      0xFD, 0xFD, 0xFE, 0xFE };

   for(u32bit j = 0; j != bits.size(); ++j)
      bits[j] = ODD_PARITY[bits[j]];
   }

/*************************************************
* Hex encode an OctetString                      *
*************************************************/
std::string OctetString::as_string() const
   {
   Pipe pipe(new Hex_Encoder);
   pipe.process_msg(bits);
   return pipe.read_all_as_string();
   }

/*************************************************
* XOR Operation for OctetStrings                 *
*************************************************/
OctetString& OctetString::operator^=(const OctetString& k)
   {
   if(&k == this) { bits.clear(); return (*this); }
   xor_buf(bits.begin(), k.begin(), std::min(length(), k.length()));
   return (*this);
   }

/*************************************************
* Equality Operation for OctetStrings            *
*************************************************/
bool operator==(const OctetString& s1, const OctetString& s2)
   {
   return (s1.bits_of() == s2.bits_of());
   }

/*************************************************
* Unequality Operation for OctetStrings          *
*************************************************/
bool operator!=(const OctetString& s1, const OctetString& s2)
   {
   return !(s1 == s2);
   }

/*************************************************
* Append Operation for OctetStrings              *
*************************************************/
OctetString operator+(const OctetString& k1, const OctetString& k2)
   {
   return OctetString(SecureVector<byte>(k1.bits_of(), k2.bits_of()));
   }

/*************************************************
* XOR Operation for OctetStrings                 *
*************************************************/
OctetString operator^(const OctetString& k1, const OctetString& k2)
   {
   SecureVector<byte> ret(std::max(k1.length(), k2.length()));
   ret.copy(k1.begin(), k1.length());
   xor_buf(ret, k2.begin(), k2.length());
   return OctetString(ret);
   }

}
}
