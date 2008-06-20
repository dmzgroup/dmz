#ifndef DMZ_EX_PLUGIN_TIME_SLICE_DOT_H
#define DMZ_EX_PLUGIN_TIME_SLICE_DOT_H

#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimeTimeSlice.h>


namespace dmz {

   class ExPluginTimeSlice :
         public Plugin,
         public TimeSlice {

      public:
         ExPluginTimeSlice (const PluginInfo &Info, Config &local);
         ~ExPluginTimeSlice ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level) {;}

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr) {;}

         // TimeSlice Interface
         virtual void update_time_slice (const Float64 DeltaTime);

      protected:
         void _init (Config &local);

         Log _log;

      private:
         ExPluginTimeSlice ();
         ExPluginTimeSlice (const ExPluginTimeSlice &);
         ExPluginTimeSlice &operator= (const ExPluginTimeSlice &);

   };
};

#endif // DMZ_EX_PLUGIN_TIME_SLICE_DOT_H
