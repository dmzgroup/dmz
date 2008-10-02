#ifndef DMZ_OBJECT_ATTRIBUTE_MASKS_DOT_H
#define DMZ_OBJECT_ATTRIBUTE_MASKS_DOT_H

#include <dmzObjectUtilExport.h>
#include <dmzTypesMask.h>

namespace dmz {

   class Log;

//! \addtogroup Object
//! @{

   //! \brief Create object ObjectObserverUtil subscription mask.
   //! Defined in dmzObjectAttributeMasks.h.
   DMZ_OBJECT_UTIL_LINK_SYMBOL extern const Mask ObjectCreateMask;
   //! \brief Destroy object ObjectObserverUtil subscription mask.
   //! Defined in dmzObjectAttributeMasks.h.
   DMZ_OBJECT_UTIL_LINK_SYMBOL extern const Mask ObjectDestroyMask;
   //! \brief Object locality ObjectObserverUtil subscription mask.
   //! Defined in dmzObjectAttributeMasks.h.
   DMZ_OBJECT_UTIL_LINK_SYMBOL extern const Mask ObjectLocalityMask;
   //! \brief Object UUID ObjectObserverUtil subscription mask.
   //! Defined in dmzObjectAttributeMasks.h.
   DMZ_OBJECT_UTIL_LINK_SYMBOL extern const Mask ObjectUUIDMask;
   //! \brief Remove object attribute ObjectObserverUtil subscription mask.
   //! Defined in dmzObjectAttributeMasks.h.
   DMZ_OBJECT_UTIL_LINK_SYMBOL extern const Mask ObjectRemoveAttributeMask;
   //! \brief Object link ObjectObserverUtil subscription mask.
   //! Defined in dmzObjectAttributeMasks.h.
   DMZ_OBJECT_UTIL_LINK_SYMBOL extern const Mask ObjectLinkMask;
   //! \brief Object unlink ObjectObserverUtil subscription mask.
   //! Defined in dmzObjectAttributeMasks.h.
   DMZ_OBJECT_UTIL_LINK_SYMBOL extern const Mask ObjectUnlinkMask;
   //! \brief Object link attribute object ObjectObserverUtil subscription mask.
   //! Defined in dmzObjectAttributeMasks.h.
   DMZ_OBJECT_UTIL_LINK_SYMBOL extern const Mask ObjectLinkAttributeMask;
   //! \brief Object counter ObjectObserverUtil subscription mask.
   //! Defined in dmzObjectAttributeMasks.h.
   DMZ_OBJECT_UTIL_LINK_SYMBOL extern const Mask ObjectCounterMask;
   //! \brief Object minimum counter ObjectObserverUtil subscription mask.
   //! Defined in dmzObjectAttributeMasks.h.
   DMZ_OBJECT_UTIL_LINK_SYMBOL extern const Mask ObjectMinCounterMask;
   //! \brief Object maximum counter ObjectObserverUtil subscription mask.
   //! Defined in dmzObjectAttributeMasks.h.
   DMZ_OBJECT_UTIL_LINK_SYMBOL extern const Mask ObjectMaxCounterMask;
   //! \brief Object type ObjectObserverUtil subscription mask.
   //! Defined in dmzObjectAttributeMasks.h.
   DMZ_OBJECT_UTIL_LINK_SYMBOL extern const Mask ObjectAltTypeMask;
   //! \brief Object state ObjectObserverUtil subscription mask.
   //! Defined in dmzObjectAttributeMasks.h.
   DMZ_OBJECT_UTIL_LINK_SYMBOL extern const Mask ObjectStateMask;
   //! \brief Object flag ObjectObserverUtil subscription mask.
   //! Defined in dmzObjectAttributeMasks.h.
   DMZ_OBJECT_UTIL_LINK_SYMBOL extern const Mask ObjectFlagMask;
   //! \brief Object time stamp ObjectObserverUtil subscription mask.
   //! Defined in dmzObjectAttributeMasks.h.
   DMZ_OBJECT_UTIL_LINK_SYMBOL extern const Mask ObjectTimeStampMask;
   //! \brief Object position ObjectObserverUtil subscription mask.
   //! Defined in dmzObjectAttributeMasks.h.
   DMZ_OBJECT_UTIL_LINK_SYMBOL extern const Mask ObjectPositionMask;
   //! \brief Object orientation ObjectObserverUtil subscription mask.
   //! Defined in dmzObjectAttributeMasks.h.
   DMZ_OBJECT_UTIL_LINK_SYMBOL extern const Mask ObjectOrientationMask;
   //! \brief Object velocity ObjectObserverUtil subscription mask.
   //! Defined in dmzObjectAttributeMasks.h.
   DMZ_OBJECT_UTIL_LINK_SYMBOL extern const Mask ObjectVelocityMask;
   //! \brief Object acceleration ObjectObserverUtil subscription mask.
   //! Defined in dmzObjectAttributeMasks.h.
   DMZ_OBJECT_UTIL_LINK_SYMBOL extern const Mask ObjectAccelerationMask;
   //! \brief Object scale ObjectObserverUtil subscription mask.
   //! Defined in dmzObjectAttributeMasks.h.
   DMZ_OBJECT_UTIL_LINK_SYMBOL extern const Mask ObjectScaleMask;
   //! \brief Object vector ObjectObserverUtil subscription mask.
   //! Defined in dmzObjectAttributeMasks.h.
   DMZ_OBJECT_UTIL_LINK_SYMBOL extern const Mask ObjectVectorMask;
   //! \brief Object scalar ObjectObserverUtil subscription mask.
   //! Defined in dmzObjectAttributeMasks.h.
   DMZ_OBJECT_UTIL_LINK_SYMBOL extern const Mask ObjectScalarMask;
   //! \brief Object text ObjectObserverUtil subscription mask.
   //! Defined in dmzObjectAttributeMasks.h.
   DMZ_OBJECT_UTIL_LINK_SYMBOL extern const Mask ObjectTextMask;
   //! \brief Object Data ObjectObserverUtil subscription mask.
   //! Defined in dmzObjectAttributeMasks.h.
   DMZ_OBJECT_UTIL_LINK_SYMBOL extern const Mask ObjectDataMask;
   //! \brief All object attributes ObjectObserverUtil subscription mask.
   //! Defined in dmzObjectAttributeMasks.h.
   DMZ_OBJECT_UTIL_LINK_SYMBOL extern const Mask ObjectAllMask;

   DMZ_OBJECT_UTIL_LINK_SYMBOL Mask
   string_to_object_attribute_mask (const String &Value, Log *log = 0);

   DMZ_OBJECT_UTIL_LINK_SYMBOL String
   object_attribute_mask_to_string (const Mask &Value);

//! @}
};

#endif // DMZ_OBJECT_ATTRIBUTE_MASKS_DOT_H
