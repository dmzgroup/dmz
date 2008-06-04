#ifndef DMZ_LUA_EXT_INPUT_DOT_H
#define DMZ_LUA_EXT_INPUT_DOT_H

#include <dmzInputObserverUtil.h>
#include <dmzLuaExt.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimeHandle.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzTypesHashTableHandleTemplate.h>


namespace dmz {

   class LuaExtInput;


   class InputObserverLua {

      public:
         InputObserverLua (LuaExtInput &input);
         ~InputObserverLua ();

         Handle get_handle () const;

         int get_index () const;

         Boolean init_input_channels (
            const Config &Data,
            const Mask &Event,
            const int Index);

         Boolean register_channel (
            const String &ChannelName,
            const Mask &Event,
            const int Index);

         void release_channel (const String &ChannelName);

         void release_all ();

      protected:
         RuntimeHandle _handle;
         LuaExtInput &_input;
         int _index;
   };


   class LuaExtInput :
         public Plugin,
         public LuaExt,
         public InputObserverUtil {

      public:
         LuaExtInput (const PluginInfo &Info, Config &local);
         ~LuaExtInput ();

         Boolean store_observer (InputObserverLua *obs);

         Boolean remove_observer (const Handle ObsHandle);

         Handle register_observer (
            const String &ChannelName,
            const Mask &EventMask,
            InputObserverLua *obs);

         void release_observer (const String &ChannelName, const Handle ObsHandle);
         void release_observer (const Handle Channel, const Handle ObsHandle);

         void release_observer_all (const Handle ObsHandle);

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level) {;}

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // LuaExt Interface
         virtual void store_lua_module (LuaModule &module);
         virtual void open_lua_extension (lua_State *LuaState);
         virtual void close_lua_extension (lua_State *LuaState);
         virtual void remove_lua_module (LuaModule &module);

         // Input Observer Interface
         void update_channel_state (const Handle Channel, const Boolean State);

         void receive_axis_event (
            const Handle Channel,
            const InputEventAxis &Value);

         void receive_button_event (
            const Handle Channel,
            const InputEventButton &Value);

         void receive_key_event (
            const Handle Channel,
            const InputEventKey &Value);

         void receive_mouse_event (
            const Handle Channel,
            const InputEventMouse &Value);

         void receive_switch_event (
            const Handle Channel,
            const InputEventSwitch &Value);

         void receive_data_event (
            const Handle Channel,
            const Handle Source,
            const Data &Value);

      protected:
         struct ChannelStruct {

            const Handle Channel;
            Boolean active;
            HashTableHandleTemplate<InputObserverLua> stateTable;
            HashTableHandleTemplate<InputObserverLua> axisTable;
            HashTableHandleTemplate<InputObserverLua> buttonTable;
            HashTableHandleTemplate<InputObserverLua> switchTable;
            HashTableHandleTemplate<InputObserverLua> keyTable;
            HashTableHandleTemplate<InputObserverLua> mouseTable;
            HashTableHandleTemplate<InputObserverLua> dataTable;

            ChannelStruct (const Handle TheChannel) :
               Channel (TheChannel),
               active (False) {;}

            ~ChannelStruct () {

               stateTable.clear ();
               axisTable.clear ();
               buttonTable.clear ();
               switchTable.clear ();
               keyTable.clear ();
               mouseTable.clear ();
               dataTable.clear ();
            }

            void remove (const Handle ObsHandle) {

               stateTable.remove (ObsHandle);
               axisTable.remove (ObsHandle);
               buttonTable.remove (ObsHandle);
               switchTable.remove (ObsHandle);
               keyTable.remove (ObsHandle);
               mouseTable.remove (ObsHandle);
               dataTable.remove (ObsHandle);
            }
         };

         void _deactivate_check (ChannelStruct &cs);

         void _init (Config &local);

         lua_State *L;

         LuaExtInput **_input;

         Log _log;
         Definitions _defs;

         HashTableHandleTemplate<InputObserverLua> _observerTable;
         HashTableHandleTemplate<ChannelStruct> _channelTable;

      private:
         LuaExtInput ();
         LuaExtInput (const LuaExtInput &);
         LuaExtInput &operator= (const LuaExtInput &);
   };
};

#endif // DMZ_LUA_EXT_INPUT_DOT_H
