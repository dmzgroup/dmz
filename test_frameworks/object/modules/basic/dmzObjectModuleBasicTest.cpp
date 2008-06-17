#include <dmzObjectModule.h>
#include "dmzObjectModuleBasicTest.h"
#include <dmzRuntimeConfig.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>


dmz::ObjectModuleBasicTest::ObjectModuleBasicTest (
      const PluginInfo &Info,
      Config &local,
      Config &global) :
      Plugin (Info),
      TimeSlice (Info),
      ObjectObserverUtil (Info, local),
      test (Info.get_name (), Info.get_context ()),
      _objMod (0) {

}


dmz::ObjectModuleBasicTest::~ObjectModuleBasicTest () {;}


// TimeSlice Interface
void
dmz::ObjectModuleBasicTest::update_time_slice (const Float64 TimeDelta) {

   test.exit ("Test completed");
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


