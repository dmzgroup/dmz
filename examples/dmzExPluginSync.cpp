#include "dmzExPluginSync.h"
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>

dmz::ExPluginSync::ExPluginSync (const PluginInfo &Info, Config &local) :
      Plugin (Info),
      Sync (Info, SyncTypeRuntime, SyncModeRepeating, 1.0),
      _log (Info) {

   _init (local);
}


dmz::ExPluginSync::~ExPluginSync () {

}


// Sync Interface
void
dmz::ExPluginSync::update_sync (const Float64 DeltaTime) {

   _log.out << "[sync_timed_event]: " << DeltaTime << endl;
}


void
dmz::ExPluginSync::_init (Config &local) {

}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzExPluginSync (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::ExPluginSync (Info, local);
}

};
