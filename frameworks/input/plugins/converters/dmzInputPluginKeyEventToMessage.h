#ifndef DMZ_INPUT_PLUGIN_KEY_EVENT_TO_MESSAGE_DOT_H
#define DMZ_INPUT_PLUGIN_KEY_EVENT_TO_MESSAGE_DOT_H

#include <dmzInputObserverUtil.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimeMessaging.h>
#include <dmzRuntimePlugin.h>
#include <dmzTypesHashTableHandleTemplate.h>
#include <dmzTypesHashTableUInt32Template.h>

namespace dmz {

   class InputPluginKeyEventToMessage :
         public Plugin,
         public InputObserverUtil {

      public:
         //! \cond
         InputPluginKeyEventToMessage (const PluginInfo &Info, Config &local);
         ~InputPluginKeyEventToMessage ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level) {;}

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr) {;}

         // Input Observer Interface
         virtual void update_channel_state (const Handle Channel, const Boolean State);

         virtual void receive_key_event (
            const Handle Channel,
            const InputEventKey &Value);

      protected:
         struct TargetStruct {

            const Handle Source;
            const Handle Target;
            const Message Msg;
            const Boolean Pressed;
            TargetStruct *next;

            TargetStruct (
                   const Handle TheSource,
                   const Handle TheTarget,
                   const Message &TheMessage,
                   const Boolean IsPressed) :
                   Source (TheSource),
                   Target (TheTarget),
                   Msg (TheMessage),
                   Pressed (IsPressed),
                   next (0) {;}

            ~TargetStruct () { if (next) { delete next; next = 0; } }
         };

         struct KeyStruct {

            TargetStruct *targets;

            KeyStruct () : targets (0) {;}
            ~KeyStruct () { if (targets) { delete targets; targets = 0; } }
         };

         struct ChannelStruct {

            HashTableUInt32Template<KeyStruct> table;

            ~ChannelStruct () { table.empty (); }
         };

         void _create_targets (Config &config, KeyStruct &ks);
         void _create_key_converters (Config &config, ChannelStruct &cs);
         void _init (Config &local);

         HashTableHandleTemplate<ChannelStruct> _channelTable;
         Log _log;

         //! \endcond
      private:
         InputPluginKeyEventToMessage ();
         InputPluginKeyEventToMessage (const InputPluginKeyEventToMessage &);
         InputPluginKeyEventToMessage &operator= (const InputPluginKeyEventToMessage &);

   };
};

#endif // DMZ_INPUT_PLUGIN_KEY_EVENT_TO_MESSAGE_DOT_H
