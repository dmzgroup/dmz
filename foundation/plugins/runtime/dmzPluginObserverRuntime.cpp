#include <dmzRuntimeModule.h>
#include "dmzPluginObserverRuntime.h"
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>

dmz::PluginObserverRuntime::PluginObserverRuntime (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      PluginObserver (Info),
      _log (Info) {

   _init (local);
}


dmz::PluginObserverRuntime::~PluginObserverRuntime () {

}


// Plugin Interface
void
dmz::PluginObserverRuntime::update_plugin_state (
      const PluginStateEnum State,
      const UInt32 Level) {

   if (State == PluginStateInit) {

   }
   else if (State == PluginStateStart) {

   }
   else if (State == PluginStateStop) {

   }
   else if (State == PluginStateShutdown) {

   }
}


void
dmz::PluginObserverRuntime::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

   }
   else if (Mode == PluginDiscoverRemove) {

   }
}


// PluginObserver Interface
void
dmz::PluginObserverRuntime::update_runtime_plugin (
      const PluginDiscoverEnum Mode,
      const Handle RuntimeModuleHandle,
      const Handle PluginHandle) {

   RuntimeModule *rm = lookup_runtime_module (RuntimeModuleHandle);

   if (rm) {

      const PluginInfo *Info = rm->lookup_plugin_info (PluginHandle);

      if (Info) {

         const String ModeStr = (Mode == PluginDiscoverAdd ? "Adding" : "Removing");

         _log.error << ModeStr << " " << Info->get_name () << " "
            << Info->get_handle () << " " << Info->get_class_name () << endl;
      }
      else {

      }
   }
   else {

   }
}


// PluginObserverRuntime Interface
void
dmz::PluginObserverRuntime::_init (Config &local) {

}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzPluginObserverRuntime (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::PluginObserverRuntime (Info, local);
}

};
