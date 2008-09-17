#ifndef DMZ_WEAPON_PLUGIN_TRACKING_MISSILE_DOT_H
#define DMZ_WEAPON_PLUGIN_TRACKING_MISSILE_DOT_H

#include <dmzObjectObserverUtil.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimeObjectType.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimeTimeSlice.h>
#include <dmzTypesHandleContainer.h>
#include <dmzTypesHashTableHandleTemplate.h>

namespace dmz {
 
   class RenderModuleIsect;

// MOVE once plugin is finished.
//! \cond
   class WeaponPluginTrackingMissile :
         public Plugin,
         public TimeSlice,
         public ObjectObserverUtil {

      public:
         WeaponPluginTrackingMissile (const PluginInfo &Info, Config &local);
         ~WeaponPluginTrackingMissile ();

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
         //! \endcond

      private:
         WeaponPluginTrackingMissile ();
         WeaponPluginTrackingMissile (const WeaponPluginTrackingMissile &);
         WeaponPluginTrackingMissile &operator= (const WeaponPluginTrackingMissile &);

   };
};

#endif // DMZ_WEAPON_PLUGIN_TRACKING_MISSILE_DOT_H
