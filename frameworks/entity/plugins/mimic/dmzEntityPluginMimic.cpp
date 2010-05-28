#include <dmzEntityConsts.h>
#include "dmzEntityPluginMimic.h"
#include <dmzInputEventMasks.h>
#include <dmzObjectAttributeMasks.h>
#include <dmzObjectConsts.h>
#include <dmzObjectModule.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzTypesMatrix.h>
#include <dmzTypesVector.h>

dmz::EntityPluginMimic::EntityPluginMimic (const PluginInfo &Info, Config &local) :
      Plugin (Info),
      TimeSlice (Info),
      MessageObserver (Info),
      InputObserverUtil (Info, local),
      ObjectObserverUtil (Info, local),
      _log (Info),
      _convert (Info),
      _hil (0),
      _target (0),
      _defaultAttrHandle (0),
      _active (0) {

   _init (local);

   stop_time_slice ();
}


dmz::EntityPluginMimic::~EntityPluginMimic () {

}


// Plugin Interface
void
dmz::EntityPluginMimic::update_plugin_state (
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
dmz::EntityPluginMimic::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

   }
   else if (Mode == PluginDiscoverRemove) {

   }
}


// TimeSlice Interface
void
dmz::EntityPluginMimic::update_time_slice (const Float64 TimeDelta) {


   ObjectModule *module (get_object_module ());

   if (module && _hil && _target) {

      Vector pos, vel;
      Matrix ori;

      module->lookup_position (_target, _defaultAttrHandle, pos);
      module->lookup_orientation (_target, _defaultAttrHandle, ori);
      module->lookup_velocity (_target, _defaultAttrHandle, vel);

      module->store_position (_hil, _defaultAttrHandle, pos);
      module->store_orientation (_hil, _defaultAttrHandle, ori);
      module->store_velocity (_hil, _defaultAttrHandle, vel);
   }
}


// Message Observer Interface
void
dmz::EntityPluginMimic::receive_message (
      const Message &Type,
      const UInt32 MessageSendHandle,
      const Handle TargetObserverHandle,
      const Data *InData,
      Data *outData) {

   _target = _convert.to_handle (InData);
}


// Input Observer Interface
void
dmz::EntityPluginMimic::update_channel_state (const Handle Channel, const Boolean State) {

   _active += State ? 1 : -1;

   if (_active == 1) { start_time_slice (); }
   else if (_active == 0) { stop_time_slice (); }
}


// Object Observer Interface
void
dmz::EntityPluginMimic::update_object_flag (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Boolean Value,
      const Boolean *PreviousValue) {

   if (Value) { _hil = ObjectHandle; }
   else if (ObjectHandle == _hil) { _hil = 0; }
}


void
dmz::EntityPluginMimic::_init (Config &local) {

   Definitions defs (get_plugin_runtime_context (), &_log);

   _defaultAttrHandle = defs.create_named_handle (ObjectAttributeDefaultName);

   activate_object_attribute (ObjectAttributeHumanInTheLoopName, ObjectFlagMask);

   init_input_channels (local, InputEventChannelStateMask, &_log);

   _targetMessage = config_create_message (
      "target-message.name",
      local,
      EntityMessageAttachName,
      get_plugin_runtime_context ());

   if (_targetMessage) { subscribe_to_message (_targetMessage); }
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzEntityPluginMimic (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::EntityPluginMimic (Info, local);
}

};
