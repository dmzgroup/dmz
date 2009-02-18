#include "dmzEntityPluginDustTrail.h"
#include <dmzObjectAttributeMasks.h>
#include <dmzObjectConsts.h>
#include <dmzObjectModule.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzTypesVector.h>

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
dmz::EntityPluginDustTrail::update_object_velocity (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Value,
      const Vector *PreviousValue) {

   ObjectModule *objMod = get_object_module ();

   if (_hil && objMod) {

      Mask state;

      objMod->lookup_state (_hil, _defaultObjectHandle, state);

      if (Value.magnitude () > 0.1) { state |= _dustState; }
      else { state.unset (_dustState); }

      objMod->store_state (_hil, _defaultObjectHandle, state);
   }
}


void
dmz::EntityPluginDustTrail::_init (Config &local) {

   Definitions defs (get_plugin_runtime_context (), &_log);

   const String StateName =
      config_to_string ("state.name", local, DefaultStateNameDustTrail);

   defs.lookup_state (StateName, _dustState);

   _defaultObjectHandle =
      activate_default_object_attribute (ObjectDestroyMask | ObjectVelocityMask);

   _hilHandle = activate_object_attribute (
      ObjectAttributeHumanInTheLoopName,
      ObjectFlagMask);
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzEntityPluginDustTrail (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::EntityPluginDustTrail (Info, local);
}

};
