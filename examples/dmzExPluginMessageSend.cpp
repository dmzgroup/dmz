#include "dmzExPluginMessageSend.h"
#include <dmzRuntimeConfigRead.h>
#include <dmzRuntimeData.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzSystem.h>

dmz::ExPluginMessageSend::ExPluginMessageSend (const PluginInfo &Info, Config &local) :
      Plugin (Info),
      Sync (Info, SyncTypeRuntime, SyncModeRepeating, 1.0),
      _log (Info),
      _type (),
      _binder (Info.get_context (), &_log),
      _value ("Some Value"),
      _time (0.0) {

   _binder.bind ("value", 0, _value);
   _binder.bind ("time", 0, _time);
   
   _init (local);
}


dmz::ExPluginMessageSend::~ExPluginMessageSend () {

}


// Plugin Interface
void
dmz::ExPluginMessageSend::discover_plugin (const Plugin *PluginPtr) {

}


void
dmz::ExPluginMessageSend::start_plugin () {

}


void
dmz::ExPluginMessageSend::update_sync (const Float64 TimeDelta) {

   Data data;
   _value << ".";
   _time = get_time ();
   
   if (_binder.write_data (data)) {

      _log.out << "[sync_plugin] _type.send (&data)" << endl;
      const UInt32 Count (_type.send (&data));
   }
}


void
dmz::ExPluginMessageSend::stop_plugin () {

}


void
dmz::ExPluginMessageSend::shutdown_plugin () {

}


void
dmz::ExPluginMessageSend::remove_plugin (const Plugin *PluginPtr) {

}


void
dmz::ExPluginMessageSend::_init (Config &local) {

   _type =
      config_to_message_type (
         "message.name",
         local,
         get_plugin_runtime_context (),
         &_log);
   
   _value = config_to_string ("message.value", local, _value);
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzExPluginMessageSend (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::ExPluginMessageSend (Info, local);
}

};
