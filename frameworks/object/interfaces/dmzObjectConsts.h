#ifndef DMZ_OBJECT_CONSTS_DOT_H
#define DMZ_OBJECT_CONSTS_DOT_H

#include <dmzTypesBase.h>
#include <dmzTypesString.h>

/*!

\file
\brief Defines attribute bit masks and object framework enumerations.

*/

namespace dmz {

//! \addtogroup Object
//! @{

   //! Object locality enumeration.
   enum ObjectLocalityEnum {
      ObjectLocalityUnknown, //!< Unknown Locality.
      ObjectLocal,           //!< Object is local.
      ObjectRemote,          //!< Object is remote.
   };

   //! Object link retention enumeration.
   enum ObjectLinkRetentionEnum {
      ObjectRetainLinks, //!< Retain links.
      ObjectIgnoreLinks, //!< Ignore links.
   };

   //! Object select enumeration.
   enum ObjectSelectModeEnum {
      ObjectSelectNew, //!< New selection.
      ObjectSelectAdd, //!< Add selection.
   };

   //! Name of message sent when object is created in the ObjectModule.
   const char ObjectCreateMessageName[] = "Object_Create_Message";
   //! Name of message sent when object is destroyed in the ObjectModule.
   const char ObjectDestroyMessageName[] = "Object_Destroy_Message";

   //! Default object attribute handle name.
   const char ObjectAttributeDefaultName[] = "Default_Object_Attribute";
   //! Last network value attribute handle name.
   const char ObjectAttributeLastNetworkValueName[] = "Last_Network_Value";
   //! Human in the loop attribute handle name.
   const char ObjectAttributeHumanInTheLoopName[] = "Human_In_The_Loop";
   //! Throttle attribute handle name.
   const char ObjectAttributeScalarThrottleName[] = "throttle";
   //! Selected object attribute handle name.
   const char ObjectAttributeSelectName[] = "Select_Object_Attribute";

   String create_last_network_value_name (const String &Value);
};


/*!

\brief Creates a last network value attribute handle name from a given name.
\param[in] Value of attribute handle.
\return Returns a String containing the name of the last network value attribute handle
based on the name provided by \a Value.

*/
inline dmz::String
dmz::create_last_network_value_name (const String &Value) {

   String result (Value);

   result << "_" << ObjectAttributeLastNetworkValueName;

   return result;
}

//! @}

#endif // DMZ_OBJECT_CONSTS_DOT_H
