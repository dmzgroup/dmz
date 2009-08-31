#ifndef DMZ_LUA_EXT_RUNTIME_DOT_H
#define DMZ_LUA_EXT_RUNTIME_DOT_H

#include <dmzLuaExt.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimeModule.h>
#include <dmzRuntimePlugin.h>

namespace dmz {

   class LuaExtRuntime : public Plugin, public LuaExt {

      public:
         //! \cond
         struct RuntimeStruct {

            RuntimeModule *runtime;
            Config global;

            RuntimeStruct () : runtime (0) {;}
         };

         LuaExtRuntime (const PluginInfo &Info, Config &local, Config &global);
         ~LuaExtRuntime ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level);

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // LuaExt Interface
         virtual void store_lua_module (LuaModule &module);
         virtual void open_lua_extension (lua_State *L);
         virtual void close_lua_extension (lua_State *L);
         virtual void remove_lua_module (LuaModule &module);

      protected:
         void _init (Config &local);

         Log _log;

         RuntimeStruct _runtime;
         RuntimeStruct **_runtimePtr;
         //! \endcond

      private:
         LuaExtRuntime ();
         LuaExtRuntime (const LuaExtRuntime &);
         LuaExtRuntime &operator= (const LuaExtRuntime &);

   };
};

#endif // DMZ_LUA_EXT_RUNTIME_DOT_H
