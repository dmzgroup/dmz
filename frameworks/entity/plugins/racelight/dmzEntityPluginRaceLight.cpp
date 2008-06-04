#include "dmzEntityPluginRaceLight.h"
#include <dmzInputEventController.h>
#include <dmzInputEventMasks.h>
#include <dmzObjectAttributeMasks.h>
#include <dmzObjectModule.h>
#include <dmzRuntimeConfigRead.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>

dmz::EntityPluginRaceLight::EntityPluginRaceLight (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      InputObserverUtil (Info, local),
      ObjectObserverUtil (Info, local),
      Sync (Info, SyncTypeRuntime, SyncModeSingle, 1.0),
      _defaultHandle (0),
      _lightHandle (0),
      _on (False),
      _list (0),
      _current (0),
      _log (Info) {

   _init (local);
}


dmz::EntityPluginRaceLight::~EntityPluginRaceLight () {

   _current = 0;
   if (_list) { delete _list; _list = 0; }
   _lightTable.empty ();
}


// Input Observer Interface
void
dmz::EntityPluginRaceLight::update_channel_state (
      const Handle Channel,
      const Boolean State) {

}


void
dmz::EntityPluginRaceLight::receive_button_event (
      const Handle Channel,
      const InputEventButton &Value) {

   if ((Value.get_button_id () == 2) && Value.get_button_value ()) {

      if (_list) {

         set_sync_interval (_list->Delay);
         _set_state (*_list, _current);
         _current = _list;
         start_sync ();
      }
   }
}


void
dmz::EntityPluginRaceLight::receive_key_event (
      const Handle Channel,
      const InputEventKey &Value) {

}


// Object Observer Interface
void
dmz::EntityPluginRaceLight::destroy_object (
      const UUID &Identity,
      const Handle ObjectHandle) {

   Handle *ptr (_lightTable.remove (ObjectHandle));

   if (ptr) { delete ptr; ptr = 0; }
}


void
dmz::EntityPluginRaceLight::update_object_flag (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Boolean Value,
      const Boolean *PreviousValue) {

   if (AttributeHandle == _lightHandle) {

      if (Value) {

         Handle *ptr (new Handle (ObjectHandle));

         if (!_lightTable.store (ObjectHandle, ptr)) { delete ptr; ptr = 0; }
      }
      else {

         Handle *ptr = _lightTable.remove (ObjectHandle);

         if (ptr) { delete ptr; ptr = 0; }
      }
   }
}


// Sync Interface
void
dmz::EntityPluginRaceLight::update_sync (const Float64 DeltaTime) {

   if (_current) {

      if (_current->next) {

         StateStruct *prev (_current);
         _current = _current->next;

         _set_state (*_current, prev);

         set_sync_interval (_current->Delay);
         start_sync ();
      }
   }
}


void
dmz::EntityPluginRaceLight::_store_object_module (ObjectModule &objMod) {

}


void
dmz::EntityPluginRaceLight::_set_state (StateStruct &current, StateStruct *prev) {

   ObjectModule *objMod (get_object_module ());

   if (objMod) {

      HashTableHandleIterator it;

      Handle *ptr (_lightTable.get_first (it));

      while (ptr) {

         Mask state;

         objMod->lookup_state (*ptr, _defaultHandle, state);

         if (prev) { state.unset (prev->State); }
         state |= current.State;

         objMod->store_state (*ptr, _defaultHandle, state);

         ptr = _lightTable.get_next (it);
      }
   }
}


void
dmz::EntityPluginRaceLight::_init (Config &local) {

   Definitions defs (get_plugin_runtime_context (), &_log);
   defs.lookup_state ("Light_Red", _redState);
   defs.lookup_state ("Light_Yellow", _yellowState);
   defs.lookup_state ("Light_Green", _greenState);

   _defaultHandle = activate_default_object_attribute (ObjectDestroyMask);

   _lightHandle = activate_object_attribute (
      config_to_string ("light.name", local, "Race_Light"),
      ObjectFlagMask);

   init_input_channels (local, InputEventButtonMask | InputEventChannelStateMask, &_log);

   const Mask EmptyMask;

   StateStruct *current = _list = new StateStruct (_redState, 1.0);
   current->next = new StateStruct (EmptyMask, 0.15);
   current = current->next;
   current->next = new StateStruct (_yellowState, 1.0);
   current = current->next;
   current->next = new StateStruct (EmptyMask, 0.15);
   current = current->next;
   current->next = new StateStruct (_yellowState, 1.0);
   current = current->next;
   current->next = new StateStruct (EmptyMask, 0.15);
   current = current->next;
   current->next = new StateStruct (_yellowState, 1.0);
   current = current->next;
   current->next = new StateStruct (EmptyMask, 0.15);
   current = current->next;
   current->next = new StateStruct (_greenState, 0.0);
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzEntityPluginRaceLight (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::EntityPluginRaceLight (Info, local);
}

};
