#include <dmzEventConsts.h>
#include <dmzEventModuleCommon.h>
#include <dmzObjectAttributeMasks.h>
#include <dmzObjectModule.h>
#include <dmzRenderIsect.h>
#include <dmzRenderModuleIsect.h>
#include <dmzRuntimeConfigToNamedHandle.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzTypesHandleContainer.h>
#include <dmzTypesMath.h>
#include <dmzTypesMatrix.h>
#include <dmzTypesVector.h>
#include <dmzWeaponConsts.h>
#include "dmzWeaponPluginTrackingMissile.h"

#include <math.h>

//! \cond
dmz::WeaponPluginTrackingMissile::WeaponPluginTrackingMissile (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      TimeSlice (Info),
      ObjectObserverUtil (Info, local),
      _log (Info),
      _defaultHandle (0),
      _sourceHandle (0),
      _targetLockHandle (0),
      _isect (0),
      _common (0) {

   _init (local);
}


dmz::WeaponPluginTrackingMissile::~WeaponPluginTrackingMissile () {

   _objectTable.empty ();
   _typeMap.clear ();
   _typeTable.empty ();
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

      if (!_isect) { _isect = RenderModuleIsect::cast (PluginPtr); }
      if (!_common) { _common = EventModuleCommon::cast (PluginPtr); }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_isect && (_isect == RenderModuleIsect::cast (PluginPtr))) { _isect = 0; }
      if (_common && (_common == EventModuleCommon::cast (PluginPtr))) { _common = 0; }
   }
}


