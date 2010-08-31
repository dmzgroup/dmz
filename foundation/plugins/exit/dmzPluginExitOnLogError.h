#ifndef DMZ_PLUGIN_EXIT_ON_LOG_ERROR_DOT_H
#define DMZ_PLUGIN_EXIT_ON_LOG_ERROR_DOT_H

#include <dmzRuntimeExit.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>

namespace dmz {

   class PluginExitOnLogError :
         public Plugin,
         public LogObserver {

      public:
         PluginExitOnLogError (const PluginInfo &Info, Config &local);
         ~PluginExitOnLogError ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level);

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // LogObserver Interface
         virtual void store_log_message (
            const String &LogName,
            const LogLevelEnum Level,
            const String &Message);

      protected:
         // PluginExitOnLogError Interface
         void _init (Config &local);

         Log _log;
         Exit _exit;
         Boolean _called;

      private:
         PluginExitOnLogError ();
         PluginExitOnLogError (const PluginExitOnLogError &);
         PluginExitOnLogError &operator= (const PluginExitOnLogError &);

   };
};

#endif // DMZ_PLUGIN_EXIT_ON_LOG_ERROR_DOT_H
