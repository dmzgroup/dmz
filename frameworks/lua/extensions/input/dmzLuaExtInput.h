#ifndef DMZ_LUA_EXT_EVENT_DOT_H
#define DMZ_LUA_EXT_EVENT_DOT_H

#include <dmzLuaExt.h>
#include <dmzInputObserver.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzTypesHandleContainer.h>
#include <dmzTypesHashTableHandleTemplate.h>

struct lua_State;

namespace dmz {

   //! \cond
   void lua_register_input_observer (lua_State *L, InputObserver &obs);
   void lua_release_input_observer (lua_State *L, InputObserver &obs);
   //! \endcond

   class InputModuleCommon;

   class LuaExtInput :
         public Plugin,
         public LuaExt {

      public:
         //! \cond
         struct InputStruct {

            InputModule *inputMod;
            LuaExtInput *ext;
            Handle defaultHandle;

            InputStruct () :
               inputMod (0),
               ext (0),
               defaultHandle (0) {;}
         };

         LuaExtInput (const PluginInfo &Info, Config &local);
         ~LuaExtInput ();

         void register_input_observer (InputObserver &obs);
         void release_input_observer (InputObserver &obs);

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

      protected:
         void _init (Config &local);

         HashTableHandleTemplate<InputObserver> _obsTable;

         lua_State *L;

         InputStruct _input;
         InputStruct **_inputPtr;

         Log _log;
         //! \endcond

      private:
         LuaExtInput ();
         LuaExtInput (const LuaExtInput &);
         LuaExtInput &operator= (const LuaExtInput &);
   };
};

#endif // DMZ_LUA_EXT_EVENT_DOT_H
