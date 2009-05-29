#ifndef DMZ_PLUGIN_EXIT_DOT_H
#define DMZ_PLUGIN_EXIT_DOT_H

#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimeTimeSlice.h>

namespace dmz {

   class PluginExit :
         public Plugin,
         public TimeSlice {

      public:
         PluginExit (const PluginInfo &Info, Config &local);
         ~PluginExit ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level);

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // TimeSlice Interface
         virtual void update_time_slice (const Float64 TimeDelta);

      protected:
         void _init (Config &local);

         Log _log;

      private:
         PluginExit ();
         PluginExit (const PluginExit &);
         PluginExit &operator= (const PluginExit &);

   };
};

#endif // DMZ_PLUGIN_EXIT_DOT_H
