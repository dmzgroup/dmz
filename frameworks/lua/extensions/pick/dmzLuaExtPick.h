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
         LuaExtPick (const PluginInfo &Info, Config &local);
         ~LuaExtPick ();

         // Plugin Interface
         virtual void discover_plugin (const Plugin *PluginPtr);
         virtual void start_plugin ();
         virtual void stop_plugin ();
         virtual void shutdown_plugin ();
         virtual void remove_plugin (const Plugin *PluginPtr);

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

      private:
         LuaExtPick ();
         LuaExtPick (const LuaExtPick &);
         LuaExtPick &operator= (const LuaExtPick &);
   };
};


#endif // DMZ_LUA_EXT_PICK_DOT_H
