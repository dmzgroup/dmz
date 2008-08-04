#include "dmzWeaponPluginFixedLauncher.h"
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>

dmz::WeaponPluginFixedLauncher::WeaponPluginFixedLauncher (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      TimeSlice (Info),
      MessageObserver (Info),
      InputObserverUtil (Info, local),
      ObjectObserverUtil (Info, local),
      _log (Info) {

   _init (local);
}


dmz::WeaponPluginFixedLauncher::~WeaponPluginFixedLauncher () {

}


// Plugin Interface
void
dmz::WeaponPluginFixedLauncher::update_plugin_state (
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
dmz::WeaponPluginFixedLauncher::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

   }
   else if (Mode == PluginDiscoverRemove) {

   }
}


// TimeSlice Interface
void
dmz::WeaponPluginFixedLauncher::update_time_slice (const Float64 TimeDelta) {


}


// Message Observer Interface
void
dmz::WeaponPluginFixedLauncher::receive_message (
      const Message &Type,
      const UInt32 MessageSendHandle,
      const Handle TargetObserverHandle,
      const Data *InData,
      Data *outData) {

}


// Input Observer Interface
void
dmz::WeaponPluginFixedLauncher::update_channel_state (
      const Handle Channel,
      const Boolean State) {

}


void
dmz::WeaponPluginFixedLauncher::receive_button_event (
      const Handle Channel,
      const InputEventButton &Value) {

}


void
dmz::WeaponPluginFixedLauncher::receive_switch_event (
      const Handle Channel,
      const InputEventSwitch &Value) {

}


void
dmz::WeaponPluginFixedLauncher::receive_key_event (
      const Handle Channel,
      const InputEventKey &Value) {

}


void
dmz::WeaponPluginFixedLauncher::receive_mouse_event (
      const Handle Channel,
      const InputEventMouse &Value) {

}


// Object Observer Interface
void
dmz::WeaponPluginFixedLauncher::destroy_object (
      const UUID &Identity,
      const Handle ObjectHandle) {

}


void
dmz::WeaponPluginFixedLauncher::update_object_state (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Mask &Value,
      const Mask *PreviousValue) {

}


void
dmz::WeaponPluginFixedLauncher::update_object_flag (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Boolean Value,
      const Boolean *PreviousValue) {

}


void
dmz::WeaponPluginFixedLauncher::update_object_position (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Value,
      const Vector *PreviousValue) {

}


void
dmz::WeaponPluginFixedLauncher::update_object_orientation (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Matrix &Value,
      const Matrix *PreviousValue) {

}


void
dmz::WeaponPluginFixedLauncher::update_object_velocity (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Value,
      const Vector *PreviousValue) {

}


void
dmz::WeaponPluginFixedLauncher::_init (Config &local) {

}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzWeaponPluginFixedLauncher (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::WeaponPluginFixedLauncher (Info, local);
}

};
