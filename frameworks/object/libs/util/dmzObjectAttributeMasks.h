#ifndef DMZ_OBJECT_ATTRIBUTE_MASKS_DOT_H
#define DMZ_OBJECT_ATTRIBUTE_MASKS_DOT_H

#include <dmzObjectUtilExport.h>
#include <dmzTypesMask.h>
/*!

\file dmzObjectAttributeMasks.h
\ingroup Object
\brief Defines ObjectObserver attribute subscription masks.
\sa dmz::ObjectObserverUtil::activate_object_attribute()

*/


namespace dmz {

   class Log;

//! \addtogroup Object
//! @{

   //! Create object ObjectObserverUtil subscription mask.
   DMZ_OBJECT_UTIL_LINK_SYMBOL extern const Mask ObjectCreateMask;
   //! Destroy object ObjectObserverUtil subscription mask.
   DMZ_OBJECT_UTIL_LINK_SYMBOL extern const Mask ObjectDestroyMask;
   //! Object UUID ObjectObserverUtil subscription mask.
   DMZ_OBJECT_UTIL_LINK_SYMBOL extern const Mask ObjectUUIDMask;
   //! Remove object attribute ObjectObserverUtil subscription mask.
   DMZ_OBJECT_UTIL_LINK_SYMBOL extern const Mask ObjectRemoveAttributeMask;
   //! Object link ObjectObserverUtil subscription mask.
   DMZ_OBJECT_UTIL_LINK_SYMBOL extern const Mask ObjectLinkMask;
   //! Object unlink ObjectObserverUtil subscription mask.
   DMZ_OBJECT_UTIL_LINK_SYMBOL extern const Mask ObjectUnlinkMask;
   //! Object link attribute object ObjectObserverUtil subscription mask.
   DMZ_OBJECT_UTIL_LINK_SYMBOL extern const Mask ObjectLinkAttributeMask;
   //! Object locality ObjectObserverUtil subscription mask.
   DMZ_OBJECT_UTIL_LINK_SYMBOL extern const Mask ObjectLocalityMask;
   //! Object type ObjectObserverUtil subscription mask.
   DMZ_OBJECT_UTIL_LINK_SYMBOL extern const Mask ObjectTypeMask;
   //! Object state ObjectObserverUtil subscription mask.
   DMZ_OBJECT_UTIL_LINK_SYMBOL extern const Mask ObjectStateMask;
   //! Object flag ObjectObserverUtil subscription mask.
   DMZ_OBJECT_UTIL_LINK_SYMBOL extern const Mask ObjectFlagMask;
   //! Object time stamp ObjectObserverUtil subscription mask.
   DMZ_OBJECT_UTIL_LINK_SYMBOL extern const Mask ObjectTimeStampMask;
   //! Object position ObjectObserverUtil subscription mask.
   DMZ_OBJECT_UTIL_LINK_SYMBOL extern const Mask ObjectPositionMask;
   //! Object orientation ObjectObserverUtil subscription mask.
   DMZ_OBJECT_UTIL_LINK_SYMBOL extern const Mask ObjectOrientationMask;
   //! Object velocity ObjectObserverUtil subscription mask.
   DMZ_OBJECT_UTIL_LINK_SYMBOL extern const Mask ObjectVelocityMask;
   //! Object acceleration ObjectObserverUtil subscription mask.
   DMZ_OBJECT_UTIL_LINK_SYMBOL extern const Mask ObjectAccelerationMask;
   //! Object scale ObjectObserverUtil subscription mask.
   DMZ_OBJECT_UTIL_LINK_SYMBOL extern const Mask ObjectScaleMask;
   //! Object vector ObjectObserverUtil subscription mask.
   DMZ_OBJECT_UTIL_LINK_SYMBOL extern const Mask ObjectVectorMask;
   //! Object scalar ObjectObserverUtil subscription mask.
   DMZ_OBJECT_UTIL_LINK_SYMBOL extern const Mask ObjectScalarMask;
   //! Object text ObjectObserverUtil subscription mask.
   DMZ_OBJECT_UTIL_LINK_SYMBOL extern const Mask ObjectTextMask;
   //! Object Data ObjectObserverUtil subscription mask.
   DMZ_OBJECT_UTIL_LINK_SYMBOL extern const Mask ObjectDataMask;
   //! All object attributes ObjectObserverUtil subscription mask.
   DMZ_OBJECT_UTIL_LINK_SYMBOL extern const Mask ObjectAllMask;

   DMZ_OBJECT_UTIL_LINK_SYMBOL Mask
   string_to_object_attribute_mask (const String &Value, Log *log = 0);

   DMZ_OBJECT_UTIL_LINK_SYMBOL String
   object_attribute_mask_to_string (const Mask &Value);

//! @}
};

#endif // DMZ_OBJECT_ATTRIBUTE_MASKS_DOT_H
