#include "dmzEntityPluginHide.h"
#include <dmzInputEventMasks.h>
#include <dmzObjectAttributeMasks.h>
#include <dmzObjectConsts.h>
#include <dmzObjectModule.h>
#include <dmzRuntimeConfigToNamedHandle.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>

dmz::EntityPluginHide::EntityPluginHide (const PluginInfo &Info, Config &local) :
      Plugin (Info),
      InputObserverUtil (Info, local),
      ObjectObserverUtil (Info, local),
      _log (Info),
      _active (0),
      _hil (0),
      _hideHandle (0) {

   _init (local);
}


dmz::EntityPluginHide::~EntityPluginHide () {

}


// Plugin Interface
void
dmz::EntityPluginHide::update_plugin_state (
      const PluginStateEnum State,
      const UInt32 Level) {

   if (State == PluginStateInit) {

   }
   else if (State == PluginStateStart) {

      _update ();
   }
   else if (State == PluginStateStop) {

   }
   else if (State == PluginStateShutdown) {

   }
}


void
dmz::EntityPluginHide::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

   }
   else if (Mode == PluginDiscoverRemove) {

   }
}


// Input Observer Interface
void
dmz::EntityPluginHide::update_channel_state (const Handle Channel, const Boolean State) {

   _active += (State ? 1 : -1);
   _update ();
}


// Object Observer Interface
void
dmz::EntityPluginHide::update_object_flag (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Boolean Value,
      const Boolean *PreviousValue) {

   if (Value) { _hil = ObjectHandle; }
   else if (!Value && (ObjectHandle == _hil)) { _hil = False; }
}


void
dmz::EntityPluginHide::_update () {

   ObjectModule *objMod = get_object_module ();

   if (_hil && objMod) {

      if (1 == _active) { objMod->store_flag (_hil, _hideHandle, True); }
      else if (0 == _active) { objMod->store_flag (_hil, _hideHandle, False); }
   }
}


void
dmz::EntityPluginHide::_init (Config &local) {

   init_input_channels (local, InputEventChannelStateMask, &_log);

   _hideHandle = config_to_named_handle (
      "hide.name",
      local,
      ObjectAttributeHideName,
      get_plugin_runtime_context ());

   activate_object_attribute (ObjectAttributeHumanInTheLoopName, ObjectFlagMask);
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzEntityPluginHide (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::EntityPluginHide (Info, local);
}

};
