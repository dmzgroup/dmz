#ifndef DMZ_ENTITY_CONSTS_DOT_H
#define DMZ_ENTITY_CONSTS_DOT_H

#include <dmzTypesBase.h>
#include <dmzTypesString.h>

/*!

\file
\ingroup Entity
\brief Defines entity object attribute names.

*/

namespace dmz {

//! \addtogroup Entity
//! @{

   enum EntityWheelEnum {
      EntityWheelCenter,
      EntityWheelLeft,
      EntityWheelRight
   };

   //! \brief Name of entity health object attribute. Defined in dmzEntityConsts.h.
   const char EntityAttributeHealthName[] = "Entity_Health_Value";

   //! \brief Name of entity attach message. Defined in dmzEntityConsts.h.
   const char EntityMessageAttachName[] = "DMZ_Entity_Attach_Message";

   const char EntityWheelRootName[] = "Wheel_";
   
   String create_wheel_attribute_name (
      const String &Root,
      const EntityWheelEnum Side,
      const Int32 Which);
//! @}
};


inline dmz::String
dmz::create_wheel_attribute_name (
      const String &Root,
      const EntityWheelEnum Side,
      const Int32 Which) {

   String result (Root);
   if (Side == EntityWheelLeft) { result << "Left_"; }
   else if (Side == EntityWheelRight) { result << "Right_"; }
   else if (Side == EntityWheelCenter) { result << "Center_"; }
   result << Which;
   return result;
}


#endif // DMZ_ENTITY_CONSTS_DOT_H
