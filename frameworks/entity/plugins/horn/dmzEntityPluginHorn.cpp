#include "dmzEntityPluginHorn.h"
#include <dmzInputEventMasks.h>
#include <dmzInputEventController.h>
#include <dmzObjectAttributeMasks.h>
#include <dmzObjectModule.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>

dmz::EntityPluginHorn::EntityPluginHorn (const PluginInfo &Info, Config &local) :
      Plugin (Info),
      InputObserverUtil (Info, local),
      ObjectObserverUtil (Info, local),
      _handle (0),
      _defaultHandle (0),
      _hilHandle (0),
      _log (Info) {

   _init (local);
}


dmz::EntityPluginHorn::~EntityPluginHorn () {

}


// Input Observer Interface
void
dmz::EntityPluginHorn::update_channel_state (const Handle Channel, const Boolean State) {

}


void
dmz::EntityPluginHorn::receive_button_event (
      const Handle Channel,
      const InputEventButton &Value) {

   if (Value.get_button_id () == 3) {

      ObjectModule *objMod (get_object_module ());

      if (objMod && _handle) {

         Mask state;

         objMod->lookup_state (_handle, _defaultHandle, state);

         if (Value.get_button_value ()) { state |= _hornState; }
         else { state.unset (_hornState); }

         objMod->store_state (_handle, _defaultHandle, state);
      }
   }
}


void
dmz::EntityPluginHorn::receive_key_event (
      const Handle Channel,
      const InputEventKey &Value) {

}


// Object Observer Interface
void
dmz::EntityPluginHorn::update_object_flag (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Boolean Value,
      const Boolean *PreviousValue) {

   if ((AttributeHandle == _hilHandle) && Value) {

      if (_handle && (ObjectHandle != _handle)) {

         ObjectModule *objMod (get_object_module ());

         if (objMod) {

            Mask state;
            if (objMod->lookup_state (_handle, _defaultHandle, state)) {

               state.unset (_hornState);
               objMod->store_state (_handle, _defaultHandle, state);
            }
         }
      }

      _handle = ObjectHandle;
   }
}


void
dmz::EntityPluginHorn::_init (Config &local) {

   Definitions defs (get_plugin_runtime_context (), &_log);

   _defaultHandle = defs.create_named_handle (ObjectAttributeDefaultName);

   _hilHandle = activate_object_attribute (
      ObjectAttributeHumanInTheLoopName,
      ObjectFlagMask);

   init_input_channels (
      local,
      InputEventButtonMask | InputEventKeyMask| InputEventChannelStateMask,
      &_log);

   defs.lookup_state ("Horn_On", _hornState);
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzEntityPluginHorn (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::EntityPluginHorn (Info, local);
}

};
