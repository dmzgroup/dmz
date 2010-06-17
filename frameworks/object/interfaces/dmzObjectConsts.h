#ifndef DMZ_OBJECT_CONSTS_DOT_H
#define DMZ_OBJECT_CONSTS_DOT_H

#include <dmzTypesBase.h>
#include <dmzTypesString.h>

/*!

\file
\ingroup Object
\brief Defines attribute bit masks and object framework enumerations.

*/

namespace dmz {

//! \addtogroup Object
//! @{

   //! Object locality enumeration. Defined in dmzObjectConsts.h.
   enum ObjectLocalityEnum {
      ObjectLocalityUnknown, //!< Unknown Locality.
      ObjectLocal,           //!< Object is local.
      ObjectRemote,          //!< Object is remote.
   };

   //! Object link retention enumeration. Defined in dmzObjectConsts.h.
   enum ObjectLinkRetentionEnum {
      ObjectRetainLinks, //!< Retain links.
      ObjectIgnoreLinks, //!< Ignore links.
   };

   //! Object select enumeration. Defined in dmzObjectConsts.h.
   enum ObjectSelectModeEnum {
      ObjectSelectNew, //!< New selection.
      ObjectSelectAdd, //!< Add selection.
   };

   //! \brief Name of message sent when object is created in the ObjectModule.
   //! Defined in dmzObjectConsts.h.
   const char ObjectCreateMessageName[] = "Object_Create_Message";
   //! \brief Name of message sent when object is destroyed in the ObjectModule.
   //! Defined in dmzObjectConsts.h.
   const char ObjectDestroyMessageName[] = "Object_Destroy_Message";

   //! Default object attribute handle name. Defined in dmzObjectConsts.h.
   const char ObjectAttributeDefaultName[] = "Object_Default_Attribute";
   //! Last network value attribute handle name. Defined in dmzObjectConsts.h.
   const char ObjectAttributeLastNetworkValueName[] = "Object_Last_Network_Value";
   //! Human in the loop attribute handle name. Defined in dmzObjectConsts.h.
   const char ObjectAttributeHumanInTheLoopName[] = "Object_Human_In_The_Loop";
   //! Throttle attribute handle name. Defined in dmzObjectConsts.h.
   const char ObjectAttributeScalarThrottleName[] = "throttle";
   //! Selected object attribute handle name. Defined in dmzObjectConsts.h.
   const char ObjectAttributeSelectName[] = "Object_Select_Attribute";
   //! Highlighted object attribute handle name. Defined in dmzObjectConsts.h.
   const char ObjectAttributeHighlightName[] = "Object_Highlight_Attribute";
   //! Hidden object attribute handle name. Defined in dmzObjectConsts.h.
   const char ObjectAttributeHideName[] = "Object_Hide_Attribute";
   //! Object bounding volume radius attribute handle name. Defined in dmzObjectConsts.h.
   const char ObjectAttributeBoundingVolumeRaidusName[] =
      "Object_Bounding_Volume_Radius_Attribute";

   String create_last_network_value_name (const String &Value);
};


/*!

\brief Creates a last network value attribute handle name from a given name.
\details Defined in dmzObjectConsts.h.
\param[in] Value of attribute handle.
\return Returns a String containing the name of the last network value attribute handle
based on the name provided by \a Value.

*/
inline dmz::String
dmz::create_last_network_value_name (const String &Value) {

   String result (Value);

   if (Value == ObjectAttributeDefaultName) {

      result = ObjectAttributeLastNetworkValueName;
   }
   else { result << "_" << ObjectAttributeLastNetworkValueName; }

   return result;
}

//! @}

#endif // DMZ_OBJECT_CONSTS_DOT_H
