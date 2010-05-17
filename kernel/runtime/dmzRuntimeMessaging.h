#ifndef DMZ_RUNTIME_MESSAGING_DOT_H
#define DMZ_RUNTIME_MESSAGING_DOT_H

#include <dmzKernelExport.h>
#include <dmzTypesBooleanOperator.h>

namespace dmz {

   class Config;
   class Data;
   class HandleContainer;
   class Log;
   class MessageContext;
   class PluginInfo;
   class RuntimeContext;
   class RuntimeContextMessaging;
   class String;

   enum MessageMonostateEnum {
      MessageMonostateOn, //!< Enables message's monostate.
      MessageMonostateOff //!< Disables message's monostate.
   };

   class DMZ_KERNEL_LINK_SYMBOL Message {

      public:
         Message ();
         Message (const Message &Type);
         Message (MessageContext *context);
         Message (const String &Name, RuntimeContext *context);
         Message (const Handle TypeHandle, RuntimeContext *context);
         ~Message ();

         Message &operator= (const Message &Type);
         Boolean operator== (const Message &Type) const;
         Boolean operator!= (const Message &Type) const;
         Boolean operator! () const;
         DMZ_BOOLEAN_OPERATOR;

         Boolean set_type (const String &Name, RuntimeContext *context);
         Boolean set_type (const Handle TypeHandle, RuntimeContext *context);

         Boolean is_of_type (const Message &Type) const;
         Boolean is_of_exact_type (const Message &Type) const;

         String get_name () const;
         Handle get_handle () const;

         Message get_parent () const;
         Boolean become_parent ();

         void set_monostate_mode (const MessageMonostateEnum Mode) const;
         MessageMonostateEnum get_monostate_mode ();
         const Data *get_monostate () const;

         UInt32 send (
            const Handle TargetObserverHandle,
            const Data *InData,
            Data *outData) const;

         UInt32 send (
            const Handle TargetObserverHandle,
            const Data *InData) const {

            return send (TargetObserverHandle, InData, 0);
         }

         UInt32 send (const Handle TargetObserverHandle) const {

            return send (TargetObserverHandle, 0, 0);
         }

         UInt32 send (
            const HandleContainer &Targets,
            const Data *InData,
            Data *outData) const;

         UInt32 send (
            const HandleContainer &Targets,
            const Data *InData) const { return send (Targets, InData, 0); }

         UInt32 send (const HandleContainer &Targets) const {

            return send (Targets, 0, 0);
         }

         UInt32 send (const Data *InData) const { return send (0, InData, 0); }

         UInt32 send () const { return send (0, 0, 0); }

         void set_message_context (MessageContext *context);
         MessageContext *get_message_context () const;

      protected:
         MessageContext *_context; //!< Internal state.
   };

   class DMZ_KERNEL_LINK_SYMBOL MessageObserver {

      public:
         Handle get_message_observer_handle () const;
         String get_message_observer_name () const;

         Boolean subscribe_to_message (const Message &Type);
         Boolean unsubscribe_to_message (const Message &Type);
         Boolean unsubscribe_to_all_messages ();

         virtual void receive_message (
            const Message &Type,
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

   DMZ_KERNEL_LINK_SYMBOL Message config_to_message (
      const String &Name,
      const Config &Source,
      RuntimeContext *context,
      Log *log = 0);

   DMZ_KERNEL_LINK_SYMBOL Message config_create_message (
      const String &Name,
      const Config &Source,
      const String &DefaultValue,
      RuntimeContext *context,
      Log *log = 0);

   DMZ_KERNEL_LINK_SYMBOL Message config_create_monostate_message (
      const String &Name,
      const Config &Source,
      const String &DefaultValue,
      RuntimeContext *context,
      Log *log = 0);
};

#endif // DMZ_RUNTIME_MESSAGING_DOT_H

