#include <dmzInputEventKey.h>
#include <dmzInputEventMasks.h>
#include <dmzObjectModule.h>
#include <dmzObjectModuleSelect.h>
#include "dmzObjectPluginSelect.h"
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>

dmz::ObjectPluginSelect::ObjectPluginSelect (const PluginInfo &Info, Config &local) :
      Plugin (Info),
      MessageObserver (Info),
      InputObserverUtil (Info, local),
      _log (Info),
      _convert (Info),
      _object (0),
      _select (0),
      _addToSelection (False),
      _key (0) {

   _init (local);
}


dmz::ObjectPluginSelect::~ObjectPluginSelect () {

}


// Plugin Interface
void
dmz::ObjectPluginSelect::update_plugin_state (
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
dmz::ObjectPluginSelect::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      if (!_object) { _object = ObjectModule::cast (PluginPtr); }
      if (!_select) { _select = ObjectModuleSelect::cast (PluginPtr); }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_object && (_object == ObjectModule::cast (PluginPtr))) { _object = 0; }
      if (_select && (_select == ObjectModuleSelect::cast (PluginPtr))) { _select = 0; }
   }
}


// Message Observer Interface
void
dmz::ObjectPluginSelect::receive_message (
      const Message &Type,
      const UInt32 MessageSendHandle,
      const Handle TargetObserverHandle,
      const Data *InData,
      Data *outData) {

   const Handle Object = _convert.to_handle (InData);

   if (_object && _select) {

      if (Object && _object->is_object (Object)) {

         if (_addToSelection) {

            if (_select->is_selected (Object)) { _select->unselect_object (Object); }
            else { _select->select_object (Object, ObjectSelectAdd); }
         }
         else { _select->select_object (Object, ObjectSelectNew); }
      }
      else if (!Object) { _select->unselect_all_objects (); }
   }
}


// Input Observer Interface
void
dmz::ObjectPluginSelect::receive_key_event (
      const Handle Channel,
      const InputEventKey &Value) {

   if (Value.get_key () == _key) { _addToSelection = Value.get_key_state (); }
}


// ObjectPluginSelect Interface
void
dmz::ObjectPluginSelect::_init (Config &local) {

   _selectMsg = config_create_message (
      "select-message.name",
      local,
      "Object_Select_Message",
      get_plugin_runtime_context ());

   subscribe_to_message (_selectMsg);

   _convert.set_handle (
      config_to_string ("data-convert-handle.name", local, "handle"),
      get_plugin_runtime_context ());

   _key = config_to_key_value ("add-to-selection-key.name", local, KeyShift, &_log);

   init_input_channels (local, InputEventKeyMask, &_log);
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzObjectPluginSelect (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::ObjectPluginSelect (Info, local);
}

};
