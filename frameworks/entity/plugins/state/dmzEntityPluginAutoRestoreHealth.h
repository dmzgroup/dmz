#ifndef DMZ_ENTITY_PLUGIN_AUTO_RESTORE_HEALTH_DOT_H
#define DMZ_ENTITY_PLUGIN_AUTO_RESTORE_HEALTH_DOT_H

#include <dmzObjectObserverUtil.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimeTimeSlice.h>
#include <dmzTypesHashTableHandleTemplate.h>

namespace dmz {

   class EntityPluginAutoRestoreHealth :
         public Plugin,
         public TimeSlice,
         public ObjectObserverUtil {

      public:
         EntityPluginAutoRestoreHealth (const PluginInfo &Info, Config &local);
         ~EntityPluginAutoRestoreHealth ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level);

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // TimeSlice Interface
         virtual void update_time_slice (const Float64 TimeDelta);

         // Object Observer Interface
         virtual void destroy_object (const UUID &Identity, const Handle ObjectHandle);

         virtual void update_object_scalar (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Float64 Value,
            const Float64 *PreviousValue);

      protected:
         struct HealthStruct {

            const Handle ObjectHandle;
            Float64 timer;

            HealthStruct (const Handle TheObjectHandle) :
                  ObjectHandle (TheObjectHandle),
                  timer (0.0) {;}
         };

         void _init (Config &local);

         Log _log;

         HashTableHandleTemplate<HealthStruct> _objTable;

         Float64 _healRate;
         Float64 _maxHealth;
         Float64 _healthIncrease;

         Handle _healthAttrHandle;

      private:
         EntityPluginAutoRestoreHealth ();
         EntityPluginAutoRestoreHealth (const EntityPluginAutoRestoreHealth &);
         EntityPluginAutoRestoreHealth &operator= (const EntityPluginAutoRestoreHealth &);

   };
};

#endif // DMZ_ENTITY_PLUGIN_AUTO_RESTORE_HEALTH_DOT_H