// TimeSlice Interface
void
dmz::WeaponPluginTrackingMissile::update_time_slice (const Float64 TimeDelta) {

   ObjectModule *module (get_object_module ());

   if (module && _isect) {

      IsectParameters params;
      params.set_test_result_type (IsectClosestPoint);

      HashTableHandleIterator it;
      ObjectStruct *obj (0);

      while (_objectTable.get_next (it, obj)) {

         _isect->disable_isect (obj->Object);
         Vector pos, vel, dir (0, 0, -1);
         Matrix ori;
         module->lookup_position (obj->Object, _defaultHandle, pos);
         module->lookup_orientation (obj->Object, _defaultHandle, ori);
         module->lookup_velocity (obj->Object, _defaultHandle, vel);
         ori.transform_vector (dir);

         if (!obj->target) {

            IsectTestContainer test;
            IsectResultContainer isectResults;
            test.set_test (1, IsectSegmentTest, pos, pos + (dir * 100.0));

            if (_isect->do_isect (params, test, isectResults)) {

               IsectResult value;
               isectResults.get_first (value);
               value.get_object_handle (obj->target);
            }
         }

         if (obj->target) {

            Vector targetPos;

            if (module->lookup_position (obj->target, _defaultHandle, targetPos)) {

               if ((pos - targetPos).magnitude () < 2.0) {

                  if (_common) {

                     _common->create_detonation_event (obj->Object, obj->target);
                  }

                  module->destroy_object (obj->Object);
                  obj = 0;
               }
               else {

                  const Vector TargetDir = (targetPos - pos).normalize ();

                  const Float64 Rot = dir.get_angle (TargetDir);

                  if (Rot > HalfPi64) { obj->target = 0; }
                  else {

                     const Float64 MaxTurn = obj->Info.MaxTurn * TimeDelta;

                     if (fabs (Rot) > MaxTurn) {

                        const Vector Axis = dir.cross (TargetDir);

                        ori = Matrix (Axis, Rot > 0.0 ? MaxTurn : -MaxTurn) * ori;
                     }
                  }
               }
            }
         }

         if (obj) {

            Float64 speed = vel.magnitude ();

            if (speed < obj->Info.MaxSpeed) {

               speed += obj->Info.Acceleration * TimeDelta;

               if (speed > obj->Info.MaxSpeed) { speed = obj->Info.MaxSpeed; }
            }

            Vector vel (0.0, 0.0, -speed);
            ori.transform_vector (vel);
            const Vector NewPos (pos + (vel * TimeDelta));

            IsectTestContainer test;
            IsectResultContainer isectResults;
            test.set_test (1, IsectSegmentTest, pos, NewPos);

            if (_isect->do_isect (params, test, isectResults)) {

               if (_common) {

                  IsectResult value;
                  isectResults.get_first (value);
                  Handle target (0);
                  value.get_object_handle (target);
                  _common->create_detonation_event (obj->Object, target);
               }

               module->destroy_object (obj->Object);
            }
            else {

               module->store_position (obj->Object, _defaultHandle, NewPos);
               module->store_velocity (obj->Object, _defaultHandle, vel);
               module->store_orientation (obj->Object, _defaultHandle, ori);

               // only re-enable isect if object is not destroyed.
               _isect->enable_isect (obj->Object);
            }
         }
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

   if ((Locality == ObjectLocal) && !_ignoreSet.contains_exact_type (Type) &&
         _typeSet.contains_type (Type)) {

      _register (ObjectHandle, Type);
   }
}


void
dmz::WeaponPluginTrackingMissile::destroy_object (
      const UUID &Identity,
      const Handle ObjectHandle) {

   ObjectStruct *os = _objectTable.remove (ObjectHandle);

   if (os) { delete os; os = 0; }
}


void
dmz::WeaponPluginTrackingMissile::_register (
      const Handle ObjectHandle,
      const ObjectType &Type) {

   TypeStruct *info (_get_type_info (Type));
   ObjectModule *module = get_object_module ();

   if (module && info) {

      Handle source (0);
      Handle target (0);

      HandleContainer sourceList;

      if (module->lookup_super_links (ObjectHandle, _sourceHandle, sourceList)) {

         source = sourceList.get_first ();
      }

      if (source) {

         HandleContainer targetList;

         if (module->lookup_sub_links (source, _targetLockHandle, targetList)) {

            target = targetList.get_first ();
         }
      }

      ObjectStruct *os = new ObjectStruct (*info, ObjectHandle, source, target);

      if (os) {

         if (!_objectTable.store (ObjectHandle, os)) { delete os; os = 0; }
         else {

         }
      }
   }
   if (!info) {

      _ignoreSet.add_object_type (Type);
      _log.error << "Tracking missile type: " << Type.get_name ()
         << " is not defined." << endl;
   }
}


dmz::WeaponPluginTrackingMissile::TypeStruct *
dmz::WeaponPluginTrackingMissile::_get_type_info (const ObjectType &Type) {

   TypeStruct *result (_typeMap.lookup (Type.get_handle ()));

   if (!result) {

      ObjectType current (Type);

      while (current && !result) {

         result = _typeTable.lookup (current.get_handle ());

         if (!result) {

            Config info;

            if (current.get_config ().lookup_all_config ("weapon", info)) {

               const Float64 MaxTurn = config_to_float64 ("turn-rate", info, Pi64 * 0.5);
               const Float64 MaxSpeed = config_to_float64 ("speed", info, 350.0);
               const Float64 Accel = config_to_float64 ("acceleration", info, 600.0);

               result = new TypeStruct (MaxTurn, MaxSpeed, Accel);

               if (result) {

                  if (_typeTable.store (current.get_handle (), result)) {

                     _typeMap.store (Type.get_handle (), result);
                  }
               }
            }
         }

         if (!result) { current.become_parent (); }
      }

      if (result) { _typeMap.store (Type.get_handle (), result); }
   }

   return result;
}


void
dmz::WeaponPluginTrackingMissile::_init (Config &local) {

   RuntimeContext *context (get_plugin_runtime_context ());

   _defaultHandle = activate_default_object_attribute (
      ObjectCreateMask | ObjectDestroyMask);

   _targetLockHandle = config_to_named_handle (
      "target-lock.name",
      local,
      WeaponAttributeTargetLockName,
      context);

   _sourceHandle = config_to_named_handle (
      "source.name",
      local,
      EventAttributeSourceName,
      context);

   _typeSet = config_to_object_type_set ("munitions", local, context);
}
//! \endcond


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzWeaponPluginTrackingMissile (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::WeaponPluginTrackingMissile (Info, local);
}

};
