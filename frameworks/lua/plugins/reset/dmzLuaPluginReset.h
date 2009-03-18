#ifndef DMZ_LUA_PLUGIN_RESET_DOT_H
#define DMZ_LUA_PLUGIN_RESET_DOT_H

#include <dmzInputObserverUtil.h>
#include <dmzLuaModule.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>

namespace dmz {

   class LuaPluginReset :
         public Plugin,
         public InputObserverUtil {

      public:
         //! \cond
         LuaPluginReset (const PluginInfo &Info, Config &local);
         ~LuaPluginReset ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level) {;}

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // Input Observer Interface
         virtual void update_channel_state (const Handle Channel, const Boolean State) {;}

         virtual void receive_axis_event (
            const Handle Channel,
            const InputEventAxis &Value) {;}

         virtual void receive_button_event (
            const Handle Channel,
            const InputEventButton &Value);

         virtual void receive_switch_event (
            const Handle Channel,
            const InputEventSwitch &Value) {;}

         virtual void receive_key_event (
            const Handle Channel,
            const InputEventKey &Value);

         virtual void receive_mouse_event (
            const Handle Channel,
            const InputEventMouse &Value) {;}

         virtual void receive_data_event (
            const Handle Channel,
            const Handle Source,
            const Data &Value) {;}

      protected:
         void _init (Config &local);

         UInt32 _key;
         LuaModule *_luaMod;

         Log _log;
         //! \endcond

      private:
         LuaPluginReset ();
         LuaPluginReset (const LuaPluginReset &);
         LuaPluginReset &operator= (const LuaPluginReset &);

   };
};

#endif // DMZ_LUA_PLUGIN_RESET_DOT_H
