#include <dmzObjectModule.h>
#include "dmzObjectModuleBasicTest.h"
#include <dmzRuntimeConfig.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>


dmz::ObjectModuleBasicTest::ObjectModuleBasicTest (
      const PluginInfo &Info,
      Config &local,
      Config &global) :
      Plugin (Info),
      Sync (Info),
      ObjectObserverUtil (Info, local),
      test (Info.get_name (), Info.get_context ()),
      _objMod (0) {

}


dmz::ObjectModuleBasicTest::~ObjectModuleBasicTest () {;}


// Plugin Interface
void
dmz::ObjectModuleBasicTest::discover_plugin (const Plugin *PluginPtr) {

   if (!_objMod) { _objMod = ObjectModule::cast (PluginPtr); }
}


void
dmz::ObjectModuleBasicTest::start_plugin () {;}


void
dmz::ObjectModuleBasicTest::update_sync (const Float64 TimeDelta) {

   test.exit ("Test completed");
}


void
dmz::ObjectModuleBasicTest::stop_plugin () {;}


void
dmz::ObjectModuleBasicTest::shutdown_plugin () {;}


void
dmz::ObjectModuleBasicTest::remove_plugin (const Plugin *PluginPtr) {

   if (_objMod && (_objMod == ObjectModule::cast (PluginPtr))) { _objMod = 0; }
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzObjectModuleBasicTest (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::ObjectModuleBasicTest (Info, local, global);
}

};


