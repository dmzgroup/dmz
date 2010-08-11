#include "dmzRuntimeContext.h"
#include "dmzRuntimeContextDefinitions.h"
#include <dmzRuntimeDataConverterTypesBase.h>
#include <dmzRuntimeExit.h>
#include "dmzRuntimeMessageContext.h"
#include <dmzRuntimeMessaging.h>
#include <dmzRuntimePluginInfo.h>

/*!

\file dmzRuntimeExit.h
\brief Contains class used to request platform exit.

\class dmz::Exit
\ingroup Runtime
\brief Class used to request the application exit.

\enum ExitStatusEnum
\ingroup Runtime
\brief Exit status enumerations.
\details Defined in dmzRuntimeExit.h.

*/

/*!

\var dmz::ExitStatusEnum dmz::ExitStatusError
\brief Exit requested due to error condition.
\details Defined in dmzRuntimeExit.h.

*/

/*!

\var dmz::ExitStatusEnum dmz::ExitStatusNormal
\brief Standard exit requested.
\details Defined in dmzRuntimeExit.h.

*/

/*!

\var dmz::ExitStatusEnum dmz::ExitStatusForced
\brief Forced exit requested.
\details Defined in dmzRuntimeExit.h.

*/

namespace {

   const char LocalNormalExitMessageName[] = "DMZ_NORMAL_EXIT_MESSAGE";
   const char LocalForcedExitMessageName[] = "DMZ_FORCED_EXIT_MESSAGE";
   const char LocalErrorExitMessageName[] = "DMZ_ERROR_EXIT_MESSAGE";

   static dmz::Message
   local_get_exit_message (const dmz::String &Name, dmz::RuntimeContext *context) {

      dmz::Message result;

      if (context) {

         dmz::RuntimeContextDefinitions *defs (context->get_definitions_context ());

         if (defs) {

            result = defs->create_message (
               Name,
               "",
               context,
               context->get_messaging_context ());
         }
      }

      return result;
   }

   static dmz::Message
   local_get_normal_exit_message (dmz::RuntimeContext *context) {

      return local_get_exit_message (LocalNormalExitMessageName, context);
   }

   static dmz::Message
   local_get_forced_exit_message (dmz::RuntimeContext *context) {

      return local_get_exit_message (LocalForcedExitMessageName, context);
   }

   static dmz::Message
   local_get_error_exit_message (dmz::RuntimeContext *context) {

      return local_get_exit_message (LocalErrorExitMessageName, context);
   }

   class exitMessageObserver : public dmz::MessageObserver {

      public:
         const dmz::Message NormalMsg;
         const dmz::Message ForcedMsg;
         const dmz::Message ErrorMsg;
         dmz::ExitObserver *callback;
         dmz::DataConverterString convert;

         exitMessageObserver (dmz::RuntimeContext *context) :
            MessageObserver (0, "exitMessageObserver", context),
            NormalMsg (local_get_normal_exit_message (context)),
            ForcedMsg (local_get_forced_exit_message (context)),
            ErrorMsg (local_get_error_exit_message (context)),
            callback (0),
            convert (context) {;}

         ~exitMessageObserver () { callback = 0; }

         void receive_message (
               const dmz::Message &Msg,
               const dmz::UInt32 MessageSendHandle,
               const dmz::Handle TargetObserverHandle,
               const dmz::Data *InData,
               dmz::Data *outData) {

            if (callback) {

               if (Msg == NormalMsg) {

                  callback->exit_requested (
                     dmz::ExitStatusNormal,
                     convert.to_string (InData));
               }
               else if (Msg == ForcedMsg) {

                  callback->exit_requested (
                     dmz::ExitStatusForced,
                     convert.to_string (InData));
               }
               else if (Msg == ErrorMsg) {

                  callback->exit_requested (
                     dmz::ExitStatusError,
                     convert.to_string (InData));
               }
            }
         }
   };
};


struct dmz::Exit::State {

   const Message NormalMsg;
   const Message ForcedMsg;
   const Message ErrorMsg;
   DataConverterString convert;

   State (RuntimeContext *context) :
         NormalMsg (local_get_normal_exit_message (context)),
         ForcedMsg (local_get_forced_exit_message (context)),
         ErrorMsg (local_get_error_exit_message (context)),
         convert (context) {;}
};


/*!

\brief Constructor
\param[in] context Pointer to runtime context.

*/
dmz::Exit::Exit (RuntimeContext *context) : _state (*(new State (context))) {;}


/*!
\brief Constructor
\param[in] Info Reference to the PluginInfo.
*/
dmz::Exit::Exit (const PluginInfo &Info) : _state (*(new State (Info.get_context ()))) {;}


//! Destructor.
dmz::Exit::~Exit () { delete &_state; }


/*!

\brief Request exit.
\details This call is used to request that the system exit. It is not guaranteed to
cause an exit.
\param[in] Status dmz::ExitStatusEnum indicating the status of the request.
\param[in] ExitReason dmz::String containing a description of the request.

*/
void
dmz::Exit::request_exit (const ExitStatusEnum Status, const String &ExitReason) {

   Data value (_state.convert.to_data (ExitReason));

   if (Status == ExitStatusNormal) {

      _state.NormalMsg.send (value ? &value : 0);
   }
   else if (Status == ExitStatusForced) {

      _state.ForcedMsg.send (value ? &value : 0);
   }
   else if (Status == ExitStatusError) {

      _state.ErrorMsg.send (value ? &value : 0);
   }
}


