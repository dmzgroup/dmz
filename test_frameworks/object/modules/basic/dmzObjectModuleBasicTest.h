#ifndef DMZ_OBJECT_MODULE_BASIC_TEST_DOT_H
#define DMZ_OBJECT_MODULE_BASIC_TEST_DOT_H

#include <dmzRuntimePlugin.h>
#include <dmzRuntimeTimeSlice.h>
#include <dmzTestPluginUtil.h>
#include <dmzObjectObserverUtil.h>

namespace dmz {

   class Config;
   class ObjectModule;

   class ObjectModuleBasicTest :
      public Plugin,
      public TimeSlice,
      protected ObjectObserverUtil {

      public:
         ObjectModuleBasicTest (
            const PluginInfo &Info,
            Config &local,
            Config &global);
         ~ObjectModuleBasicTest ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level) {;}

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr) {;}

         void update_time_slice (const Float64 TimeDelta);

      protected:
         TestPluginUtil test;
         ObjectModule *_objMod;
   };
};

#endif // DMZ_OBJECT_MODULE_BASIC_TEST_DOT_H
