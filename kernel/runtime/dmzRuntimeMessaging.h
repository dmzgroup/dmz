#ifndef DMZ_RUNTIME_MESSAGING_DOT_H
#define DMZ_RUNTIME_MESSAGING_DOT_H

#include <dmzKernelExport.h>
#include <dmzTypesBooleanOperator.h>

namespace dmz {

   class Data;
   class MessageTypeContext;
   class PluginInfo;
   class RuntimeContext;
   class RuntimeContextMessaging;

   class DMZ_KERNEL_LINK_SYMBOL MessageType {

      public:
         MessageType ();
         MessageType (const MessageType &Type);
         MessageType (MessageTypeContext *context);
         MessageType (const String &Name, RuntimeContext *context);
         MessageType (const Handle TypeHandle, RuntimeContext *context);
         ~MessageType ();

         MessageType &operator= (const MessageType &Type);
         Boolean operator== (const MessageType &Type) const;
         Boolean operator!= (const MessageType &Type) const;
         Boolean operator! () const;
         DMZ_BOOLEAN_OPERATOR;

         Boolean set_type (const String &Name, RuntimeContext *context);
         Boolean set_type (const Handle TypeHandle, RuntimeContext *context);

         Boolean is_of_type (const MessageType &Type) const;
         Boolean is_of_exact_type (const MessageType &Type) const;

         String get_name () const;
         Handle get_handle () const;

         Boolean get_parent (MessageType &type) const;
         Boolean become_parent ();

         UInt32 send_message (
            const Handle TargetObserverHandle,
            const Data *InData,
            Data *outData) const;

         UInt32 send_message (const Data *InData) const {

            return send_message (0, InData, 0);
         }

         UInt32 send_message () const { return send_message (0, 0, 0); }

         void set_message_type_context (MessageTypeContext *context);
         MessageTypeContext *get_message_type_context () const;

      protected:
         MessageTypeContext *_context; //!< Internal state.
   };

   class DMZ_KERNEL_LINK_SYMBOL MessageObserver {

      public:
         Handle get_message_observer_handle () const;
         String get_message_observer_name () const;

         Boolean subscribe_to_message (const MessageType &Type);
         Boolean unsubscribe_to_message (const MessageType &Type);
         Boolean unsubscribe_to_all_messages ();

         virtual void receive_message (
            const MessageType &Type,
            const Handle MessageSendHandle,
            const Handle TargetObserverHandle,
            const Data *InData,
            Data *outData) = 0;

      protected:
         MessageObserver (
            const Handle ObserverHandle,
            const String &Name,
            RuntimeContext *context);

         MessageObserver (const PluginInfo &Info);

         virtual ~MessageObserver ();

         struct MessageObserverState;
         MessageObserverState &_msgObsState; //!< Internal state.

      private:
         MessageObserver ();
         MessageObserver (const MessageObserver &);
         MessageObserver &operator= (const MessageObserver &);
   };
};

#endif // DMZ_RUNTIME_MESSAGING_DOT_H

