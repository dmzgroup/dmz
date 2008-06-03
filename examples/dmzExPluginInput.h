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
         virtual void discover_plugin (const Plugin *PluginPtr);
         virtual void start_plugin ();
         virtual void stop_plugin ();
         virtual void shutdown_plugin ();
         virtual void remove_plugin (const Plugin *PluginPtr);

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
