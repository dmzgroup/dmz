#ifndef DMZ_INPUT_EVENT_MASKS_DOT_H
#define DMZ_INPUT_EVENT_MASKS_DOT_H

#include <dmzInputUtilExport.h>
#include <dmzTypesMask.h>

/*!

\file dmzInputEventMasks.h
\ingroup Input
\brief Defines InputObserver channel subscription masks.
\sa dmz::InputObserverUtil::activate_input_channel()

*/


namespace dmz {

//! \addtogroup Input
//! @{

   //! Channel state change subscription mask.
   DMZ_INPUT_UTIL_LINK_SYMBOL extern const Mask InputEventChannelStateMask;
   //! Axis event subscription mask.
   DMZ_INPUT_UTIL_LINK_SYMBOL extern const Mask InputEventAxisMask;
   //! Button event subscription mask.
   DMZ_INPUT_UTIL_LINK_SYMBOL extern const Mask InputEventButtonMask;
   //! Switch event subscription mask.
   DMZ_INPUT_UTIL_LINK_SYMBOL extern const Mask InputEventSwitchMask;
   //! Key event subscription mask.
   DMZ_INPUT_UTIL_LINK_SYMBOL extern const Mask InputEventKeyMask;
   //! Mouse event subscription mask.
   DMZ_INPUT_UTIL_LINK_SYMBOL extern const Mask InputEventMouseMask;
   //! Data event subscription mask.
   DMZ_INPUT_UTIL_LINK_SYMBOL extern const Mask InputEventDataMask;
   //! All input callbacks subscription mask.
   DMZ_INPUT_UTIL_LINK_SYMBOL extern const Mask InputEventAllMask;

//! @}
};

#endif // DMZ_INPUT_EVENT_MASKS_DOT_H
