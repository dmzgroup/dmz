#include <dmzEventCallbackMasks.h>
#include <dmzEventConsts.h>

/*!

\file dmzEventCallbackMasks.h
\ingroup Event
\brief Defines EventObserver callback subscription masks.
\sa dmz::EventObserverUtil::activate_event_callback()

*/

//! \cond
const dmz::Mask dmz::EventCreateMask (0, dmz::EventCallbackCreateEvent);
const dmz::Mask dmz::EventCloseMask (0, dmz::EventCallbackCloseEvent);
const dmz::Mask dmz::EventAllMask (0, dmz::EventCallbackAll);
//! \endcond

