#include <dmzObjectAttributeMasks.h>
#include <dmzObjectModule.h>
#include <dmzRenderIsect.h>
#include <dmzRenderModuleIsect.h>
#include <dmzRuntimeConfigToBase.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzTypesMatrix.h>
#include <dmzTypesVector.h>
#include "dmzWeaponPluginTrackingMissile.h"

dmz::WeaponPluginTrackingMissile::WeaponPluginTrackingMissile (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      TimeSlice (Info),
      ObjectObserverUtil (Info, local),
      _log (Info),
      _defaultSpeed (40.0),
      _defaultHandle (0),
      _isectMod (0) {

   _init (local);
}


dmz::WeaponPluginTrackingMissile::~WeaponPluginTrackingMissile () {

   _objectTable.clear ();
   _speedTable.empty ();
}


// Plugin Interface
void
dmz::WeaponPluginTrackingMissile::update_plugin_state (
      const PluginStateEnum State,
      const UInt32 Level) {

   if (State == PluginStateInit) {

   }
   else if (State == PluginStateStart) {

   }
   else if (State == PluginStateStop) {

   }
   else if (State == PluginStateShutdown) {

   }
}


void
dmz::WeaponPluginTrackingMissile::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      if (!_isectMod) { _isectMod = RenderModuleIsect::cast (PluginPtr); }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_isectMod && (_isectMod == RenderModuleIsect::cast (PluginPtr))) {

         _isectMod = 0;
      }
   }
}


// TimeSlice Interface
void
dmz::WeaponPluginTrackingMissile::update_time_slice (const Float64 TimeDelta) {

   ObjectModule *objMod (get_object_module ());

   if (objMod && _isectMod) {

      HashTableHandleIterator it;

      IsectParameters params;

      Float64 *speedPtr (_objectTable.get_first (it));

      while (speedPtr) {

         Handle obj (it.get_hash_key ());

         Vector pos;
         Matrix ori;
         objMod->lookup_position (obj, _defaultHandle, pos);
         objMod->lookup_orientation (obj, _defaultHandle, ori);

         Vector vel (0.0, 0.0, -(*speedPtr));
         ori.transform_vector (vel);
         const Vector NewPos (pos + (vel * TimeDelta));

         IsectTestContainer test;
         IsectResultContainer isectResults;
         test.set_test (1, IsectSegmentTest, pos, NewPos);

         _isectMod->disable_isect (obj);
         if (_isectMod->do_isect (params, test, isectResults)) {

_log.error << "BOOM!" << endl;
            objMod->destroy_object (obj);
         }
         else {

            objMod->store_position (obj, _defaultHandle, NewPos);
            objMod->store_velocity (obj, _defaultHandle, vel);
         }
         _isectMod->enable_isect (obj);
         
         speedPtr = _objectTable.get_next (it);
      }
   }
}


// Object Observer Interface
void
dmz::WeaponPluginTrackingMissile::create_object (
      const UUID &Identity,
      const Handle ObjectHandle,
      const ObjectType &Type,
      const ObjectLocalityEnum Locality) {

   if ((Locality == ObjectLocal) && _typeSet.contains_type (Type)) {

      _store_speed (ObjectHandle, Type);
   }
}


void
dmz::WeaponPluginTrackingMissile::destroy_object (
      const UUID &Identity,
      const Handle ObjectHandle) {

   _objectTable.remove (ObjectHandle);
}


void
dmz::WeaponPluginTrackingMissile::_store_speed (
      const Handle ObjectHandle,
      const ObjectType &Type) {

   Float64 *ptr (_speedTable.lookup (Type.get_handle ()));

   if (!ptr) {

      ptr = new Float64 (_defaultSpeed);

      if (ptr && !_speedTable.store (Type.get_handle (), ptr)) {

         delete ptr; ptr = 0;
      }
   }

   if (ptr) { _objectTable.store (ObjectHandle, ptr); }
}


void
dmz::WeaponPluginTrackingMissile::_init (Config &local) {

   RuntimeContext *context (get_plugin_runtime_context ());

   _defaultHandle = activate_default_object_attribute (
      ObjectCreateMask | ObjectDestroyMask);

   _typeSet = config_to_object_type_set ("munitions", local, context);

   _defaultSpeed = config_to_float64 ("speed.value", local, _defaultSpeed);
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzWeaponPluginTrackingMissile (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::WeaponPluginTrackingMissile (Info, local);
}

};
