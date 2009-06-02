#ifndef DMZ_LUA_EXT_PICK_DOT_H
#define DMZ_LUA_EXT_PICK_DOT_H

#include <dmzLuaExt.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>

namespace dmz {

   class RenderModulePick;

   class LuaExtPick :
         public Plugin,
         public LuaExt {

      public:
         //! \cond
         LuaExtPick (const PluginInfo &Info, Config &local);
         ~LuaExtPick ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level) {;}

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

         RenderModulePick *_pick;
         RenderModulePick **_pickPtr;

         Log _log;
         //! \endcond

      private:
         LuaExtPick ();
         LuaExtPick (const LuaExtPick &);
         LuaExtPick &operator= (const LuaExtPick &);
   };
};


#endif // DMZ_LUA_EXT_PICK_DOT_H
