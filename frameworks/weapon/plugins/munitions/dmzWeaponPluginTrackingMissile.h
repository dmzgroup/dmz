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
   class EventModuleCommon;

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
         struct TypeStruct {

            const Float64 MaxTurn;
            const Float64 MaxSpeed;
            const Float64 Acceleration;

            TypeStruct (
                  const Float64 TheMaxTurn,
                  const Float64 TheMaxSpeed,
                  const Float64 TheAcceleration) :
                  MaxTurn (TheMaxTurn),
                  MaxSpeed (TheMaxSpeed),
                  Acceleration (TheAcceleration) {;}
         };

         struct ObjectStruct {

            const TypeStruct &Info;
            const Handle Object;
            const Handle Source;
            Handle target;

            ObjectStruct (
                  const TypeStruct &TheInfo,
                  const Handle TheObject,
                  const Handle TheSource,
                  const Handle TheTarget) :
                  Info (TheInfo),
                  Object (TheObject),
                  Source (TheSource),
                  target (TheTarget) {;}
         };

         void _register (const Handle ObjectHandle, const ObjectType &Type);
         TypeStruct *_get_type_info (const ObjectType &Type);
         void _init (Config &local);

         Log _log;
         Handle _defaultHandle;
         Handle _targetLockHandle;
         Handle _sourceHandle;
         ObjectTypeSet _ignoreSet;
         ObjectTypeSet _typeSet;
         RenderModuleIsect *_isect;
         EventModuleCommon *_common;
         HashTableHandleTemplate<TypeStruct> _typeMap;
         HashTableHandleTemplate<TypeStruct> _typeTable;
         HashTableHandleTemplate<ObjectStruct> _objectTable;
         //! \endcond

      private:
         WeaponPluginTrackingMissile ();
         WeaponPluginTrackingMissile (const WeaponPluginTrackingMissile &);
         WeaponPluginTrackingMissile &operator= (const WeaponPluginTrackingMissile &);

   };
};

#endif // DMZ_WEAPON_PLUGIN_TRACKING_MISSILE_DOT_H
