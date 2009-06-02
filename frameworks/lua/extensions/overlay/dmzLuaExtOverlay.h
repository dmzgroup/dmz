#ifndef DMZ_LUA_EXT_OVERLAY_DOT_H
#define DMZ_LUA_EXT_OVERLAY_DOT_H

#include <dmzLuaExt.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>

namespace dmz {

   class RenderModuleOverlay;

   class LuaExtOverlay :
         public Plugin,
         public LuaExt {

      public:
         //! \cond
         LuaExtOverlay (const PluginInfo &Info, Config &local);
         ~LuaExtOverlay ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level);

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

         Log _log;

         lua_State *L;

         RenderModuleOverlay *_overlay;
         RenderModuleOverlay **_overlayPtr;
         //! \endcond

      private:
         LuaExtOverlay ();
         LuaExtOverlay (const LuaExtOverlay &);
         LuaExtOverlay &operator= (const LuaExtOverlay &);

   };
};

#endif // DMZ_LUA_EXT_OVERLAY_DOT_H
