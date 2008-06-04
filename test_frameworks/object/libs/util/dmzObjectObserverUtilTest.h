#ifndef DMZ_OBJECT_OBSERVER_UTIL_TEST_DOT_H
#define DMZ_OBJECT_OBSERVER_UTIL_TEST_DOT_H

#include <dmzObjectObserverUtil.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimeObjectType.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimeSync.h>
#include <dmzTestPluginUtil.h>

namespace dmz {

   class Config;
   class ObjectModule;

   class ObjectObserverUtilTest :
      public Plugin,
      public Sync,
      protected ObjectObserverUtil {

      public:
         ObjectObserverUtilTest (
            const PluginInfo &Info,
            Config &local,
            Config &global);
         ~ObjectObserverUtilTest ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level) {;}

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr) {;}

         // Sync Interface
         void update_sync (const Float64 TimeDelta);

         // Object Observer Interface
         void store_object_position (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Vector &Position,
            const Vector *LastPostion);

      protected:
         TestPluginUtil test;
         ObjectType _type;
         ObjectModule *_objMod;
         Log _log;
         Handle _defaultHandle;
   };
};

#endif // DMZ_OBJECT_OBSERVER_UTIL_TEST_DOT_H
