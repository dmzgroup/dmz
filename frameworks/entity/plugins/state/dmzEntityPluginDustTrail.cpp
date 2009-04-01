#include "dmzEntityPluginDustTrail.h"
#include <dmzObjectAttributeMasks.h>
#include <dmzObjectConsts.h>
#include <dmzObjectModule.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzTypesVector.h>

/*!

\class dmz::EntityPluginDustTrail
\ingroup Entity
\brief Stets dust trail state for the human-in-the-loop object.
\details The plugin detects if the human-in-the-loop object is moving and sets the
dust trail state for the object. Uses dmz::DefaultStateNameDustTrail by default.
\code
<dmz>
<dmzEntityPluginDustTrail>
   <state name="state name"/>
</dmzEntityPluginDustTrail>
</dmz>
\endcode

*/


//! \cond
dmz::EntityPluginDustTrail::EntityPluginDustTrail (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      ObjectObserverUtil (Info, local),
      _log (Info),
      _hil (0),
      _hilHandle (0), 
      _defaultObjectHandle (0) {

   _init (local);
}


dmz::EntityPluginDustTrail::~EntityPluginDustTrail () {

}


// Plugin Interface
void
dmz::EntityPluginDustTrail::update_plugin_state (
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
dmz::EntityPluginDustTrail::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

   }
   else if (Mode == PluginDiscoverRemove) {

   }
}


// Object Observer Interface
void  
dmz::EntityPluginDustTrail::destroy_object (
      const UUID &Identity,
      const Handle ObjectHandle) {
         
   if (ObjectHandle == _hil) { _hil = 0; }
}


void
dmz::EntityPluginDustTrail::update_object_flag (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Boolean Value,
      const Boolean *PreviousValue) {

   if (Value && (AttributeHandle == _hilHandle)) { _hil = ObjectHandle; }
}


void
dmz::EntityPluginDustTrail::update_object_state (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Mask &Value,
      const Mask *PreviousValue) {

   ObjectModule *objMod = get_object_module ();

   if ((ObjectHandle == _hil) && objMod) {

      Vector vel;
      Mask state (Value);

      objMod->lookup_velocity (_hil, _defaultObjectHandle, vel);

      _update_dust_trail (vel, state);

      objMod->store_state (_hil, _defaultObjectHandle, state);
   }
}


void
dmz::EntityPluginDustTrail::update_object_velocity (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Value,
      const Vector *PreviousValue) {

   ObjectModule *objMod = get_object_module ();

   if ((ObjectHandle == _hil) && objMod) {

      Mask state;

      objMod->lookup_state (_hil, _defaultObjectHandle, state);

      _update_dust_trail (Value, state);

      objMod->store_state (_hil, _defaultObjectHandle, state);
   }
}


void
dmz::EntityPluginDustTrail::_update_dust_trail (const Vector &Velocity, Mask &state) {

   if (Velocity.magnitude () > 0.1) {

      if (state.contains (_airBornState)) { state.unset (_dustState); }
      else { state |= _dustState; }
   }
   else { state.unset (_dustState); }
}


void
dmz::EntityPluginDustTrail::_init (Config &local) {

   Definitions defs (get_plugin_runtime_context (), &_log);

   const String DustStateName =
      config_to_string ("dust-state.name", local, DefaultStateNameDustTrail);

   const String AirBornStateName =
      config_to_string ("air-born-state.name", local, DefaultStateNameAirBorn);

   defs.lookup_state (DustStateName, _dustState);
   defs.lookup_state (AirBornStateName, _airBornState);

   _defaultObjectHandle = activate_default_object_attribute (
      ObjectDestroyMask | ObjectVelocityMask | ObjectStateMask);

   _hilHandle = activate_object_attribute (
      ObjectAttributeHumanInTheLoopName,
      ObjectFlagMask);
}

//! \endcond

extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzEntityPluginDustTrail (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::EntityPluginDustTrail (Info, local);
}

};
