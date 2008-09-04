#include <dmzRuntimeLog.h>
#include <dmzTypesStringTokenizer.h>
#include <dmzObjectAttributeMasks.h>
#include <dmzObjectMaskConsts.h>

//! \cond
const dmz::Mask dmz::ObjectCreateMask (0, dmz::ObjectAttributeCreateObject);
const dmz::Mask dmz::ObjectDestroyMask (0, dmz::ObjectAttributeDestroyObject);
const dmz::Mask dmz::ObjectLocalityMask (0, dmz::ObjectAttributeLocality);
const dmz::Mask dmz::ObjectUUIDMask (0, dmz::ObjectAttributeUUID);
const dmz::Mask dmz::ObjectRemoveAttributeMask (0, dmz::ObjectAttributeRemove);
const dmz::Mask dmz::ObjectLinkMask (0, dmz::ObjectAttributeLink);
const dmz::Mask dmz::ObjectUnlinkMask (0, dmz::ObjectAttributeUnlink);
const dmz::Mask dmz::ObjectLinkAttributeMask (0, dmz::ObjectAttributeLinkObject);
const dmz::Mask dmz::ObjectCounterMask (0, dmz::ObjectAttributeCounter);
const dmz::Mask dmz::ObjectMinCounterMask (0, dmz::ObjectAttributeCounterMin);
const dmz::Mask dmz::ObjectMaxCounterMask (0, dmz::ObjectAttributeCounterMax);
const dmz::Mask dmz::ObjectAltTypeMask (0, dmz::ObjectAttributeAltObjectType);
const dmz::Mask dmz::ObjectStateMask (0, dmz::ObjectAttributeState);
const dmz::Mask dmz::ObjectFlagMask (0, dmz::ObjectAttributeFlag);
const dmz::Mask dmz::ObjectTimeStampMask (0, dmz::ObjectAttributeTimeStamp);
const dmz::Mask dmz::ObjectPositionMask (0, dmz::ObjectAttributePosition);
const dmz::Mask dmz::ObjectOrientationMask (0, dmz::ObjectAttributeOrientation);
const dmz::Mask dmz::ObjectVelocityMask (0, dmz::ObjectAttributeVelocity);
const dmz::Mask dmz::ObjectAccelerationMask (0, dmz::ObjectAttributeAcceleration);
const dmz::Mask dmz::ObjectScaleMask (0, dmz::ObjectAttributeScale);
const dmz::Mask dmz::ObjectVectorMask (0, dmz::ObjectAttributeVector);
const dmz::Mask dmz::ObjectScalarMask (0, dmz::ObjectAttributeScalar);
const dmz::Mask dmz::ObjectTextMask (0, dmz::ObjectAttributeText);
const dmz::Mask dmz::ObjectDataMask (0, dmz::ObjectAttributeData);
const dmz::Mask dmz::ObjectAllMask (0, dmz::ObjectAttributeAll);
//! \endcond


/*!

\brief Looks up object attribute mask from names.
\param[in] Value String containing state names.
\param[in] log Pointer to the log to use for error messages.
\return Returns Mask object containing a mask of all found object attributes.
\note Attribute names should be delineated by the "|" character (a.k.a. the bitwise or
operator).

*/
dmz::Mask
dmz::string_to_object_attribute_mask (const String &Value, Log *log) {

   Mask result;

   StringTokenizer st (Value, '|');

   String token = st.get_next ().to_lower ();

   while (token) {

      if (token == "all") { result |= ObjectAllMask; }
      else if (token == "link") { result |= ObjectLinkMask; }
      else if (token == "counter") { result |= ObjectCounterMask; }
      else if (token == "mincounter") { result |= ObjectMinCounterMask; }
      else if (token == "maxcounter") { result |= ObjectMaxCounterMask; }
      else if (token == "alttype") { result |= ObjectAltTypeMask; }
      else if (token == "state") { result |= ObjectStateMask; }
      else if (token == "flag") { result |= ObjectFlagMask; }
      else if (token == "timestamp") { result |= ObjectTimeStampMask; }
      else if (token == "position") { result |= ObjectPositionMask; }
      else if (token == "orientation") { result |= ObjectOrientationMask; }
      else if (token == "velocity") { result |= ObjectVelocityMask; }
      else if (token == "acceleration") { result |= ObjectAccelerationMask; }
      else if (token == "scale") { result |= ObjectScaleMask; }
      else if (token == "vector") { result |= ObjectVectorMask; }
      else if (token == "scalar") { result |= ObjectScalarMask; }
      else if (token == "text") { result |= ObjectTextMask; }
      else if (token == "data") { result |= ObjectDataMask; }
      else if (log) {

         log->error << "Unknown attribute mask type: " << token << endl;
      }

      token = st.get_next ().to_lower ();
   }

   return result;
}


namespace {

inline void
local_add_or (dmz::String &value) { if (value) { value << " | "; } }

};


/*!

\brief Gets object attribute mask names.
\param[in] Value Mask containing object attribute masks to be named.
\return Returns dmz::String of all the attribute masks stored in \a Value that have names.
\note This function will find the names for as many attributes as possible.
If the \a Value
mask contains more than one attribute, the names are separated by the "|" character
(a.k.a. bitwise or operator).

*/
dmz::String
dmz::object_attribute_mask_to_string (const Mask &Value) {

   String result;

   if (Value.contains (ObjectAllMask)) {

      local_add_or (result);
      result << "all";
   }

   if (Value.contains (ObjectLinkMask)) {

      local_add_or (result);
      result << "link";
   }

   if (Value.contains (ObjectCounterMask)) {

      local_add_or (result);
      result << "counter";
   }

   if (Value.contains (ObjectMinCounterMask)) {

      local_add_or (result);
      result << "mincounter";
   }

   if (Value.contains (ObjectMaxCounterMask)) {

      local_add_or (result);
      result << "maxcounter";
   }

   if (Value.contains (ObjectAltTypeMask)) {

      local_add_or (result);
      result << "alttype";
   }

   if (Value.contains (ObjectStateMask)) {

      local_add_or (result);
      result << "state";
   }

   if (Value.contains (ObjectFlagMask)) {

      local_add_or (result);
      result << "flag";
   }

   if (Value.contains (ObjectTimeStampMask)) {

      local_add_or (result);
      result << "timestamp";
   }

   if (Value.contains (ObjectPositionMask)) {

      local_add_or (result);
      result << "position";
   }

   if (Value.contains (ObjectOrientationMask)) {

      local_add_or (result);
      result << "orientation";
   }

   if (Value.contains (ObjectVelocityMask)) {

      local_add_or (result);
      result << "velocity";
   }

   if (Value.contains (ObjectAccelerationMask)) {

      local_add_or (result);
      result << "acceleration";
   }

   if (Value.contains (ObjectScaleMask)) {

      local_add_or (result);
      result << "scale";
   }

   if (Value.contains (ObjectVectorMask)) {

      local_add_or (result);
      result << "vector";
   }

   if (Value.contains (ObjectScalarMask)) {

      local_add_or (result);
      result << "scalar";
   }

   if (Value.contains (ObjectTextMask)) {

      local_add_or (result);
      result << "text";
   }

   if (Value.contains (ObjectDataMask)) {

      local_add_or (result);
      result << "data";
   }

   return result;
}
