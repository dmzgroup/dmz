#include "dmzRuntimeContext.h"
#include "dmzRuntimeContextLog.h"
#include <dmzRuntimeHandle.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePluginInfo.h>

#include <stdio.h>

/*!

\file dmzRuntimeLog.h
\brief Contains the interfaces for the various Log classes and utility functions.

\enum dmz::LogLevelEnum
\ingroup Runtime
\brief Log level enumerations.
\details Defined in dmzRuntimeLog.h.

*/

/*!

\ingroup Runtime
\brief Converts a String to a dmz::LogLevelEnum.
\details Defined in dmzRuntimeLog.h.
This function is \b not case sensitive. The following strings are converted:
- "debug" is converted to dmz::LogLevelDebug.
- "info" is converted to dmz::LogLevelInfo.
- "warn" is converted to dmz::LogLevelWarn.
- "error" is converted to dmz::LogLevelError.
- "out" is converted to dmz::LogLevelOut.
If the passed in String can not be matched the function returns dmz::LogLevelDebug.
\param[in] Level String containing the name of the level to convert.
\return Returns the dmz::LogLevelEnum.

*/
dmz::LogLevelEnum
dmz::string_to_log_level (const String &Level) {

   LogLevelEnum result (LogLevelDebug);

   const String LowerLevel = Level.get_lower ();

   if (LowerLevel == "debug") { result = LogLevelDebug; }
   else if (LowerLevel == "info") { result = LogLevelInfo; }
   else if (LowerLevel == "warn") { result = LogLevelWarn; }
   else if (LowerLevel == "error") { result = LogLevelError; }
   else if (LowerLevel == "out") { result = LogLevelOut; }
   else if (LowerLevel == "never") { result = LogLevelNever; }

   return result;
}


/*!

\class dmz::StreamLog
\ingroup Runtime
\brief Log stream.
\details The Log stream is used to create the different levels in the Log class
(i.e. out, debug, info, warn, and error). There is one StreamLog per level.

*/

struct dmz::StreamLog::State {

   const String LogName;
   const LogLevelEnum Level;
   RuntimeContextLog *logContext;

   State (
         const String &TheLogName,
         const LogLevelEnum TheLevel,
         RuntimeContextLog *theLogContext) :
         LogName (TheLogName),
         Level (TheLevel),
         logContext (theLogContext) {

      if (logContext) { logContext->ref (); }
   }

   ~State () { if (logContext) { logContext->unref (); } }
};


/*!

\brief Constructor.
\param[in] LogName String containing name of the log.
\param[in] Level Enumeration specifying the log stream level.
\param[in] context Pointer to runtime context.

*/
dmz::StreamLog::StreamLog (
      const String &LogName,
      const LogLevelEnum Level,
      RuntimeContext *context) :
      _state (*(new State (
         LogName,
         Level,
         context ? context->get_log_context () : 0))) {;}


//! Destructor.
dmz::StreamLog::~StreamLog () { delete &_state; }


dmz::Stream &
dmz::StreamLog::flush () {

   return this->newline ();
}


dmz::Stream &
dmz::StreamLog::newline () {

   if (_state.logContext) {

      String *message = _state.logContext->get_buffer ();

      if (message) {

         _state.logContext->write_message (_state.LogName, _state.Level, *message);
         message->flush ();
      }
   }
   else { fprintf (stderr, "\n"); }

   return *this;
}


dmz::Stream &
dmz::StreamLog::operator<< (const UInt16 Value) {

   String *message (0);
   if (_state.logContext) { message = _state.logContext->get_buffer (); }

   if (message) { *message << Value; }
   else { String out; out << Value; fprintf (stderr, "%s", out.get_buffer ()); }

   return *this;
}


dmz::Stream &
dmz::StreamLog::operator<< (const UInt32 Value) {

   String *message (0);
   if (_state.logContext) { message = _state.logContext->get_buffer (); }

   if (message) { *message << Value; }
   else { String out; out << Value; fprintf (stderr, "%s", out.get_buffer ()); }

   return *this;
}


dmz::Stream &
dmz::StreamLog::operator<< (const UInt64 Value) {

   String *message (0);
   if (_state.logContext) { message = _state.logContext->get_buffer (); }

   if (message) { *message << Value; }
   else { String out; out << Value; fprintf (stderr, "%s", out.get_buffer ()); }

   return *this;
}


dmz::Stream &
dmz::StreamLog::operator<< (const Int16 Value) {

   String *message (0);
   if (_state.logContext) { message = _state.logContext->get_buffer (); }

   if (message) { *message << Value; }
   else { String out; out << Value; fprintf (stderr, "%s", out.get_buffer ()); }

   return *this;
}


dmz::Stream &
dmz::StreamLog::operator<< (const Int32 Value) {

   String *message (0);
   if (_state.logContext) { message = _state.logContext->get_buffer (); }

   if (message) { *message << Value; }
   else { String out; out << Value; fprintf (stderr, "%s", out.get_buffer ()); }

   return *this;
}


dmz::Stream &
dmz::StreamLog::operator<< (const Int64 Value) {

   String *message (0);
   if (_state.logContext) { message = _state.logContext->get_buffer (); }

   if (message) { *message << Value; }
   else { String out; out << Value; fprintf (stderr, "%s", out.get_buffer ()); }

   return *this;
}


