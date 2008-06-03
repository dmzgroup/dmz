#include "dmzPluginHelloWorld.h"
#include <dmzRuntimePluginFactoryLinkSymbol.h>


dmz::PluginHelloWorld::PluginHelloWorld (const PluginInfo &Info) :
      Plugin (Info),
      Sync (Info),
      _log (Info.get_name (), Info.get_context ()),
      _mod (0) {;}


dmz::PluginHelloWorld::~PluginHelloWorld () {

   _log.out << "Destructor for: " << _PluginInfoData.get_name () << " called." << endl;
}


void
dmz::PluginHelloWorld::discover_plugin (const Plugin *PluginPtr) {

   if (!_mod) {

      _mod = ModuleHelloWorld::cast (PluginPtr);
   }
}


void
dmz::PluginHelloWorld::start_plugin () {

   _log.out << "Plugin: " << _PluginInfoData.get_name () << " is about to start." << endl;
}


void
dmz::PluginHelloWorld::update_sync (const Float64 TimeDelta) {

   if (_mod) { _mod->print_hello (); }

   _log.out << "Plugin: " << _PluginInfoData.get_name ()
      << " is synced with delta time of : " << TimeDelta << endl;
}


void
dmz::PluginHelloWorld::stop_plugin () {

   _log.out << "Plugin: " << _PluginInfoData.get_name () << " is about to stop." << endl;
}


void
dmz::PluginHelloWorld::shutdown_plugin () {

   _log.out << "Plugin: " << _PluginInfoData.get_name () << " is about to shutdown."
      << endl;
}


void
dmz::PluginHelloWorld::remove_plugin (const Plugin *PluginPtr) {

}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzPluginHelloWorld (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::PluginHelloWorld (Info);
}

};

