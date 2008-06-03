#include "dmzRuntimeContext.h"
#include "dmzRuntimeContextThreadKey.h"
#include "dmzRuntimeContextTime.h"
#include <dmzRuntimeHandle.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzRuntimeTime.h>
#include <dmzTypesString.h>

/*!

\file dmzRuntimeSync.h
\brief Contains Synchronization class.

\enum SyncTypeEnum
\ingroup Runtime
\brief Sync type enumerations.
\details Defined in dmzRuntimeSync.h.

\enum SyncModeEnum
\ingroup Runtime
\brief Sync mode enumerations.
\details Defined in dmzRuntimeSync.h.

\class dmz::Sync
\ingroup Runtime
\brief Class for creating and receiving a sync.
\details This class allows derived classes to have a call back invoke at a specified
interval. The sync call backs may be repeating or a single occurrence.
Syncs can either use system time which uses time straight from the
operating system or it can use runtime time which may be paused, run faster or slower
than real time, and even run backwards based on the current time factor. A repeating
sync callback begin immediately upon the next time slice. The single occurrence sync is
not started until dmz::Sync::start_sync is called. After a single occurrence sync call
back has been made, the sync may be scheduled for invocation again by calling
dmz::Sync::start_sync(). Any sync call may be halted by calling dmz::Sync::stop_sync().
A time interval of zero will be called every time slice if repeating.

*/

/*!

\brief Reserves next place in the Sync list for any Sync with the given Handle.
\details Defined in dmzRuntimeSync.h.
If this function is called using a Handle that already has a slot in the
Sync list, All Sync objects with that Handle will be moved to the end of the list.
\ingroup Runtime
\param[in] TheHandle Handle to use in reservation.
\param[in] context Pointer to the RuntimeContext.

*/
dmz::Boolean
dmz::reserve_sync_place (const Handle TheHandle, RuntimeContext *context) {

   Int32 result (0);

   RuntimeContextTime *timeContext = context ? context->get_time_context () : 0;

   if (timeContext) {

      result = timeContext->move_sync_to_end (TheHandle);
   }

   return result != 0;
}


struct dmz::Sync::State {

   SyncStruct *sync;
   RuntimeHandle *handlePtr;
   RuntimeContextTime *timeContext;
   RuntimeContextThreadKey *key;

   State (
         const Handle TheHandle,
         RuntimeContext *context) :
         sync (0),
         handlePtr (0),
         timeContext (0),
         key (0) {

      if (!TheHandle) { handlePtr = new RuntimeHandle ("Sync", context); }
      timeContext = context ? context->get_time_context () : 0;
      if (timeContext) { timeContext->ref (); }
      key = context ? context->get_thread_key () : 0;
      if (key) { key->ref (); }
   }

   ~State () {

      if (timeContext) {

         if (sync) { timeContext->remove_sync (*sync); }
         timeContext->unref ();
      }

      if (sync) { delete sync; }
      if (handlePtr) { delete handlePtr; handlePtr = 0; }
      if (key) { key->unref (); key = 0; }
   }
};


/*!

\brief Constructor.
\details The time interval is relative to when the sync is started. This constructor
uses SyncTypeRuntime and SyncModeRepeating as the defaults with a time interval of
0.0.
\param[in] SyncHandle Unique handle, if set to zero, one will be created internally.
\param[in] context Pointer to the runtime context.

*/
dmz::Sync::Sync (
      const Handle SyncHandle,
      RuntimeContext *context) :
      __state (*(new State (SyncHandle, context))) {

   if (__state.timeContext) {

      __state.sync = __state.timeContext->create_sync_struct (
         __state.handlePtr ? __state.handlePtr->get_runtime_handle () : SyncHandle,
         SyncTypeRuntime,
         SyncModeRepeating,
         0.0,
         *this);
   }
}


/*!

\brief Constructor.
\details The time interval is relative to when the sync is started.
\param[in] SyncHandle Unique handle, if set to zero, one will be created internally.
\param[in] Type dmz::SyncTypeEnum specifies runtime or system time.
\param[in] Mode dmz::SyncModeEnum specifies single or repeating sync.
\param[in] TimeInterval Length of time until sync in seconds.
\param[in] context Pointer to the runtime context.

*/
dmz::Sync::Sync (
      const Handle SyncHandle,
      const SyncTypeEnum Type,
      const SyncModeEnum Mode,
      const Float64 TimeInterval,
      RuntimeContext *context) :
      __state (*(new State (SyncHandle, context))) {

   if (__state.timeContext) {

      __state.sync = __state.timeContext->create_sync_struct (
         __state.handlePtr ? __state.handlePtr->get_runtime_handle () : SyncHandle,
         Type,
         Mode,
         TimeInterval,
         *this);
   }

}



