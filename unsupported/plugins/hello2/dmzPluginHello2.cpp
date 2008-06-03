#include "dmzPluginHello2.h"
#include <dmzRuntimeConfigRead.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzTypesVector.h>

dmz::PluginHello2::PluginHello2 (const PluginInfo &Info, Config &local) :
      Plugin (Info),
      _point (4.0, 5.0, 6.0),
      _log (Info) {

   _log.debug << "This is a debug message" << endl;
   _log.info << "This is an info message" << endl;
   _log.warn << "This is an warn message" << endl;
   _log.error << "This is an error message" << endl;
   _log.out << "This is an out message" << endl;

   _init (local);
}


dmz::PluginHello2::~PluginHello2 () {


  _log.out << "In destructor" << endl;
}


// Plugin Interface
void
dmz::PluginHello2::discover_plugin (const Plugin *PluginPtr) {

  Definitions defs (get_plugin_runtime_context (), &_log);

  if (PluginPtr) {

      _log.out << "In discovered: " <<
         defs.lookup_runtime_name (PluginPtr->get_plugin_handle ()) << endl;
  }
}


void
dmz::PluginHello2::start_plugin () {

   _log.out << "In start_plugin" << endl;
}


void
dmz::PluginHello2::stop_plugin () {

   _log.out << "In stop_plugin" << endl;
}


void
dmz::PluginHello2::shutdown_plugin () {

   _log.out << "In shutdown_plugin" << endl;
}


void
dmz::PluginHello2::remove_plugin (const Plugin *PluginPtr) {

  Definitions defs (get_plugin_runtime_context (), &_log);

  if (PluginPtr) {

      _log.out << "Removed: " <<
         defs.lookup_runtime_name (PluginPtr->get_plugin_handle ()) << endl;
  }
}


void
dmz::PluginHello2::_init (Config &local) {

   _point = config_to_vector ("point", local, _point);

   _log.out << "Point is: " << _point << endl;

   String text (config_to_string ("message.text", local));

   _log.out << "Got message: " << text << endl;
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzPluginHello2 (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::PluginHello2 (Info, local);
}

};
