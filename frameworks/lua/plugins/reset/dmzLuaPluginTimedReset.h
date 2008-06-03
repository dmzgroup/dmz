#ifndef DMZ_LUA_PLUGIN_TIMED_RESET_DOT_H
#define DMZ_LUA_PLUGIN_TIMED_RESET_DOT_H

#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimeSync.h>

namespace dmz {

   class LuaModule;

   class LuaPluginTimedReset :
         public Plugin,
         public Sync {

      public:
         LuaPluginTimedReset (const PluginInfo &Info, Config &local);
         ~LuaPluginTimedReset ();

         // Plugin Interface
         virtual void discover_plugin (const Plugin *PluginPtr);
         virtual void start_plugin ();
         virtual void stop_plugin ();
         virtual void shutdown_plugin ();
         virtual void remove_plugin (const Plugin *PluginPtr);

         // Sync Interface
         void update_sync (const Float64 TimeDelta);

      protected:
         void _init (Config &local);

         LuaModule *_luaMod;

         Log _log;

      private:
         LuaPluginTimedReset ();
         LuaPluginTimedReset (const LuaPluginTimedReset &);
         LuaPluginTimedReset &operator= (const LuaPluginTimedReset &);

   };
};

#endif // DMZ_LUA_PLUGIN_TIMED_RESET_DOT_H
