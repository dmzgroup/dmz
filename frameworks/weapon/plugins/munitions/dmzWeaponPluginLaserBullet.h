#ifndef DMZ_WEAPON_PLUGIN_LASER_BULLET_DOT_H
#define DMZ_WEAPON_PLUGIN_LASER_BULLET_DOT_H

#include <dmzObjectObserverUtil.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimeObjectType.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimeTimeSlice.h>
#include <dmzTypesHandleContainer.h>
#include <dmzTypesHashTableHandleTemplate.h>

namespace dmz {
 
   class RenderModuleIsect;

   class WeaponPluginLaserBullet :
         public Plugin,
         public TimeSlice,
         public ObjectObserverUtil {

      public:
         WeaponPluginLaserBullet (const PluginInfo &Info, Config &local);
         ~WeaponPluginLaserBullet ();

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
         virtual void create_object (
            const UUID &Identity,
            const Handle ObjectHandle,
            const ObjectType &Type,
            const ObjectLocalityEnum Locality);

         virtual void destroy_object (const UUID &Identity, const Handle ObjectHandle);

      protected:
         void _store_speed (const Handle ObjectHandle, const ObjectType &Type);
         void _init (Config &local);

         Log _log;
         Float64 _defaultSpeed;
         Handle _defaultHandle;
         ObjectTypeSet _typeSet;
         RenderModuleIsect *_isectMod;
         HashTableHandleTemplate<Float64> _objectTable;
         HashTableHandleTemplate<Float64> _speedTable;

      private:
         WeaponPluginLaserBullet ();
         WeaponPluginLaserBullet (const WeaponPluginLaserBullet &);
         WeaponPluginLaserBullet &operator= (const WeaponPluginLaserBullet &);

   };
};

#endif // DMZ_WEAPON_PLUGIN_LASER_BULLET_DOT_H
