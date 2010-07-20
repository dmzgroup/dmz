#include "dmzRuntimeContext.h"
#include "dmzRuntimeContextThreadKey.h"
#include "dmzRuntimeContextTime.h"
#include <dmzRuntimeHandle.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzRuntimeTime.h>
#include <dmzTypesString.h>

/*!

\file dmzRuntimeTimeSlice.h
\ingroup Runtime
\brief Contains TimeSlice class.

\enum TimeSliceTypeEnum
\ingroup Runtime
\brief TimeSlice type enumerations.
\details Defined in dmzRuntimeTimeSlice.h.

\enum TimeSliceModeEnum
\ingroup Runtime
\brief TimeSlice mode enumerations.
\details Defined in dmzRuntimeTimeSlice.h.

\class dmz::TimeSlice
\ingroup Runtime
\brief Class for creating and receiving a time slice.
\details This class allows derived classes to have a call back invoke at a specified
interval. The time slice call backs may be repeating or a single occurrence.
TimeSlices can either use system time which uses time straight from the
operating system or it can use runtime time which may be paused, run faster or slower
than real time, and even run backwards based on the current time factor. A repeating
time slice callback begin immediately upon the next time slice. The single occurrence
time slice is
not started until dmz::TimeSlice::start_time_slice is called. After a single occurrence
time slice call
back has been made, the time slice may be scheduled for invocation again by calling
dmz::TimeSlice::start_time_slice(). Any time slice call may be halted by calling
dmz::TimeSlice::stop_time_slice().
A time interval of zero will be called every time slice if repeating.

*/

/*!

\brief Reserves next place in the TimeSlice list for any TimeSlice with the given Handle.
\details Defined in dmzRuntimeTimeSlice.h.
If this function is called using a Handle that already has a slot in the
TimeSlice list, All TimeSlice objects with that Handle will be moved to the end of the
list.
\ingroup Runtime
\param[in] TheHandle Handle to use in reservation.
\param[in] context Pointer to the RuntimeContext.

*/
dmz::Boolean
dmz::reserve_time_slice_place (const Handle TheHandle, RuntimeContext *context) {

   Int32 result (0);

   RuntimeContextTime *timeContext = context ? context->get_time_context () : 0;

   if (timeContext) {

      result = timeContext->move_time_slice_to_end (TheHandle);
   }

   return result != 0;
}


struct dmz::TimeSlice::State {

   TimeSliceStruct *timeSlice;
   RuntimeHandle *handlePtr;
   RuntimeContextTime *timeContext;
   RuntimeContextThreadKey *key;

   State (
         const Handle TheHandle,
         RuntimeContext *context) :
         timeSlice (0),
         handlePtr (0),
         timeContext (0),
         key (0) {

      if (!TheHandle) { handlePtr = new RuntimeHandle ("TimeSlice", context); }
      timeContext = context ? context->get_time_context () : 0;
      if (timeContext) { timeContext->ref (); }
      key = context ? context->get_thread_key () : 0;
      if (key) { key->ref (); }
   }

   ~State () {

      if (timeContext) {

         if (timeSlice) { timeContext->remove_time_slice (*timeSlice); }
         timeContext->unref ();
      }

      if (timeSlice) { delete timeSlice; }
      if (handlePtr) { delete handlePtr; handlePtr = 0; }
      if (key) { key->unref (); key = 0; }
   }
};


/*!

\brief Constructor.
\details The time interval is relative to when the time slice is started. This constructor
uses TimeSliceTypeRuntime and TimeSliceModeRepeating as the defaults with a time
interval of 0.0.
\param[in] TimeSliceHandle Unique handle, if set to zero, one will be created internally.
\param[in] context Pointer to the runtime context.

*/
dmz::TimeSlice::TimeSlice (
      const Handle TimeSliceHandle,
      RuntimeContext *context) :
      __state (*(new State (TimeSliceHandle, context))) {

   if (__state.timeContext) {

      __state.timeSlice = __state.timeContext->create_time_slice_struct (
         __state.handlePtr ? __state.handlePtr->get_runtime_handle () : TimeSliceHandle,
         TimeSliceTypeRuntime,
         TimeSliceModeRepeating,
         0.0,
         *this);
   }
}


/*!

\brief Constructor.
\details The time interval is relative to when the time slice is started.
\param[in] TimeSliceHandle Unique handle, if set to zero, one will be created internally.
\param[in] Type dmz::TimeSliceTypeEnum specifies runtime or system time.
\param[in] Mode dmz::TimeSliceModeEnum specifies single or repeating time slice.
\param[in] TimeInterval Length of time until time slice in seconds.
\param[in] context Pointer to the runtime context.

*/
dmz::TimeSlice::TimeSlice (
      const Handle TimeSliceHandle,
      const TimeSliceTypeEnum Type,
      const TimeSliceModeEnum Mode,
      const Float64 TimeInterval,
      RuntimeContext *context) :
      __state (*(new State (TimeSliceHandle, context))) {

   if (__state.timeContext) {

      __state.timeSlice = __state.timeContext->create_time_slice_struct (
         __state.handlePtr ? __state.handlePtr->get_runtime_handle () : TimeSliceHandle,
         Type,
         Mode,
         TimeInterval,
         *this);
   }

}



