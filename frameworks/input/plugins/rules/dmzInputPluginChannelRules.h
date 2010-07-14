#ifndef DMZ_INPUT_PLUGIN_CHANNEL_RULES_DOT_H
#define DMZ_INPUT_PLUGIN_CHANNEL_RULES_DOT_H

#include <dmzInputObserverUtil.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>

namespace dmz {

   class InputModule;

   class InputPluginChannelRules :
         public Plugin,
         public InputObserverUtil {

      public:
         InputPluginChannelRules (const PluginInfo &Info, Config &local);
         ~InputPluginChannelRules ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level);

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // Input Observer Interface
         virtual void update_channel_state (const Handle Channel, const Boolean State);

         virtual void receive_axis_event (
            const Handle Channel,
            const InputEventAxis &Value) {;}

         virtual void receive_button_event (
            const Handle Channel,
            const InputEventButton &Value) {;}

         virtual void receive_switch_event (
            const Handle Channel,
            const InputEventSwitch &Value) {;}

         virtual void receive_key_event (
            const Handle Channel,
            const InputEventKey &Value) {;}

         virtual void receive_mouse_event (
            const Handle Channel,
            const InputEventMouse &Value) {;}

         virtual void receive_data_event (
            const Handle Channel,
            const Handle Source,
            const Data &Value) {;}

      protected:
         struct ChannelStruct {

            const Handle Channel;
            ChannelStruct *next;

            ChannelStruct (const Handle TheHandle) :
                  Channel (TheHandle),
                  next (0) {;}

            ~ChannelStruct () { if (next) { delete next; next = 0; } }
         };

         // InputPluginChannelRules Interface
         void _init (Config &local);

         Log _log;
         ChannelStruct *_channelList;
         Handle _defaultChannel;
         InputModule *_inputModule;
         String _inputModuleName;

      private:
         InputPluginChannelRules ();
         InputPluginChannelRules (const InputPluginChannelRules &);
         InputPluginChannelRules &operator= (const InputPluginChannelRules &);
         UInt32 __activeChannelCount;

   };
};

#endif // DMZ_INPUT_PLUGIN_CHANNEL_RULES_DOT_H
