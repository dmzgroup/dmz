#include "dmzEntityPluginTeleport.h"
#include <dmzInputEventController.h>
#include <dmzInputEventMasks.h>
#include <dmzObjectModule.h>
#include <dmzObjectAttributeMasks.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>

/*!

\class dmz::EntityPluginTeleport
\ingroup Entity
\brief Teleports the human-in-the-loop object back to its start position.

*/

//! \cond
dmz::EntityPluginTeleport::EntityPluginTeleport (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      InputObserverUtil (Info, local),
      ObjectObserverUtil (Info, local),
      _ResetButton (config_to_uint32 ("reset.value", local, 4)),
      _active (0),
      _handle (0),
      _defaultHandle (0),
      _hilHandle (0),
      _log (Info) {

   _init (local);
}


dmz::EntityPluginTeleport::~EntityPluginTeleport () {

}


// Input Observer Interface
void
dmz::EntityPluginTeleport::update_channel_state (
      const Handle Channel,
      const Boolean State) {

   _active += (State ? 1 : -1);
}


void
dmz::EntityPluginTeleport::receive_button_event (
      const Handle Channel,
      const InputEventButton &Value) {

   if ((_active > 0) && (Value.get_button_id () == _ResetButton) &&
         Value.get_button_value ()) {

      ObjectModule *objMod (get_object_module ());

      if (_handle && _defaultHandle && objMod) {

         const Vector ZeroVec;

         objMod->store_position (_handle, _defaultHandle, _startPos);
         objMod->store_orientation (_handle, _defaultHandle, _startOri);
         objMod->store_velocity (_handle, _defaultHandle, ZeroVec);
         objMod->store_acceleration (_handle, _defaultHandle, ZeroVec);
      }
   }
}


void
dmz::EntityPluginTeleport::receive_key_event (
      const Handle Channel,
      const InputEventKey &Value) {

}


// Object Observer Interface
void
dmz::EntityPluginTeleport::destroy_object (
      const UUID &Identity,
      const Handle ObjectHandle) {

   if (ObjectHandle == _handle) { _handle = 0; }
}


void
dmz::EntityPluginTeleport::update_object_flag (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Boolean Value,
      const Boolean *PreviousValue) {

   if ((AttributeHandle == _hilHandle) && Value) {

      _handle = ObjectHandle;

      ObjectModule *objMod (get_object_module ());

      if (_defaultHandle && objMod) {

         objMod->lookup_position (_handle, _defaultHandle, _startPos);
         objMod->lookup_orientation (_handle, _defaultHandle, _startOri);
      }
   }
}


void
dmz::EntityPluginTeleport::_init (Config &local) {

   Definitions defs (get_plugin_runtime_context (), &_log);

   _defaultHandle = defs.create_named_handle (ObjectAttributeDefaultName);

   _hilHandle = activate_object_attribute (
      ObjectAttributeHumanInTheLoopName,
      ObjectFlagMask);

   Mask inputMask (InputEventButtonMask | InputEventChannelStateMask);

   // Put key and button map to teleport here.

   init_input_channels (local, inputMask, &_log);
}
//! \endcond


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzEntityPluginTeleport (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::EntityPluginTeleport (Info, local);
}

};
