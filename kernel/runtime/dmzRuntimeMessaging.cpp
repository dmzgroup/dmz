#include "dmzRuntimeContext.h"
#include "dmzRuntimeContextMessaging.h"
#include <dmzRuntimeData.h>
#include <dmzRuntimeHandle.h>
#include <dmzRuntimeDefinitions.h>
#include "dmzRuntimeMessageContext.h"
#include <dmzRuntimeMessaging.h>
#include <dmzRuntimePluginInfo.h>


/*!

\class dmz::Message
\ingroup Runtime
\brief Container for message type context.
\details Message types are hierarchical. A message type may have one parent and
multiple children.
\sa dmz::MessageContext

*/


//! Constructor.
dmz::Message::Message () : _context (0) {;}


//! Copy constructor.
dmz::Message::Message (const Message &Type) : _context (0) { *this = Type; }


//! Message type context constructor.
dmz::Message::Message (MessageContext *context) : _context (0) {

   set_message_type_context (context);
}


/*!

\brief Named constructor.
\details This constructor attempts to find a message type context with the given \a Name.
If one is not found, the internal context pointer is set to NULL.
\param[in] Name String containing name of message type context.
\param[in] context Pointer to runtime context.

*/
dmz::Message::Message (const String &Name, RuntimeContext *context) :
      _context (0) {

   set_type (Name, context);
}


/*!

\brief Unique handle constructor.
\details This constructor attempts to find the message type context with the given
\a Handle.  If one is not found, the internal context pointer is set to NULL.
\param[in] TypeHandle Unique message type handle.
\param[in] context Pointer to the runtime context.

*/
dmz::Message::Message (const Handle TypeHandle, RuntimeContext *context) :
      _context (0) {

   set_type (TypeHandle, context);
}


//! Destructor.
dmz::Message::~Message () { if (_context) { _context->unref (); _context = 0; } }


//! Assignment operator.
dmz::Message &
dmz::Message::operator= (const Message &Type) {

   set_message_type_context (Type._context);
   return *this;
}


//! Relational "equal to" operator.
dmz::Boolean
dmz::Message::operator== (const Message &Type) const {

   return Type._context == _context;
}


//! Relational "not equal to" operator.
dmz::Boolean
dmz::Message::operator!= (const Message &Type) const {

   return Type._context != _context;
}


//! Logical not operator.
dmz::Boolean
dmz::Message::operator! () const { return !_context; }


/*!

\brief Sets the message type context to the named message type.
\param[in] Name String containing name of desired message type context.
\param[in] context Pointer to runtime context.
\return Returns dmz::True if the named message type context is found.

*/
dmz::Boolean
dmz::Message::set_type (const String &Name, RuntimeContext *context) {

   Definitions defs (context);

   return defs.lookup_message_type (Name, *this);
}


/*!

\brief Sets the message type context to the type specified by the unique handle.
\param[in] TypeHandle Unique handle of a message type context.
\param[in] context Pointer to the runtime context.
\return Returns dmz::True if the message type context is found.

*/
dmz::Boolean
dmz::Message::set_type (const Handle TypeHandle, RuntimeContext *context) {

   Definitions defs (context);

   return defs.lookup_message_type (TypeHandle, *this);
}


/*!

\brief Test if message type is a related type.
\param[in] Type Message to test against.
\return Returns dmz::True if \a Type is the same or a parent.

*/
dmz::Boolean
dmz::Message::is_of_type (const Message &Type) const {

   Boolean result (False);

   if (Type) {

      Message current (_context);

      while (current && !result) {

         if (Type == current) { result = True; }
         else { current.become_parent (); }
      }
   }

   return result;
}


/*!

\brief Test if messge type is an exact type.
\param[in] Type Message to test against.
\return Returns dmz::True if \a Type is the same type.

*/
dmz::Boolean
dmz::Message::is_of_exact_type (const Message &Type) const {

   return _context && (Type == *this);
}


//! Gets message types name.
dmz::String
dmz::Message::get_name () const {

   String result;

   if (_context) { result = _context->Name; }

   return result;
}


/*!

\brief Gets message type handle.
\return Returns event type's unique handle. Returns zero if the Message is empty.

*/
dmz::Handle
dmz::Message::get_handle () const {

   Handle result (0);

   if (_context) { result = _context->Handle.get_runtime_handle (); }

   return result;
}


