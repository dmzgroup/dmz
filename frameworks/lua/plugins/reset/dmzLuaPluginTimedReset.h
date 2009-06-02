#ifndef DMZ_LUA_PLUGIN_TIMED_RESET_DOT_H
#define DMZ_LUA_PLUGIN_TIMED_RESET_DOT_H

#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimeTimeSlice.h>

namespace dmz {

   class LuaModule;

   class LuaPluginTimedReset :
         public Plugin,
         public TimeSlice {

      public:
         //! \cond
         LuaPluginTimedReset (const PluginInfo &Info, Config &local);
         ~LuaPluginTimedReset ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level) {;}

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // TimeSlice Interface
         void update_time_slice (const Float64 TimeDelta);

      protected:
         void _init (Config &local);

         LuaModule *_luaMod;

         Log _log;
         //! \endcond

      private:
         LuaPluginTimedReset ();
         LuaPluginTimedReset (const LuaPluginTimedReset &);
         LuaPluginTimedReset &operator= (const LuaPluginTimedReset &);

   };
};

#endif // DMZ_LUA_PLUGIN_TIMED_RESET_DOT_H
