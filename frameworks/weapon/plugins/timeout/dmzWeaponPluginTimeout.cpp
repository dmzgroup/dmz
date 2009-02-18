#include <dmzEventModuleCommon.h>
#include <dmzObjectAttributeMasks.h>
#include <dmzObjectModule.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeObjectType.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include "dmzWeaponPluginTimeout.h"

dmz::WeaponPluginTimeout::WeaponPluginTimeout (const PluginInfo &Info, Config &local) :
      Plugin (Info),
      TimeSlice (Info),
      ObjectObserverUtil (Info, local),
      _log (Info),
      _eventMod (0),
      _defaultTimeout (new Float64 (-1.0)) {

   _init (local);
}


dmz::WeaponPluginTimeout::~WeaponPluginTimeout () {

   _timeoutTable.clear ();
   if (_defaultTimeout) { delete _defaultTimeout; _defaultTimeout = 0; }
   _masterTimeoutTable.empty ();
}


// Plugin Interface
void
dmz::WeaponPluginTimeout::update_plugin_state (
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
dmz::WeaponPluginTimeout::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      if (!_eventMod) { _eventMod = EventModuleCommon::cast (PluginPtr); }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_eventMod && (_eventMod == EventModuleCommon::cast (PluginPtr))) {

         _eventMod = 0; 
      }
   }
}


// TimeSlice Interface
void
dmz::WeaponPluginTimeout::update_time_slice (const Float64 DeltaTime) {

   HashTableHandleIterator it;
   Float64 *ptr = _objTable.get_first (it);

   while (ptr) {

      *ptr -= DeltaTime;

      if (*ptr < 0.0) {

         const Handle Object (it.get_hash_key ());

         ObjectModule *objMod = get_object_module ();

         if (objMod) {
     
            if (_eventMod) { _eventMod->create_detonation_event (Object, 0); }

            objMod->destroy_object (Object);
         }
      }

      ptr = _objTable.get_next (it);
   }
}


// Object Observer Interface
void
dmz::WeaponPluginTimeout::create_object (
      const UUID &Identity,
      const Handle ObjectHandle,
      const ObjectType &Type,
      const ObjectLocalityEnum Locality) {

   if (Locality == ObjectLocal) {

      Float64 *timeout = _find_timeout (Type);

      if (timeout && (*timeout > 0.0)) {

         Float64 *ptr = new Float64 (*timeout);

         if (ptr && !_objTable.store (ObjectHandle, ptr)) { delete ptr; ptr = 0; }
      }
   }
}


void
dmz::WeaponPluginTimeout::destroy_object (
      const UUID &Identity,
      const Handle ObjectHandle) {

   Float64 *ptr = _objTable.remove (ObjectHandle);

   if (ptr) { delete ptr; ptr = 0; }
}


dmz::Float64 *
dmz::WeaponPluginTimeout::_find_timeout (const ObjectType &Type) {

   ObjectType current (Type);

   Float64 *result = _timeoutTable.lookup (current.get_handle ());

   while (!result && current) {

      const Float64 Timeout =
         config_to_float64 ("timeout.value", current.get_config (), -1.0);

      if (Timeout > 0.0) {

         result = new Float64 (Timeout);

         if (result && _masterTimeoutTable.store (current.get_handle (), result)) {

            _timeoutTable.store (Type.get_handle (), result);
         }
         else if (result) { delete result; result = 0; }

         current.set_type_context (0);
      }
      else {

         current.become_parent ();

         if (current) {

            result = _timeoutTable.lookup (current.get_handle ());

            if (result) { _timeoutTable.store (Type.get_handle (), result); }
         }
      }
   }

   if (!result) {

      _timeoutTable.store (Type.get_handle (), _defaultTimeout);
   }

   return result;
}

void
dmz::WeaponPluginTimeout::_init (Config &local) {

   activate_default_object_attribute (ObjectCreateMask | ObjectDestroyMask);
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzWeaponPluginTimeout (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::WeaponPluginTimeout (Info, local);
}

};
