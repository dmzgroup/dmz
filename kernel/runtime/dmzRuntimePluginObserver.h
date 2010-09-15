#ifndef DMZ_RUNTIME_PLUGIN_OBSERVER_DOT_H
#define DMZ_RUNTIME_PLUGIN_OBSERVER_DOT_H

#include <dmzKernelExport.h>
#include <dmzRuntimePlugin.h>
#include <dmzTypesBase.h>
#include <dmzTypesString.h>

namespace dmz {

class RuntimeModule;

class DMZ_KERNEL_LINK_SYMBOL PluginObserver {

   public:
      // PluginObserver Interface
      RuntimeModule *lookup_runtime_module (const Handle RuntimeModuleHandle);

      virtual void update_runtime_plugin (
         const PluginDiscoverEnum Mode,
         const Handle RuntimeModuleHandle,
         const Handle PluginHandle) = 0;

   protected:
      PluginObserver (const PluginInfo &Info);
      ~PluginObserver ();

   private:
      PluginObserver ();
      PluginObserver (const PluginObserver &);
      PluginObserver &operator= (const PluginObserver &);

      struct State;
      State &__state;
};

};

#endif // DMZ_RUNTIME_PLUGIN_OBSERVER_DOT_H
