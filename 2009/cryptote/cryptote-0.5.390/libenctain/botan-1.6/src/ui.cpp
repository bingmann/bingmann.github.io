/*************************************************
* User Interface Source File                     *
* (C) 1999-2007 The Botan Project                *
*************************************************/

#include "botan-1.6/include/ui.h"
#include "botan-1.6/include/libstate.h"

namespace Enctain {
namespace Botan {

/*************************************************
* Get a passphrase from the user                 *
*************************************************/
std::string User_Interface::get_passphrase(const std::string&,
                                           const std::string&,
                                           UI_Result& action) const
   {
   action = OK;

   if(!first_try)
      action = CANCEL_ACTION;

   return preset_passphrase;
   }

/*************************************************
* User_Interface Constructor                     *
*************************************************/
User_Interface::User_Interface(const std::string& preset) :
   preset_passphrase(preset)
   {
   first_try = true;
   }

}
}
