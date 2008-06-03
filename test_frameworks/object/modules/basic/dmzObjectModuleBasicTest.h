#ifndef DMZ_OBJECT_MODULE_BASIC_TEST_DOT_H
#define DMZ_OBJECT_MODULE_BASIC_TEST_DOT_H

#include <dmzRuntimePlugin.h>
#include <dmzRuntimeSync.h>
#include <dmzTestPluginUtil.h>
#include <dmzObjectObserverUtil.h>

namespace dmz {

   class Config;
   class ObjectModule;

   class ObjectModuleBasicTest :
      public Plugin,
      public Sync,
      protected ObjectObserverUtil {

      public:
         ObjectModuleBasicTest (
            const PluginInfo &Info,
            Config &local,
            Config &global);
         ~ObjectModuleBasicTest ();

         // Plugin Interface
         void discover_plugin (const Plugin *PluginPtr);
         void start_plugin ();
         void stop_plugin ();
         void shutdown_plugin ();
         void remove_plugin (const Plugin *PluginPtr);

         void update_sync (const Float64 TimeDelta);

      protected:
         TestPluginUtil test;
         ObjectModule *_objMod;
   };
};

#endif // DMZ_OBJECT_MODULE_BASIC_TEST_DOT_H
