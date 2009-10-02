#include "dmzPluginSaveMessageMonostate.h"
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>


dmz::PluginSaveMessageMonostate::PluginSaveMessageMonostate (const PluginInfo &Info, Config &local) :
      Plugin (Info),
      _log (Info) {

   _init (local);
}


dmz::PluginSaveMessageMonostate::~PluginSaveMessageMonostate () {

}


// Plugin Interface
void
dmz::PluginSaveMessageMonostate::update_plugin_state (
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
dmz::PluginSaveMessageMonostate::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

   }
   else if (Mode == PluginDiscoverRemove) {

   }
}


void
dmz::PluginSaveMessageMonostate::_init (Config &local) {

}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzPluginSaveMessageMonostate (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::PluginSaveMessageMonostate (Info, local);
}

};