/*!

\brief Gets message type's parent.
\return Returns event types parent. The returned Message will be empty if the
message type has no parent.

*/
dmz::Boolean
dmz::Message::get_parent (Message &msg) const {

   if (_context) { msg.set_message_type_context (_context->parent); }

   return msg.get_message_type_context () != 0;
}


/*!

\brief Becomes message type's parent.
\details The Message will be empty if it has no parent.
\return Returns dmz::True if the message type became its parent. Returns dmz::False
if the current message type has no parent.

*/
dmz::Boolean
dmz::Message::become_parent () {

   if (_context) {

      MessageContext *prev (_context);
      _context = prev->parent;
      if (_context) { _context->ref (); }
      prev->unref (); prev = 0;
   }

   return _context != 0;
}


/*!

\brief Sends the message.
\param[in] ObserverHandle Unique handle of message observer to send message. If set
to zero, message will be sent to all subscribers to the message type.
\param[in] InData Pointer to the data object that is sent along with the message. May
be NULL if no data is to be sent.
\param[out] outData Pointer to Data object that will be used by the target
message observer to return results. This parameter may be NULL if no data is to be
returned.
\return Returns a handle associated with the sent message. This handle is not a unique
runtime handle but is instead a running counter that will roll over when max unsigned
integer messages have been sent.
\note The outData should only be used when sending a message to a specific message
observer, otherwise the outData may be over written if more than one message
observer has subscribed to the sent message. 

*/
dmz::UInt32
dmz::Message::send (
      const Handle ObserverHandle,
      const Data *InData,
      Data *outData) const {

   return
      (_context && _context->context) ?
         _context->context->send (*this, ObserverHandle, InData, outData) : 0;
}


//! For internal use.
void
dmz::Message::set_message_type_context (MessageContext *context) {

   if (context != _context) {

      if (_context) { _context->unref (); _context = 0; }
      _context = context;
      if (_context) { _context->ref (); }
   }
}


//! For internal use.
dmz::MessageContext *
dmz::Message::get_message_type_context () const { return _context; }


/*!

\fn dmz::Handle dmz::Messaging::send (const Message &Type, const Data *InData)
\brief Sends message to all subscribed message observers.
\param[in] Type dmz::Message containing message type to send.
\param[in] InData Pointer to Data object containing data to send with the message.
May be NULL if no data is to be sent with the message.
\return Returns a handle associated with the sent message. This handle is not a unique
runtime handle but is instead a running counter that will roll over when max unsigned
integer messages have been sent.

*/

/*!

\class dmz::MessageObserver
\ingroup Runtime
\brief Interface for receiving messages.
\details Derived class may subscribe to and receive messages.

*/

struct dmz::MessageObserver::MessageObserverState {

   const Handle ObsHandle;
   const String Name;
   RuntimeHandle *handlePtr;
   RuntimeContextMessaging *context;
   HashTableHandleTemplate<Message> msgTable;

   MessageObserverState (
         const Handle TheHandle,
         const String &TheName,
         RuntimeContext *theContext) :
         ObsHandle (TheHandle),
         Name (TheName),
         handlePtr (0),
         context (0) {

      if (theContext) {

         context = theContext->get_messaging_context ();

         if (!ObsHandle) {

            handlePtr = new RuntimeHandle (TheName + ".MessageObserver", theContext);
         }
      }

      if (context) { context->ref (); }
   }

   ~MessageObserverState () {

      if (context) { context->unref (); context = 0; }
      if (handlePtr) { delete handlePtr; handlePtr = 0; }
      msgTable.empty ();
   }
};


/*!

\brief Constructor.
\param[in] ObsHandle Unique handle. If set to zero, the observer will allocate its own.
\param[in] Name String containing the name of the observer. If name is empty or not
unique, the unique observer handle can not be found using
dmz::Messaging::lookup_observer_handle.
\param[in] context Pointer to the runtime context.

*/
dmz::MessageObserver::MessageObserver (
      const Handle ObsHandle,
      const String &Name,
      RuntimeContext *context) :
      _msgObsState (*(new MessageObserverState (ObsHandle, Name, context))) {

   if (_msgObsState.context) { _msgObsState.context->add_observer (*this); }
}


