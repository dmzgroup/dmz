#ifndef DMZ_PLUGIN_OBSERVER_RUNTIME_DOT_H
#define DMZ_PLUGIN_OBSERVER_RUNTIME_DOT_H

#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimePluginObserver.h>

namespace dmz {

   class PluginObserverRuntime :
         public Plugin,
         public PluginObserver {

      public:
         PluginObserverRuntime (const PluginInfo &Info, Config &local);
         ~PluginObserverRuntime ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level);

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // PluginObserver Interface
         virtual void update_runtime_plugin (
            const PluginDiscoverEnum Mode,
            const Handle RuntimeModuleHandle,
            const Handle PluginHandle);

      protected:
         // PluginObserverRuntime Interface
         void _init (Config &local);

         Log _log;

      private:
         PluginObserverRuntime ();
         PluginObserverRuntime (const PluginObserverRuntime &);
         PluginObserverRuntime &operator= (const PluginObserverRuntime &);

   };
};

#endif // DMZ_PLUGIN_OBSERVER_RUNTIME_DOT_H
