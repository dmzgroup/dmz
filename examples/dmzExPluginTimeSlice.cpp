#include "dmzExPluginTimeSlice.h"
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>

dmz::ExPluginTimeSlice::ExPluginTimeSlice (const PluginInfo &Info, Config &local) :
      Plugin (Info),
      TimeSlice (Info, TimeSliceTypeRuntime, TimeSliceModeRepeating, 1.0),
      _log (Info) {

   _init (local);
}


dmz::ExPluginTimeSlice::~ExPluginTimeSlice () {

}


// TimeSlice Interface
void
dmz::ExPluginTimeSlice::update_time_slice (const Float64 DeltaTime) {

   _log.out << "[sync_timed_event]: " << DeltaTime << endl;
}


void
dmz::ExPluginTimeSlice::_init (Config &local) {

}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzExPluginTimeSlice (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::ExPluginTimeSlice (Info, local);
}

};
