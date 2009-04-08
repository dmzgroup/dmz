#ifndef DMZ_WEAPON_CONSTS_DOT_H
#define DMZ_WEAPON_CONSTS_DOT_H

#include <dmzTypesBase.h>
#include <dmzTypesString.h>

/*!

\file
\ingroup Weapon
\brief Defines entity object attribute names.

*/

namespace dmz {

//! \addtogroup Weapon
//! @{

   String create_weapon_attribute_name (const Int32 Which);
//! @}
};


inline dmz::String
dmz::create_weapon_attribute_name (const Int32 Which) {

   String result ("Weapon_");
   result << Which;
   return result;
}

#endif // DMZ_WEAPON_CONSTS_DOT_H
