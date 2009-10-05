#include "dmzRuntimeContextMessaging.h"
#include "dmzRuntimeMessageContext.h"

//! Constructor.
dmz::RuntimeContextMessageContainer::RuntimeContextMessageContainer () :
      messageHandleTable (&messageHandleLock),
      messageNameTable (&messageNameLock) {;}


//! Destructor.
dmz::RuntimeContextMessageContainer::~RuntimeContextMessageContainer () {

   messageHandleTable.clear ();
   messageNameTable.empty ();
}


//! Creates message type.
dmz::Message
dmz::RuntimeContextMessageContainer::create_message (
      const String &Name,
      const String &ParentName,
      RuntimeContext *context,
      RuntimeContextMessaging *messagingContext) {

   Message result;

   Message *type (messageNameTable.lookup (Name));

   if (!type && context) {

      MessageContext *parentContext (0);

      if (ParentName) {

         Message *parent (messageNameTable.lookup (ParentName));

         if (parent) { parentContext = parent->get_message_context (); }
      }

      MessageContext *mtc =
         new MessageContext (Name, context, messagingContext, parentContext);

      if (mtc) {

         type = new Message (mtc);

         if (type) {

            if (messageNameTable.store (type->get_name (), type)) {

               messageHandleTable.store (type->get_handle (), type);
            }
            else {

               delete type; type = 0;
               type = messageNameTable.lookup (Name);
            }
         }

         mtc->unref (); mtc = 0;
      }
   }

   if (type) { result = *type; }

   return result;
}


//! Constructor.
dmz::RuntimeContextMessaging::RuntimeContextMessaging (
      RuntimeContextThreadKey &theKey,
      RuntimeContextMessageContainer &container,
      RuntimeContext *context) :
      log (context ? context->get_log_context () : 0),
      head (0),
      tail (0),
      messageCount (0),
      key (theKey),
      obsHandleTable (&obsHandleLock),
      obsNameTable (&obsNameLock),
      monostateErrorTable (&monostateErrorLock) {

   globalType = container.create_message ("Global_Message", "", context, this);
   key.ref ();
   if (log) { log->ref (); }
}


//! Destructor.
dmz::RuntimeContextMessaging::~RuntimeContextMessaging () {

   obsHandleTable.clear ();
   obsNameTable.clear ();
   key.unref ();

   if (log) { log->unref (); }

   listLock.lock ();
   if (head) { delete head; head = tail = 0; }
   listLock.unlock ();
}


inline void
local_send (
      const dmz::UInt32 Count,
      const dmz::Handle ObsHandle,
      const dmz::Data *InData,
      dmz::MessageContext &context,
      dmz::MessageContext &realContext) {

   if (!context.inSend) {

      context.inSend = dmz::True;

      dmz::HashTableHandleIterator it;

      const dmz::Message Type (&realContext);

      for (
            dmz::MessageObserver *obs = context.obsTable.get_first (it);
            obs != 0;
            obs = context.obsTable.get_next (it)) {

         obs->receive_message (Type, Count, ObsHandle, InData, 0);
      }

      context.inSend = dmz::False;
   }
}


void
dmz::RuntimeContextMessaging::send_monostate_warning (const Message &Msg) {

   if (log) {

      MessageContext *context = Msg.get_message_context ();

      if (context &&
            !monostateErrorTable.lookup (context->Handle.get_runtime_handle ())) {

         String out ("Attempting to send Monostate Message: ");
         out << context->Name << " to target. Target ignored.";
         log->write_kernel_message (LogLevelWarn, out);
         monostateErrorTable.store (context->Handle.get_runtime_handle (), context);
      }
   }
}


dmz::UInt32
dmz::RuntimeContextMessaging::send (
      const Boolean IncrementCount,
      const Message &Type,
      const Handle ObserverHandle,
      const Data *InData,
      Data *outData) const {

   RuntimeContextMessaging *self ((RuntimeContextMessaging *)this);

   UInt32 result (0);

   if (self->key.is_main_thread ()) {

      MessageContext *type (Type.get_message_context ());
      MessageContext *startType (type);

      if (type) {

         if (Type != globalType) {

            if (IncrementCount) {

               self->messageCount++;

               if (messageCount <= 1) { self->messageCount = 2; }
            }

            result = messageCount;

            if (ObserverHandle) {

               MessageObserver *obs (0);

               while (!obs && type) {

                  obs = type->obsTable.lookup (ObserverHandle);
                  if (!obs) { type = type->parent; }
               }

               if (obs && type) {

                  obs->receive_message (Type, result, ObserverHandle, InData, outData);
               }
            }
            else {

               while (type) {

                  local_send (result, ObserverHandle, InData, *type, *startType);
                  type = type->parent;
               }
            }
         }

         MessageContext *gcontext (globalType.get_message_context ());

         if (gcontext) {

            local_send (result, ObserverHandle, InData, *gcontext, *startType);
         }
      }
   }
   else if (Type.get_message_context ()) {

      MessageStruct *ms (new MessageStruct (Type, ObserverHandle, InData));

      if (ms) {

         self->listLock.lock ();
         if (tail) { self->tail->next = ms; self->tail = ms; }
         else { self->head = self->tail = ms; }
         self->listLock.unlock ();

         result = 1;
      }
   }

   return result;
}


//! Sends messages from other threads.
void
dmz::RuntimeContextMessaging::update_time_slice () {

   if (head) {

      listLock.lock ();
      MessageStruct *ms = head; head = tail = 0;
      listLock.unlock ();

      while (ms) {

         ms->Type.send (ms->ObserverHandle, ms->DataPtr, 0);
         ms = ms->next;
      }

      delete ms; ms = 0;
   }
}



//! Adds message observer.
dmz::Boolean
dmz::RuntimeContextMessaging::add_observer (MessageObserver &obs) {

   const Handle ObsHandle (obs.get_message_observer_handle ());
   const Boolean Result1 (ObsHandle ? obsHandleTable.store (ObsHandle, &obs) : False);

   const String Name (obs.get_message_observer_name ());
   const Boolean Result2 (Name && ObsHandle ? obsNameTable.store (Name, &obs) : False);

   return Result1 && Result2;
}


//! Removes message observer.
dmz::Boolean
dmz::RuntimeContextMessaging::remove_observer (MessageObserver &obs) {

   const Handle ObsHandle (obs.get_message_observer_handle ());
   const Boolean Result1 (ObsHandle ?
      (&obs == obsHandleTable.remove (ObsHandle)) : False);

   // Prevent the remove of a different obs with same name
   const String Name (obs.get_message_observer_name ());
   const Boolean Result2 (Name && ObsHandle ?
      (&obs == obsNameTable.lookup (Name)) : False);

   if (Result2) { obsNameTable.remove (Name); }

   return Result1 && Result2;
}

