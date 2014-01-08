/*************************************************
* Exceptions Header File                         *
* (C) 1999-2007 The Botan Project                *
*************************************************/

#ifndef BOTAN_EXCEPTION_H__
#define BOTAN_EXCEPTION_H__

#include "botan-1.6/include/types.h"
#include "enctain.h"
#include <string>

namespace Enctain {
namespace Botan {

/*************************************************
* Exception Base Class                           *
*************************************************/
class Exception : public InternalError
   {
   public:
       Exception(const std::string& m = "Unknown error")
	  : InternalError(ETE_TEXT)
       { set_msg(m); }
   protected:
       void set_msg(const std::string& m) { m_msg = "Enctain: <Botan> " + m; }
   };

/*************************************************
* Invalid_Argument Exception                     *
*************************************************/
struct Invalid_Argument : public Exception
   {
   Invalid_Argument(const std::string& err = "") : Exception(err) {}
   };

/*************************************************
* Invalid_Key_Length Exception                   *
*************************************************/
struct Invalid_Key_Length : public Invalid_Argument
   {
   Invalid_Key_Length(const std::string&, u32bit);
   };

/*************************************************
* Invalid_Block_Size Exception                   *
*************************************************/
struct Invalid_Block_Size : public Invalid_Argument
   {
   Invalid_Block_Size(const std::string&, const std::string&);
   };

/*************************************************
* Invalid_IV_Length Exception                    *
*************************************************/
struct Invalid_IV_Length : public Invalid_Argument
   {
   Invalid_IV_Length(const std::string&, u32bit);
   };

/*************************************************
* Invalid_Message_Number Exception               *
*************************************************/
struct Invalid_Message_Number : public Invalid_Argument
   {
   Invalid_Message_Number(const std::string&, u32bit);
   };

/*************************************************
* Invalid_State Exception                        *
*************************************************/
struct Invalid_State : public Exception
   {
   Invalid_State(const std::string& err) : Exception(err) {}
   };

/*************************************************
* PRNG_Unseeded Exception                        *
*************************************************/
struct PRNG_Unseeded : public Invalid_State
   {
   PRNG_Unseeded(const std::string& algo) :
      Invalid_State("PRNG not seeded: " + algo) {}
   };

/*************************************************
* Policy_Violation Exception                     *
*************************************************/
struct Policy_Violation : public Invalid_State
   {
   Policy_Violation(const std::string& err) :
      Invalid_State("Policy violation: " + err) {}
   };

/*************************************************
* Lookup_Error Exception                         *
*************************************************/
struct Lookup_Error : public Exception
   {
   Lookup_Error(const std::string& err) : Exception(err) {}
   };

/*************************************************
* Algorithm_Not_Found Exception                  *
*************************************************/
struct Algorithm_Not_Found : public Exception
   {
   Algorithm_Not_Found(const std::string&);
   };

/*************************************************
* Format_Error Exception                         *
*************************************************/
struct Format_Error : public Exception
   {
   Format_Error(const std::string& err = "") : Exception(err) {}
   };

/*************************************************
* Invalid_Algorithm_Name Exception               *
*************************************************/
struct Invalid_Algorithm_Name : public Format_Error
   {
   Invalid_Algorithm_Name(const std::string&);
   };

/*************************************************
* Encoding_Error Exception                       *
*************************************************/
struct Encoding_Error : public Format_Error
   {
   Encoding_Error(const std::string& name) :
      Format_Error("Encoding error: " + name) {}
   };

/*************************************************
* Decoding_Error Exception                       *
*************************************************/
struct Decoding_Error : public Enctain::RuntimeError
   {
       Decoding_Error(const std::string& name)
	   : Enctain::RuntimeError(ETE_TEXT, "Decoding error: " + name)
       {}
   };

/*************************************************
* Stream_IO_Error Exception                      *
*************************************************/
struct Stream_IO_Error : public Exception
   {
   Stream_IO_Error(const std::string& err) :
      Exception("I/O error: " + err) {}
   };

/*************************************************
* Configuration Error Exception                  *
*************************************************/
struct Config_Error : public Format_Error
   {
   Config_Error(const std::string& err) :
      Format_Error("Config error: " + err) {}
   Config_Error(const std::string&, u32bit);
   };

/*************************************************
* Integrity Failure Exception                    *
*************************************************/
struct Integrity_Failure : public Exception
   {
   Integrity_Failure(const std::string& err) :
      Exception("Integrity failure: " + err) {}
   };

/*************************************************
* Internal_Error Exception                       *
*************************************************/
struct Internal_Error : public Exception
   {
   Internal_Error(const std::string& err) :
      Exception("Internal error: " + err) {}
   };

/*************************************************
* Self Test Failure Exception                    *
*************************************************/
struct Self_Test_Failure : public Internal_Error
   {
   Self_Test_Failure(const std::string& err) :
      Internal_Error("Self test failed: " + err) {}
   };

/*************************************************
* Memory Allocation Exception                    *
*************************************************/
struct Memory_Exhaustion : public Exception
   {
   Memory_Exhaustion() :
      Exception("Ran out of memory, allocation failed") {}
   };

}
}

#endif
