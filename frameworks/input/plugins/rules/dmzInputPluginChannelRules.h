#ifndef DMZ_INPUT_PLUGIN_CHANNEL_RULES_DOT_H
#define DMZ_INPUT_PLUGIN_CHANNEL_RULES_DOT_H

#include <dmzInputObserverUtil.h>
#include <dmzRuntimePlugin.h>
#include <dmzTypesHashTableStringTemplate.h>
#include <dmzTypesHashTableUInt32Template.h>

namespace dmz {

//! \cond
   class InputPluginChannelRules : public Plugin, public InputObserverUtil {

      public:
         InputPluginChannelRules (const PluginInfo &Info);
         ~InputPluginChannelRules ();

         virtual void discovery_plugin (const Plugin *PluginPtr);
         virtual void start_plugin ();
         virtual void sync_plugin (const Float64 DeltaTime);
         virtual void stop_plugin ();
         virtual void shutdown_plugin ();
         virtual void remove_plugin (const Plugin *PluginPtr);

         virtual void store_input_module_channels (
            const String &Name,
            InputModule &module);

         virtual void remove_input_module_channels (
            const String &Name,
            InputModule &module);

         virtual void update_channel_state (const UInt32 Channel, const Boolean State);

         // No need for these functions so just stub them out
         virtual void receive_axis_event (
            const UInt32 Channel,
            const InputEventAxis &Value) {;}

         virtual void receive_button_event (
            const UInt32 Channel,
            const InputEventButton &Value) {;}

         virtual void receive_key_event (
            const UInt32 Channel,
            const InputEventKey &Value) {;}

         virtual void receive_mouse_event (
            const UInt32 Channel,
            const InputEventMouse &Value) {;}

         virtual void receive_switch_event (
            const UInt32 Channel,
            const InputEventSwitch &Value) {;}

      protected:
         struct ChannelStruct {

            const String Name;
            Uint32 handle;

            ChannelStuct (const String &TheName) : Name (TheName), handle (0) {;}
         };

         struct ChannelGroupStruct {

         };

         HashTableStringTemplate<ChannelStruct> _nameTable;
         HashTableUInt32Template<ChannelStruct> _handleTable;
//! \endcond

      private:
         InputPluginChannelRules (const InputChannelRules &);
         InputPluginChannelRules &operator= (const InputChannelRules &);
   };
};

#endif // DMZ_INPUT_PLUGIN_CHANNEL_RULES_DOT_H
