#include "dmzRuntimeContextLog.h"

struct dmz::RuntimeContextLog::EventStruct {

   EventStruct *next;

   const String LogName;
   const LogLevelEnum Level;
   const String Message;

   EventStruct (
      const String &TheLogName,
      const LogLevelEnum TheLevel,
      const String &TheMessage) :
      next (0),
      LogName (TheLogName),
      Level (TheLevel),
      Message (TheMessage) {;}

   ~EventStruct () { if (next) { delete next; next = 0; } }
};


//! Constructor
dmz::RuntimeContextLog::RuntimeContextLog (RuntimeContextThreadKey *key) :
      _mainBuffer (""),
      _threadKey (key),
      _eventsToProcess (False),
      _events (0),
      _eventsTail (0) {

   if (_threadKey) { _threadKey->ref (); }
}


//! Destructor.
dmz::RuntimeContextLog::~RuntimeContextLog () {

   if (_threadKey) { _threadKey->unref (); _threadKey = 0; }
   if (_events) { delete _events; _events = _eventsTail = 0; }
}


//! Syncs log messages created in other threads.
void
dmz::RuntimeContextLog::sync () {

   if (_eventsToProcess) {

      EventStruct *head (0);

      _mutex.lock ();
         head = _events;
         _events = _eventsTail = 0;
         _eventsToProcess = False;
      _mutex.unlock ();

      if (head) {

         EventStruct *current = head;

         while (current) {

            current = current->next;
         }

         delete head; head = 0;
      }
   }
}


//! Attaches log observer to log message stream.
void
dmz::RuntimeContextLog::attach_log_observer (LogObserver &obs) {

   _mutex.lock ();
   _obsTable.store (obs.get_log_observer_handle (), &obs);
   _mutex.unlock ();
}


//! Detaches log observer from log message stream.
void
dmz::RuntimeContextLog::detach_log_observer (LogObserver &obs) {

   _mutex.lock ();
   _obsTable.remove (obs.get_log_observer_handle ());
   _mutex.unlock ();
}


//! Gets thread specific log buffer.
dmz::String *
dmz::RuntimeContextLog::get_buffer () {

   String *buffer (0);

   if (_threadKey && _threadKey->is_main_thread()) { buffer = &_mainBuffer; }
   else if (!_threadKey) { buffer = &_mainBuffer; }
   else {

      LogBuffer *lb (_bufferTable.get_data ());

      if (!lb) { lb = new LogBuffer (_bufferTable); _bufferTable.set_data (lb); }
      if (lb) { buffer = &(lb->buffer); }
   }

   return buffer;
}


//! Writes log message to observers.
void
dmz::RuntimeContextLog::write_message (
         const String &LogName,
         const LogLevelEnum Level,
         const String &Message) {

   const Boolean MainThread (_threadKey ? _threadKey->is_main_thread () : True);

   if (MainThread) {

      HashTableHandleIterator it;

      LogObserver *obs = _obsTable.get_first (it);

      while (obs) {

         obs->store_log_message (LogName, Level, Message);

         obs = _obsTable.get_next (it);
      }
   }
   else {

      EventStruct *event = new EventStruct (LogName, Level, Message);
      _mutex.lock ();
         if (_eventsTail) { _eventsTail->next = event; _eventsTail = event; }
         else { _events = _eventsTail = event; }
      _mutex.unlock ();
   }
}

