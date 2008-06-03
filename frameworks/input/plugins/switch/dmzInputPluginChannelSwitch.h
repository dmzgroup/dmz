#ifndef DMZ_INPUT_PLUGIN_CHANNEL_SWITCH_DOT_H
#define DMZ_INPUT_PLUGIN_CHANNEL_SWITCH_DOT_H

#include <dmzInputObserverUtil.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>

namespace dmz {

   class InputPluginChannelSwitch : public Plugin, public InputObserverUtil {

      public:
         //! \cond
         InputPluginChannelSwitch (const PluginInfo &Info, Config &local);
         ~InputPluginChannelSwitch ();

         // Plugin Interface
         virtual void discover_plugin (const Plugin *PluginPtr);
         virtual void start_plugin ();
         virtual void stop_plugin ();
         virtual void shutdown_plugin ();
         virtual void remove_plugin (const Plugin *PluginPtr);

         // Input Observer Interface
         virtual void update_channel_state (const UInt32 Channel, const Boolean State);

         virtual void receive_axis_event (
            const UInt32 Channel,
            const InputEventAxis &Value) {;}

         virtual void receive_button_event (
            const UInt32 Channel,
            const InputEventButton &Value);

         virtual void receive_key_event (
            const UInt32 Channel,
            const InputEventKey &Value);

         virtual void receive_mouse_event (
            const UInt32 Channel,
            const InputEventMouse &Value) {;}

         virtual void receive_switch_event (
            const UInt32 Channel,
            const InputEventSwitch &Value) {;}

      protected:
         struct ChannelStruct {

            const Handle Channel;
            ChannelStruct *next;

            ChannelStruct (const Handle TheHandle) :
                  Channel (TheHandle),
                  next (0) {;}

            ~ChannelStruct () { if (next) { delete next; next = 0; } }
         };

         virtual void _store_input_module (InputModule &module);

         void _init (Config &local);

         ChannelStruct *_list;
         ChannelStruct *_current;

         Log _log;
         //! \endcond

      private:
         InputPluginChannelSwitch ();
         InputPluginChannelSwitch (const InputPluginChannelSwitch &);
         InputPluginChannelSwitch &operator= (const InputPluginChannelSwitch &);
   };
};

#endif // DMZ_INPUT_PLUGIN_CHANNEL_SWITCH_DOT_H