/*!

\brief Constructor.
\details This constructor uses SyncTypeRuntime and SyncModeRepeating as the defaults
with a time interval of 0.0.
\param[in] Info dmz::PluginInfo which will be used for the unique handle and runtime

*/
dmz::Sync::Sync (const PluginInfo &Info) :
      __state (*(new State (Info.get_handle (), Info.get_context ()))) {

   if (__state.timeContext) {

      __state.sync = __state.timeContext->create_sync_struct (
         __state.handlePtr ?
            __state.handlePtr->get_runtime_handle () : Info.get_handle (),
         SyncTypeRuntime,
         SyncModeRepeating,
         0.0,
         *this);
   }
}

/*!

\brief Constructor.
\details The time interval is relative to when the sync is started.
\param[in] Info dmz::PluginInfo which will be used for the unique handle and runtime
context.
\param[in] Type dmz::SyncTypeEnum specifies runtime or system time.
\param[in] Mode dmz::SyncModeEnum specifies single or repeating sync.
\param[in] TimeInterval Length of time until sync in seconds.

*/
dmz::Sync::Sync (
      const PluginInfo &Info,
      const SyncTypeEnum Type,
      const SyncModeEnum Mode,
      const Float64 TimeInterval) :
      __state (*(new State (Info.get_handle (), Info.get_context ()))) {

   if (__state.timeContext) {

      __state.sync = __state.timeContext->create_sync_struct (
         __state.handlePtr ?
            __state.handlePtr->get_runtime_handle () : Info.get_handle (),
         Type,
         Mode,
         TimeInterval,
         *this);
   }
}


//! Destructor.
dmz::Sync::~Sync () { delete &__state; }


//! Returns unique handle.
dmz::Handle
dmz::Sync::get_sync_handle () const {

   return __state.sync ?  __state.sync->SyncHandle : 0;
}


//! Sets sync type.
void
dmz::Sync::set_sync_type (const SyncTypeEnum Type) {

   if (__state.sync) { __state.sync->set_type (Type); }
}

//! Returns sync type.
dmz::SyncTypeEnum
dmz::Sync::get_sync_type () const {

   return __state.sync ? __state.sync->type : SyncTypeRuntime;
}


//! Sets sync mode.
void
dmz::Sync::set_sync_mode (const SyncModeEnum Mode) {

   if (__state.sync) { __state.sync->set_mode (Mode); }
}


//! Returns sync mode.
dmz::SyncModeEnum
dmz::Sync::get_sync_mode () const {

   return __state.sync ? __state.sync->mode : SyncModeRepeating;
}


/*!

\brief Sets time interval until next sync.
\param[in] TimeInterval Length of time until sync in seconds.

*/
void
dmz::Sync::set_sync_interval (const Float64 TimeInterval) {

   if (__state.sync) { __state.sync->set_interval (TimeInterval); }
}


//! Returns time interval in seconds.
dmz::Float64
dmz::Sync::get_sync_interval () const {

   return __state.sync ? __state.sync->timeInterval : 0.0;
}


/*!

\brief Starts sync.
\details If this function is called before sync has expired, the sync is reset and
the full time interval must elapse before the sync callback will be invoked.

*/
void
dmz::Sync::start_sync () {

   if (__state.key && __state.key->is_main_thread () &&
         __state.sync && __state.timeContext) {

      __state.timeContext->stop_sync (*(__state.sync));
      __state.timeContext->start_sync (*(__state.sync));
   }
}


/*!

\brief Stops sync.
\details Works on both single and repeating syncs.

*/
void
dmz::Sync::stop_sync () {

   if (__state.key && __state.key->is_main_thread () &&
         __state.sync && __state.timeContext) {

      __state.timeContext->stop_sync (*(__state.sync));
   }
}



/*!

\brief Removes sync.
\details Removes Sync from sync list. Works on both single and repeating syncs.

*/
void
dmz::Sync::remove_sync () {

   if (__state.key && __state.key->is_main_thread () &&
         __state.sync && __state.timeContext) {

      __state.timeContext->remove_sync (*(__state.sync));
   }
}


/*!

\fn dmz::Sync::update_sync (const Float64 DeltaTime)
\brief Pure virtual function derived classes implement to receive sync callbacks.
\param[in] DeltaTime Number of seconds that have elapsed since either
dmz::Sync::start_sync was called or the last sync occurred.

*/