/*!

\brief PluginInfo constructor.
\param[in] Info PluginInfo used for setting the observers name, handle, and runtime
context.
\note This constructor is intended for Plugins that wish to receive messages.
The observer will have the same name and runtime handle as the plugin using this
constructor.

*/
dmz::MessageObserver::MessageObserver (const PluginInfo &Info) :
      _msgObsState (*(new MessageObserverState (
         Info.get_handle (),
         Info.get_name (),
         Info.get_context ()))) {

   if (_msgObsState.context) { _msgObsState.context->add_observer (*this); }
}


//! Destructor.
dmz::MessageObserver::~MessageObserver () {

   unsubscribe_to_all_messages ();
   if (_msgObsState.context) { _msgObsState.context->remove_observer (*this); }
   delete &_msgObsState;
}


//! Gets message observer handle.
dmz::Handle
dmz::MessageObserver::get_message_observer_handle () const {

   return _msgObsState.handlePtr ?
      _msgObsState.handlePtr->get_runtime_handle () : _msgObsState.ObsHandle;
}


//! Gets message observer name.
dmz::String
dmz::MessageObserver::get_message_observer_name () const { return _msgObsState.Name; }


/*!

\brief Subscribes to a message type.
\details This function is used to specify which messages types the observer wishes
to receive.
\note Messages subscriptions may only occur in the main thread.
\param[in] Type dmz::Message to subscribe to.
\return Returns dmz::True if the subscription was successful.

*/
dmz::Boolean
dmz::MessageObserver::subscribe_to_message (const Message &Type) {

   Boolean result (False);

   if (_msgObsState.context && !_msgObsState.msgTable.lookup (Type.get_handle ())) { 

      if (_msgObsState.context->key.is_main_thread ()) {

         MessageContext *typeContext (Type.get_message_type_context ());

         const Handle ObsHandle (get_message_observer_handle ());

         if (typeContext && ObsHandle) {

            result = typeContext->obsTable.store (ObsHandle, this);
            Message *ptr = new Message (typeContext);

            if (ptr) {

               if (!_msgObsState.msgTable.store (ptr->get_handle (), ptr)) {

                  delete ptr; ptr = 0;
               }
            }
         }
      }
   }

   return result;
}


/*!

\brief Unsubscribes from a message type.
\note Unsubscribing from messages may only occur in the main thread.
\param[in] Type dmz::Message to unsubscribe to.
\return Returns dmz::True if the unsubscription was successful.

*/
dmz::Boolean
dmz::MessageObserver::unsubscribe_to_message (const Message &Type) {

   Boolean result (False);

   if (_msgObsState.context) { 

      if (_msgObsState.context->key.is_main_thread ()) {

         MessageContext *typeContext (Type.get_message_type_context ());

         const Handle ObsHandle (get_message_observer_handle ());

         if (typeContext && ObsHandle) {

            result = (this == typeContext->obsTable.remove (ObsHandle));
            Message *ptr = _msgObsState.msgTable.remove (Type.get_handle ());
            if (ptr) { delete ptr; ptr = 0; }
         }
      }
   }

   return result;
}


/*!

\brief Unsubscribes to all messages currently subscribed to.
\return Returns dmz::True if all messages were unsubscribed.

*/
dmz::Boolean
dmz::MessageObserver::unsubscribe_to_all_messages () {

   Boolean result (False);

   if (_msgObsState.context) { 

      if (_msgObsState.context->key.is_main_thread ()) {

         result = True;

         HashTableHandleIterator it;

         for (
               Message *current = _msgObsState.msgTable.get_first (it);
               current != 0;
               current = _msgObsState.msgTable.get_next (it)) {

            unsubscribe_to_message (*current);
         }
      }
   }

   _msgObsState.msgTable.empty ();

   return result;
}


/*!

\fn void dmz::MessageObserver::receive_message (
const Message &Type,
const UInt32 MessageSendHandle,
const Handle TargetObserverHandle,
const Data *InData,
Data *outData)
\brief Pure virtual function that is invoked to notify the observer that a message
has been received.
\details Derived class need to implement this function in order to receive messages.
\param[in] Type dmz::Message being sent.
\param[in] MessageSendHandle This handle corresponds to the handle returned to the caller
of dmz::Messaging::send.
\param[in] TargetObserverHandle Handle of observer the message was sent to. Will be zero
if the message was sent to all subscribers.
\param[in] InData Pointer to data object containing data associated with the message. May
be NULL if no data was sent.
\param[out] outData Pointer to data object to be used to return data to the caller. May
be NULL.

*/
