#include "dmzEntityPluginDeadTimer.h"
#include <dmzObjectAttributeMasks.h>
#include <dmzObjectConsts.h>
#include <dmzObjectModule.h>
#include <dmzRuntimeConfigToBase.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>

dmz::EntityPluginDeadTimer::EntityPluginDeadTimer (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      TimeSlice (Info, TimeSliceTypeRuntime, TimeSliceModeSingle, 2.0),
      ObjectObserverUtil (Info, local),
      _log (Info),
      _hil (0),
      _hilHandle (0),
      _defaultHandle (0) {

   stop_time_slice ();

   _init (local);
}


dmz::EntityPluginDeadTimer::~EntityPluginDeadTimer () {

}


// TimeSlice Interface
void
dmz::EntityPluginDeadTimer::update_time_slice (const Float64 TimeDelta) {

   ObjectModule *objMod (get_object_module ());

   if (objMod && _hil) {

      Mask state;
      objMod->lookup_state (_hil, _defaultHandle, state);
      state.unset (_deadState);
      objMod->store_state (_hil, _defaultHandle, state);
   }
}


void
dmz::EntityPluginDeadTimer::destroy_object (
      const UUID &Identity,
      const Handle ObjectHandle) {

   if (ObjectHandle == _hil) { _hil = 0; }
}


void
dmz::EntityPluginDeadTimer::update_object_state (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Mask &Value,
      const Mask *PreviousValue) {

   if (_deadState && (AttributeHandle == _defaultHandle) && (ObjectHandle == _hil)) {

      const Boolean IsDead (Value.contains (_deadState));
      const Boolean WasDead (
         PreviousValue ? PreviousValue->contains (_deadState) : False);

      if (IsDead && !WasDead) { start_time_slice (); }
   }
}


void
dmz::EntityPluginDeadTimer::update_object_flag (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Boolean Value,
      const Boolean *PreviousValue) {

   if (Value && (AttributeHandle == _hilHandle)) { _hil = ObjectHandle; }
}


void
dmz::EntityPluginDeadTimer::_init (Config &local) {

   Definitions defs (get_plugin_runtime_context (), &_log);

   defs.lookup_state (DefaultStateNameDead, _deadState);

   _defaultHandle =
      activate_default_object_attribute (ObjectDestroyMask | ObjectStateMask);

   _hilHandle = activate_object_attribute (
      ObjectAttributeHumanInTheLoopName,
      ObjectFlagMask);

   set_time_slice_interval (
      config_to_float64 ("timer.value", local, get_time_slice_interval ()));
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzEntityPluginDeadTimer (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::EntityPluginDeadTimer (Info, local);
}

};
