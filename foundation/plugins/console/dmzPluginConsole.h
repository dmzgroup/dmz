#ifndef DMZ_PLUGIN_CONSOLE_DOT_H
#define DMZ_PLUGIN_CONSOLE_DOT_H

#include <dmzRuntimeLog.h>
#include <dmzRuntimeMessaging.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimeTimeSlice.h>
#include <dmzTypesHashTableStringTemplate.h>

namespace dmz {

   class PluginConsole :
         public Plugin,
         public TimeSlice {

      public:
         //! \cond
         PluginConsole (const PluginInfo &Info, Config &local);
         ~PluginConsole ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level) {;}

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr) {;}

         virtual void update_time_slice (const Float64 TimeDelta);

      protected:
         void _init (Config &local);

         Log _log;

         HashTableStringTemplate<Message> _msgTable;
         //! \endcond
      private:
         PluginConsole ();
         PluginConsole (const PluginConsole &);
         PluginConsole &operator= (const PluginConsole &);

   };
};

#endif // DMZ_PLUGIN_CONSOLE_DOT_H