struct dmz::TimedExit::State {

   ExitStatusEnum status;

   State (const ExitStatusEnum Status) : status (Status) {;}
};


/*!

\class dmz::TimedExit
\ingroup Runtime
\brief Class requests exit after specified time elapses.
\details This class is useful in Plugin unit tests where the test is waiting for an
event that should occur with in a known time limit.

*/

/*!

\brief Constructor.
\details This class will request an exit after a set time interval has elapsed.
\param[in] TimeInterval Number of seconds to wait before requesting an exit.
\param[in] Status dmz::ExitStatusEnum to use in the exit request.
\param[in] context Pointer to the runtime context.

*/
dmz::TimedExit::TimedExit (
      const Float64 TimeInterval,
      const ExitStatusEnum Status,
      RuntimeContext *context) :
      TimeSlice (0, TimeSliceTypeSystemTime, TimeSliceModeSingle, TimeInterval, context),
      Exit (context),
      _state (*(new State (Status))) {;}


//! Destructor
dmz::TimedExit::~TimedExit () { delete &_state; }


/*!

\brief Gets the dmz::ExitStatusEnum to be used in the exit request.
\return Returns the dmz::ExitStatusEnum to be used in the exit request.

*/
dmz::ExitStatusEnum
dmz::TimedExit::get_status () const { return _state.status; }


/*!

\brief Set exit status to use in exit request.
\param[in] Status dmz::ExitStatusEnum to use in the exit request.

*/
void
dmz::TimedExit::set_status (const ExitStatusEnum Status) {

   _state.status = Status;
}


//! Internal call.
void
dmz::TimedExit::update_time_slice (const Float64 DeltaTime) {

   request_exit (ExitStatusNormal, "Exit timer expired");
}


struct dmz::ExitObserver::State {

   exitMessageObserver obs;

   State (RuntimeContext *context) : obs (context) {;}
};


/*!

\class dmz::ExitObserver
\ingroup Runtime
\brief Interface for watching for exit request from dmz::Exit class.
\details Watches for exit requests and calls the pure virtual function
dmz::ExitObserver::exit_requested
when an exit request is received. Classes wishing to be notified when an exit
is requested should be derived from this class and implement the pure virtual
function exit_requested.

*/

/*!

\brief Constructor.
\param[in] Info Reference to a PluginInfo.

*/
dmz::ExitObserver::ExitObserver (const PluginInfo &Info) :
      __state (*(new State (Info.get_context ()))) {

   __state.obs.callback = this;
   __state.obs.subscribe_to_message (__state.obs.NormalMsg);
   __state.obs.subscribe_to_message (__state.obs.ForcedMsg);
   __state.obs.subscribe_to_message (__state.obs.ErrorMsg);
}

/*!

\brief Constructor.
\param[in] context Pointer to runtime context.

*/
dmz::ExitObserver::ExitObserver (RuntimeContext *context) :
      __state (*(new State (context))) {

   __state.obs.callback = this;
   __state.obs.subscribe_to_message (__state.obs.NormalMsg);
   __state.obs.subscribe_to_message (__state.obs.ForcedMsg);
   __state.obs.subscribe_to_message (__state.obs.ErrorMsg);
}


//! Destructor.
dmz::ExitObserver::~ExitObserver () { delete &__state; }

/*!

\fn dmz::ExitObserver::exit_requested (
   const ExitStatusEnum Status, const String &ExitReason)
\brief Pure virtual function that receives the exit request.
\param[in] Status dmz::ExitStatusEnum for the request.
\param[in] ExitReason String containing the reason for the exit request.

*/


struct dmz::ExitObserverBasic::State {

   Boolean exitRequested;
   ExitStatusEnum status;
   String reason;

   State () : exitRequested (False), status (ExitStatusNormal) {;}
};


/*!

\class dmz::ExitObserverBasic
\ingroup Runtime
\brief Watches for an exit request and raises flags when exit is requested.

*/

/*!

\brief Constructor.
\param[in] context Pointer to runtime context.

*/
dmz::ExitObserverBasic::ExitObserverBasic (RuntimeContext *context) :
      ExitObserver (context),
      _state (*(new State)) {;}


//! Destructor.
dmz::ExitObserverBasic::~ExitObserverBasic () { delete &_state; }


/*!

\brief Query if exit has been requested.
\return Returns dmz::True if exit has been requested.

*/
dmz::Boolean
dmz::ExitObserverBasic::is_exit_requested () { return _state.exitRequested; }


//! Gets exit status.
dmz::ExitStatusEnum
dmz::ExitObserverBasic::get_status () { return _state.status; }


//! Gets exit reason.
dmz::String
dmz::ExitObserverBasic::get_reason () { return _state.reason; }


//! Internal callback.
void
dmz::ExitObserverBasic::exit_requested (
      const ExitStatusEnum Status,
      const String &ExitReason) {

   _state.exitRequested = True;
   _state.status = Status;
   _state.reason = ExitReason;
}


