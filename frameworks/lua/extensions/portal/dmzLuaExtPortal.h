#ifndef DMZ_LUA_EXT_PORTAL_DOT_H
#define DMZ_LUA_EXT_PORTAL_DOT_H

#include <dmzAudioModulePortal.h>
#include <dmzLuaExt.h>
#include <dmzRenderModulePortal.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>

struct lua_State;

namespace dmz {

   class LuaExtPortal :
         public Plugin,
         public LuaExt {

      public:
         LuaExtPortal (const PluginInfo &Info, Config &local);
         ~LuaExtPortal () {;}

         void set_view (const String &Name, const Vector &Pos, const Matrix &Ori);
         void get_view (const String &Name, Vector &pos, Matrix &ori);

         // Plugin Interface
         virtual void discover_plugin (const Plugin *PluginPtr);
         virtual void start_plugin () {;}
         virtual void stop_plugin () {;}
         virtual void shutdown_plugin () {;}
         virtual void remove_plugin (const Plugin *PluginPtr);

         // LuaExt Interface
         virtual void store_lua_module (LuaModule &module) {;}
         virtual void open_lua_extension (lua_State *LuaState);
         virtual void close_lua_extension (lua_State *LuaState);
         virtual void remove_lua_module (LuaModule &module) {;}

      protected:
         AudioModulePortal *_audioMaster;
         RenderModulePortal *_renderMaster;

         Log _log;

      private:
         LuaExtPortal ();
         LuaExtPortal (const LuaExtPortal &);
         LuaExtPortal &operator= (const LuaExtPortal &);

   };
};

#endif // DMZ_LUA_EXT_PORTAL_DOT_H
