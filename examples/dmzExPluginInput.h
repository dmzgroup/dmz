#ifndef DMZ_EX_PLUGIN_INPUT_DOT_H
#define DMZ_EX_PLUGIN_INPUT_DOT_H

#include <dmzInputObserverUtil.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>

namespace dmz {

   class ExPluginInput :
         public Plugin,
         public InputObserverUtil {

      public:
         ExPluginInput (const PluginInfo &Info, Config &local);
         ~ExPluginInput ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level) {;}

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr) {;}

         // Input Observer Interface
         virtual void update_channel_state (const UInt32 Channel, const Boolean State);

         virtual void receive_axis_event (
            const UInt32 Channel,
            const InputEventAxis &Value);

         virtual void receive_button_event (
            const UInt32 Channel,
            const InputEventButton &Value);

         virtual void receive_key_event (
            const UInt32 Channel,
            const InputEventKey &Value);

      protected:
         void _init (Config &local);

         Log _log;

      private:
         ExPluginInput ();
         ExPluginInput (const ExPluginInput &);
         ExPluginInput &operator= (const ExPluginInput &);

   };
};

#endif // DMZ_EX_PLUGIN_INPUT_DOT_H
