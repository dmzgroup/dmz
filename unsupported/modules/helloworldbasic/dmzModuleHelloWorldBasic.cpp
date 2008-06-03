#include "dmzModuleHelloWorldBasic.h"
#include <dmzRuntimePluginFactoryLinkSymbol.h>

#include <dmzSystemStreamFile.h>


dmz::ModuleHelloWorldBasic::ModuleHelloWorldBasic (const PluginInfo &Info) :
      Plugin (Info),
      ModuleHelloWorld (Info),
      _log (Info.get_name (), Info.get_context ()) {;}


dmz::ModuleHelloWorldBasic::~ModuleHelloWorldBasic () {

   _log.out << "Destructor called for " << _PluginInfoData.get_name () << endl;
}


void
dmz::ModuleHelloWorldBasic::discover_plugin (const Plugin *PluginPtr) {

   if (PluginPtr) {

      StreamFileOut out;

      out << "discovered plugin: " << PluginPtr->get_plugin_name () << endl;
   }
}


void
dmz::ModuleHelloWorldBasic::start_plugin () {

   _log.out << "Start called for " << _PluginInfoData.get_name () << endl;
}


void
dmz::ModuleHelloWorldBasic::stop_plugin () {

   _log.out << "Stop called for " << _PluginInfoData.get_name () << endl;
}


void
dmz::ModuleHelloWorldBasic::shutdown_plugin () {

   _log.out << "Shutdown called for " << _PluginInfoData.get_name () << endl;
}


void
dmz::ModuleHelloWorldBasic::remove_plugin (const Plugin *PluginPtr) {

   if (PluginPtr) {

      StreamFileOut out;

      out << "removing plugin: " << PluginPtr->get_plugin_name () << endl;
   }
}


void
dmz::ModuleHelloWorldBasic::print_hello () {

   _log.out << "Hello World!" << endl;
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzModuleHelloWorldBasic (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::ModuleHelloWorldBasic (Info);
}

};

