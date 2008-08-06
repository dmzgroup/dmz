#include "dmzRuntimeContext.h"
#include "dmzRuntimeContextDefinitions.h"
#include "dmzRuntimeContextLog.h"
#include "dmzRuntimeContextMessaging.h"
#include "dmzRuntimeContextTime.h"
#include "dmzRuntimeContextRTTI.h"
#include "dmzRuntimeContextThreadKey.h"
#include "dmzRuntimeContextUndo.h"
#include <dmzRuntimeHandleAllocator.h>
#include <dmzRuntimeMessaging.h>
#include <dmzSystem.h>

#ifdef _WIN32
// This disables the warning regarding using the 'this' pointer as an
// argument in the constructor. The pointer isn't actually used for anything
// other than its value.
#pragma warning (disable : 4355)
#endif // _Win32

//! Constructor.
dmz::RuntimeContext::RuntimeContext () :
      _session ("session"),
      _key (new RuntimeContextThreadKey ((void *)this)),
      _handleAllocator (0),
      _defContext (0),
      _messageContainerContext (0),
      _messagingContext (0),
      _rttiContext (0),
      _timeContext (0),
      _undoContext (0),
      _logContext (0) {

   create_uuid (uuid);
}


//! Destructor.
dmz::RuntimeContext::~RuntimeContext () {

   if (_key) { _key->unref (); _key = 0; }

   _allocLock.lock ();
   if (_handleAllocator) { _handleAllocator->unref (); _handleAllocator = 0; }
   _allocLock.unlock ();

   _defLock.lock ();
   if (_defContext) { _defContext->unref (); _defContext = 0; }
   _defLock.unlock ();

   _msgContainerLock.lock ();
   if (_messageContainerContext) {

      _messageContainerContext->unref (); _messageContainerContext = 0;
   }
   _msgContainerLock.unlock ();

   _msgLock.lock ();
   if (_messagingContext) { _messagingContext->unref (); _messagingContext = 0; }
   _msgLock.unlock ();

   _rttiLock.lock ();
   if (_rttiContext) { _rttiContext->unref (); _rttiContext = 0; }
   _rttiLock.unlock ();

   _timeLock.lock ();
   if (_timeContext) { _timeContext->unref (); _timeContext = 0; }
   _timeLock.unlock ();

   _undoLock.lock ();
   if (_undoContext) { _undoContext->unref (); _undoContext = 0; }
   _undoLock.unlock ();

   _logLock.lock ();
   // Unref log context LAST
   if (_logContext) { _logContext->unref (); _logContext = 0; }
   _logLock.unlock ();
}


//! TimeSlices internal kernel objects.
void
dmz::RuntimeContext::update_time_slice () {

   if (_messagingContext) { _messagingContext->update_time_slice (); }
   if (_logContext) { _logContext->update_time_slice (); }
   // time context should always update_time_slice last
   if (_timeContext) { _timeContext->update_time_slice (); }
}


//! Gets session Config root.
dmz::Config
dmz::RuntimeContext::get_session_config () const { return _session; }


//! Sets session Config root.
void
dmz::RuntimeContext::set_session_config (const Config &Session) {

   _session = Session;
}



//! Gets thread key.
dmz::RuntimeContextThreadKey *
dmz::RuntimeContext::get_thread_key () { return _key; }


//! Gets handle allocator.
dmz::HandleAllocator *
dmz::RuntimeContext::get_handle_allocator () {

   if (!_handleAllocator) {

      _allocLock.lock ();
      if (!_handleAllocator) { _handleAllocator = new HandleAllocator; }
      _allocLock.unlock ();
   }

   return _handleAllocator;
}


//! Gets definition context.
dmz::RuntimeContextDefinitions *
dmz::RuntimeContext::get_definitions_context () {

   if (!_defContext) {

      _defLock.lock ();
      if (!_defContext) { _defContext = new RuntimeContextDefinitions; }
      _defLock.unlock ();
   }

   return _defContext;
}


//! Gets message container context.
dmz::RuntimeContextMessageContainer *
dmz::RuntimeContext::get_message_container_context () {

   if (!_messageContainerContext && _key) {

      _msgContainerLock.lock ();
      if (!_messageContainerContext) {

         _messageContainerContext = new RuntimeContextMessageContainer;
      }
      _msgContainerLock.unlock ();
   }

   return _messageContainerContext;
}


//! Gets messaging context.
dmz::RuntimeContextMessaging *
dmz::RuntimeContext::get_messaging_context () {

   if (!_messagingContext && _key) {

      RuntimeContextMessageContainer *container (get_message_container_context ());

      _msgLock.lock ();
      if (!_messagingContext && container) {

         _messagingContext = new RuntimeContextMessaging (*_key, *container, this);
      }
      _msgLock.unlock ();
   }

   return _messagingContext;
}


//! Gets RTTI context.
dmz::RuntimeContextRTTI *
dmz::RuntimeContext::get_rtti_context () {

   if (!_rttiContext) {

      _rttiLock.lock ();
      if (!_rttiContext) { _rttiContext = new RuntimeContextRTTI (); }
      _rttiLock.unlock ();
   }

   return _rttiContext;
}


//! Gets time context.
dmz::RuntimeContextTime *
dmz::RuntimeContext::get_time_context () {

   if (!_timeContext) {

      _timeLock.lock ();
      if (!_timeContext) { _timeContext = new RuntimeContextTime (); }
      _timeLock.unlock ();
   }

   return _timeContext;
}


//! Gets undo context.
dmz::RuntimeContextUndo *
dmz::RuntimeContext::get_undo_context () {

   if (!_undoContext) {

      _undoLock.lock ();
      if (!_undoContext) { _undoContext = new RuntimeContextUndo (*this); }
      _undoLock.unlock ();
   }

   return _undoContext;
}


//! Gets log context.
dmz::RuntimeContextLog *
dmz::RuntimeContext::get_log_context () {

   if (!_logContext) {

      _logLock.lock ();
      if (!_logContext) { _logContext = new RuntimeContextLog (_key); }
      _logLock.unlock ();
   }

   return _logContext;
}

