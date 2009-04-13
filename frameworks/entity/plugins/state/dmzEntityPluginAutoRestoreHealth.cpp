#include <dmzEntityConsts.h>
#include "dmzEntityPluginAutoRestoreHealth.h"
#include <dmzObjectAttributeMasks.h>
#include <dmzObjectModule.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>

dmz::EntityPluginAutoRestoreHealth::EntityPluginAutoRestoreHealth (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      TimeSlice (Info),
      ObjectObserverUtil (Info, local),
      _log (Info),
      _healRate (10.0),
      _maxHealth (100.0),
      _healthIncrease (1.0),
      _healthAttrHandle (0) {

   _init (local);
}


dmz::EntityPluginAutoRestoreHealth::~EntityPluginAutoRestoreHealth () {

   _objTable.empty ();
}


// Plugin Interface
void
dmz::EntityPluginAutoRestoreHealth::update_plugin_state (
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
dmz::EntityPluginAutoRestoreHealth::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

   }
   else if (Mode == PluginDiscoverRemove) {

   }
}


// TimeSlice Interface
void
dmz::EntityPluginAutoRestoreHealth::update_time_slice (const Float64 TimeDelta) {

   ObjectModule *objMod (get_object_module ());

   if (objMod) {

      HashTableHandleIterator it;
      HealthStruct *current (0);

      while (_objTable.get_next (it, current)) {

         current->timer += TimeDelta;

         if (current->timer >= _healRate) {

            current->timer = current->timer - _healRate;

            Float64 health (0.0);

            objMod->lookup_scalar (current->ObjectHandle, _healthAttrHandle, health);

            if (health < _maxHealth) {

               health += _healthIncrease;
               if (health > _maxHealth) { health = _maxHealth; }
               objMod->store_scalar (current->ObjectHandle, _healthAttrHandle, health);
            }
         }
      }
   }
}


// Object Observer Interface
void
dmz::EntityPluginAutoRestoreHealth::destroy_object (
      const UUID &Identity,
      const Handle ObjectHandle) {

   HealthStruct *hs = _objTable.remove (ObjectHandle);

   if (hs) { delete hs; hs = 0; }
}


void
dmz::EntityPluginAutoRestoreHealth::update_object_scalar (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Float64 Value,
      const Float64 *PreviousValue) {

   HealthStruct *hs = _objTable.lookup (ObjectHandle);

   if (!hs) {

      ObjectModule *objMod (get_object_module ());

      if (objMod) {

         if (objMod->lookup_locality (ObjectHandle) == ObjectLocal) {

            hs = new HealthStruct (ObjectHandle);

            if (!_objTable.store (ObjectHandle, hs)) { delete hs; hs = 0; }
         }
      }
   }
}


void
dmz::EntityPluginAutoRestoreHealth::_init (Config &local) {

   activate_default_object_attribute (ObjectDestroyMask);

   _healthAttrHandle = activate_object_attribute (
      EntityAttributeHealthName,
      ObjectScalarMask);

   _healRate = config_to_float64 ("health.rate", local, _healRate);
   _maxHealth = config_to_float64 ("health.max", local, _maxHealth);
   _healthIncrease = config_to_float64 ("health.increase", local, _healthIncrease);
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzEntityPluginAutoRestoreHealth (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::EntityPluginAutoRestoreHealth (Info, local);
}

};
