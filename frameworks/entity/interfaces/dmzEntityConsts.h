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

   //! Enum describes wheel type. Defined in dmzEntityConsts.h.
   enum EntityWheelEnum {
      EntityWheelCenter, //!< Center wheel.
      EntityWheelLeft,   //!< Left Wheel.
      EntityWheelRight   //!< Right Wheel.
   };

   //! \brief Name of entity health object attribute. Defined in dmzEntityConsts.h.
   const char EntityAttributeHealthName[] = "Entity_Health_Value";

   //! \brief Name of entity attach message. Defined in dmzEntityConsts.h.
   const char EntityMessageAttachName[] = "DMZ_Entity_Attach_Message";

   //! \brief String root used to create wheel attribute names.
   //! Defined in dmzEntityConsts.h
   const char EntityWheelRootName[] = "Entity_Wheel_";

   String create_wheel_attribute_name (
      const String &Root,
      const EntityWheelEnum Side,
      const Int32 Which);
//! @}
};


/*!

\brief Creates wheel attribute names.
\ingroup Entity
\details Defined in dmzEntityConsts.h.
\param[in] Root String containing the root value to use in the attribute name.
\param[in] Side Enum specifying the side of the entity the wheel is located.
\param[in] Which specifies which wheel.
\return Returns a String containing the attribute name for the specified wheel.

*/
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
