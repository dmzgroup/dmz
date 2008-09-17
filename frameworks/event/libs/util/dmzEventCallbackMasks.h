#ifndef DMZ_EVENT_CALLBACK_MASKS_DOT_H
#define DMZ_EVENT_CALLBACK_MASKS_DOT_H

#include <dmzEventUtilExport.h>
#include <dmzTypesMask.h>

namespace dmz {

//! \addtogroup Event
//! @{

   //! Create event EventObserver callback subscription mask.
   DMZ_EVENT_UTIL_LINK_SYMBOL extern const Mask EventCreateMask;
   //! Close event EventObserver callback subscription mask.
   DMZ_EVENT_UTIL_LINK_SYMBOL extern const Mask EventCloseMask;
   //! All event EventObserver callback subscription mask.
   DMZ_EVENT_UTIL_LINK_SYMBOL extern const Mask EventAllMask;
//! @}
};

#endif // DMZ_EVENT_CALLBACK_MASKS_DOT_H
