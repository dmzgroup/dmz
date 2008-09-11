#ifndef DMZ_WEAPON_PLUGIN_GRAVITY_BULLET_DOT_H
#define DMZ_WEAPON_PLUGIN_GRAVITY_BULLET_DOT_H

#include <dmzObjectObserverUtil.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimeObjectType.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimeTimeSlice.h>
#include <dmzTypesHandleContainer.h>
#include <dmzTypesHashTableHandleTemplate.h>

namespace dmz {
 
   class EventModuleCommon;
   class RenderModuleIsect;

   class WeaponPluginGravityBullet :
         public Plugin,
         public TimeSlice,
         public ObjectObserverUtil {

      public:
         WeaponPluginGravityBullet (const PluginInfo &Info, Config &local);
         ~WeaponPluginGravityBullet ();

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
         Float64 _gravity;
         Handle _defaultHandle;
         ObjectTypeSet _typeSet;
         EventModuleCommon *_eventMod;
         RenderModuleIsect *_isectMod;
         HashTableHandleTemplate<Float64> _objectTable;
         HashTableHandleTemplate<Float64> _speedTable;

      private:
         WeaponPluginGravityBullet ();
         WeaponPluginGravityBullet (const WeaponPluginGravityBullet &);
         WeaponPluginGravityBullet &operator= (const WeaponPluginGravityBullet &);

   };
};

#endif // DMZ_WEAPON_PLUGIN_GRAVITY_BULLET_DOT_H