/*!

\brief Constructor.
\details This constructor uses TimeSliceTypeRuntime and TimeSliceModeRepeating as the
defaults with a time interval of 0.0.
\param[in] Info dmz::PluginInfo which will be used for the unique handle and runtime

*/
dmz::TimeSlice::TimeSlice (const PluginInfo &Info) :
      __state (*(new State (Info.get_handle (), Info.get_context ()))) {

   if (__state.timeContext) {

      __state.timeSlice = __state.timeContext->create_time_slice_struct (
         __state.handlePtr ?
            __state.handlePtr->get_runtime_handle () : Info.get_handle (),
         TimeSliceTypeRuntime,
         TimeSliceModeRepeating,
         0.0,
         *this);
   }
}

/*!

\brief Constructor.
\details The time interval is relative to when the time slice is started.
\param[in] Info dmz::PluginInfo which will be used for the unique handle and runtime
context.
\param[in] Type dmz::TimeSliceTypeEnum specifies runtime or system time.
\param[in] Mode dmz::TimeSliceModeEnum specifies single or repeating time slice.
\param[in] TimeInterval Length of time until time slice in seconds.

*/
dmz::TimeSlice::TimeSlice (
      const PluginInfo &Info,
      const TimeSliceTypeEnum Type,
      const TimeSliceModeEnum Mode,
      const Float64 TimeInterval) :
      __state (*(new State (Info.get_handle (), Info.get_context ()))) {

   if (__state.timeContext) {

      __state.timeSlice = __state.timeContext->create_time_slice_struct (
         __state.handlePtr ?
            __state.handlePtr->get_runtime_handle () : Info.get_handle (),
         Type,
         Mode,
         TimeInterval,
         *this);
   }
}


//! Destructor.
dmz::TimeSlice::~TimeSlice () { delete &__state; }


//! Returns unique handle.
dmz::Handle
dmz::TimeSlice::get_time_slice_handle () const {

   return __state.timeSlice ?  __state.timeSlice->TimeSliceHandle : 0;
}


//! Sets time slice type.
void
dmz::TimeSlice::set_time_slice_type (const TimeSliceTypeEnum Type) {

   if (__state.timeSlice) { __state.timeSlice->set_type (Type); }
}

//! Returns time slice type.
dmz::TimeSliceTypeEnum
dmz::TimeSlice::get_time_slice_type () const {

   return __state.timeSlice ? __state.timeSlice->type : TimeSliceTypeRuntime;
}


//! Sets time slice mode.
void
dmz::TimeSlice::set_time_slice_mode (const TimeSliceModeEnum Mode) {

   if (__state.timeSlice) { __state.timeSlice->set_mode (Mode); }
}


//! Returns time slice mode.
dmz::TimeSliceModeEnum
dmz::TimeSlice::get_time_slice_mode () const {

   return __state.timeSlice ? __state.timeSlice->mode : TimeSliceModeRepeating;
}


/*!

\brief Sets time interval until next time slice.
\param[in] TimeInterval Length of time until time slice in seconds.

*/
void
dmz::TimeSlice::set_time_slice_interval (const Float64 TimeInterval) {

   if (__state.timeSlice) { __state.timeSlice->set_interval (TimeInterval); }
}


//! Returns time interval in seconds.
dmz::Float64
dmz::TimeSlice::get_time_slice_interval () const {

   return __state.timeSlice ? __state.timeSlice->timeInterval : 0.0;
}


/*!

\brief Starts time slice.
\details If this function is called before time slice has expired, the time slice is
reset and the full time interval must elapse before the time slice callback will be
invoked.

*/
void
dmz::TimeSlice::start_time_slice () {

   if (__state.key && __state.key->is_main_thread () &&
         __state.timeSlice && __state.timeContext) {

      __state.timeContext->stop_time_slice (*(__state.timeSlice));
      __state.timeContext->start_time_slice (*(__state.timeSlice));
   }
}


/*!

\brief Stops time slice.
\details Works on both single and repeating time slices.

*/
void
dmz::TimeSlice::stop_time_slice () {

   if (__state.key && __state.key->is_main_thread () &&
         __state.timeSlice && __state.timeContext) {

      __state.timeContext->stop_time_slice (*(__state.timeSlice));
   }
}



/*!

\brief Removes time slice.
\details Removes TimeSlice from time slice list. Works on both single and repeating time
slices.

*/
void
dmz::TimeSlice::remove_time_slice () {

   if (__state.key && __state.key->is_main_thread () &&
         __state.timeSlice && __state.timeContext) {

      __state.timeContext->remove_time_slice (*(__state.timeSlice));
   }
}


/*!

\fn dmz::TimeSlice::update_time_slice (const Float64 DeltaTime)
\brief Pure virtual function derived classes implement to receive time slice callbacks.
\param[in] DeltaTime Number of seconds that have elapsed since either
dmz::TimeSlice::start_time_slice was called or the last time slice occurred.

*/

