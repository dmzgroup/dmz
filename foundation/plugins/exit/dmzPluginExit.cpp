#include "dmzPluginExit.h"
#include <dmzRuntimeExit.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>


dmz::PluginExit::PluginExit (const PluginInfo &Info, Config &local) :
      Plugin (Info),
      TimeSlice (Info),
      _log (Info) {

   _init (local);
}


dmz::PluginExit::~PluginExit () {

}


// Plugin Interface
void
dmz::PluginExit::update_plugin_state (
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
dmz::PluginExit::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

   }
   else if (Mode == PluginDiscoverRemove) {

   }
}


// TimeSlice Interface
void
dmz::PluginExit::update_time_slice (const Float64 TimeDelta) {

   Exit (get_plugin_runtime_context ()).request_exit (ExitStatusNormal, "Quick Exit");
}


void
dmz::PluginExit::_init (Config &local) {

}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzPluginExit (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::PluginExit (Info, local);
}

};
