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
   //! Weapon lock attribute handle name. Defined in dmzWeaponsConsts.h
   const char WeaponAttributeTargetLockName[] = "Weapon_Target_Lock";
   String create_weapon_attribute_name (const Int32 Which);
//! @}
};


/*!

\brief Creates weapon attribute name.
\param[in] Which Indicates which weapon attribute.
\return Returns a String containing the weapon attribute name.

*/
inline dmz::String
dmz::create_weapon_attribute_name (const Int32 Which) {

   String result ("Weapon_");
   result << Which;
   return result;
}

#endif // DMZ_WEAPON_CONSTS_DOT_H