dmz::Stream &
dmz::StreamLog::operator<< (const Float32 Value) {

   String *message (0);
   if (_state.logContext) { message = _state.logContext->get_buffer (); }

   if (message) { *message << Value; }
   else { String out; out << Value; fprintf (stderr, "%s", out.get_buffer ()); }

   return *this;
}


dmz::Stream &
dmz::StreamLog::operator<< (const Float64 Value) {

   String *message (0);
   if (_state.logContext) { message = _state.logContext->get_buffer (); }

   if (message) { *message << Value; }
   else { String out; out << Value; fprintf (stderr, "%s", out.get_buffer ()); }

   return *this;
}


dmz::Stream &
dmz::StreamLog::operator<< (const String &Value) {

   String *message (0);
   if (_state.logContext) { message = _state.logContext->get_buffer (); }

   if (message) { *message << Value; }
   else { String out; out << Value; fprintf (stderr, "%s", out.get_buffer ()); }

   return *this;
}


dmz::Stream &
dmz::StreamLog::operator<< (const char Value) {

   String *message (0);
   if (_state.logContext) { message = _state.logContext->get_buffer (); }

   if (message) { *message << Value; }
   else { char buf[2] = { Value, '\0' }; fprintf (stderr, "%s", buf); }

   return *this;
}


dmz::Stream &
dmz::StreamLog::operator<< (const char *Value) {

   String *message (0);
   if (_state.logContext) { message = _state.logContext->get_buffer (); }

   if (message) { *message << Value; }
   else { fprintf (stderr, "%s", Value); }

   return *this;
}


dmz::Stream &
dmz::StreamLog::operator<< (const void *Value) {

   String *message (0);
   if (_state.logContext) { message = _state.logContext->get_buffer (); }

   if (message) { *message << Value; }
   else { fprintf (stderr, "%p", Value); }

   return *this;
}


dmz::Stream &
dmz::StreamLog::operator<< (stream_operator_func func) {

   func (*this);
   return *this;
}


/*!

\class dmz::Log
\ingroup Runtime
\brief Log class.
\details Class provides system logging. A Log is created with a name. The name does
not need to be unique but it should be descriptive enough to know the source.

*/


/*!

\brief Constructor.
\details The Log gets it name from dmz::PluginInfo::get_name and uses its runtime context.
\param[in] Info PluginInfo used to initialize the Log.

*/
dmz::Log::Log (const PluginInfo &Info) :
      out (Info.get_name (), LogLevelOut, Info.get_context ()),
      error (Info.get_name (), LogLevelError, Info.get_context ()),
      warn (Info.get_name (), LogLevelWarn, Info.get_context ()),
      info (Info.get_name (), LogLevelInfo, Info.get_context ()),
      debug (Info.get_name (), LogLevelDebug, Info.get_context ()) {;}


/*!

\brief Constructor.
\param[in] LogName String containing the Log's name.
\param[in] context Pointer to the runtime context.

*/
dmz::Log::Log (const String &LogName, RuntimeContext *context) :
      out (LogName, LogLevelOut, context),
      error (LogName, LogLevelError, context),
      warn (LogName, LogLevelWarn, context),
      info (LogName, LogLevelInfo, context),
      debug (LogName, LogLevelDebug, context) {;}


//! Destructor.
dmz::Log::~Log () {;}

/*!

\class dmz::LogObserver
\ingroup Runtime
\brief Provides interface for receiving log messages.

*/


struct dmz::LogObserver::LogObsState {

   RuntimeContextLog *logContext;
   RuntimeHandle Handle;

   LogObsState (RuntimeContext *context) :
         logContext (context ? context->get_log_context () : 0),
         Handle ("LogObserver", context) {

      if (logContext) { logContext->ref (); }
   }

   ~LogObsState () { if (logContext) { logContext->unref (); logContext = 0; } }
};


/*!

\brief Constructor.
\details The constructor automatically attaches the observer to the log stream so it will
immediately start receiving messages.
\param[in] context Pointer to runtime context.
\sa dmz::LogObserver::attach_log_observer \n dmz::LogObserver::detach_log_observer

*/
dmz::LogObserver::LogObserver (RuntimeContext *context) :
      _logObsState (*(new LogObsState (context))) { attach_log_observer (); }


//! Destructor.
dmz::LogObserver::~LogObserver () {

   detach_log_observer ();
   delete &_logObsState;
}


//! Returns log observer handle.
dmz::Handle
dmz::LogObserver::get_log_observer_handle () {

   return _logObsState.Handle.get_runtime_handle ();
}


//! Attaches log observer to the log stream so that it will receive messages.
void
dmz::LogObserver::attach_log_observer () {

   if (_logObsState.logContext) { _logObsState.logContext->attach_log_observer (*this); }
}


//! Detaches log observer to the log stream so that it will \b not receive messages.
void
dmz::LogObserver::detach_log_observer () {

   if (_logObsState.logContext) { _logObsState.logContext->detach_log_observer (*this); }
}


/*!

\fn dmz::LogObserver::store_log_message (
            const String &LogName,
            const LogLevelEnum Level,
            const String &Message)
\brief Pure virtual function that is invoked every time a log publishes a message.
\details Derived classes need to implement this function in order to receive log messages.

*/
