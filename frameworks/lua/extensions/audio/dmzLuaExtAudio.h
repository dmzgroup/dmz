#ifndef DMZ_LUA_EXT_AUDIO_DOT_H
#define DMZ_LUA_EXT_AUDIO_DOT_H

#include <dmzLuaExt.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>

namespace dmz {

   class AudioModule;

   class LuaExtAudio :
         public Plugin,
         public LuaExt {

      public:
         //! \cond
         LuaExtAudio (const PluginInfo &Info, Config &local);
         ~LuaExtAudio ();

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

         AudioModule *_audio;
         AudioModule **_audioPtr;
         //! \endcond

      private:
         LuaExtAudio ();
         LuaExtAudio (const LuaExtAudio &);
         LuaExtAudio &operator= (const LuaExtAudio &);

   };
};

#endif // DMZ_LUA_EXT_AUDIO_DOT_H
