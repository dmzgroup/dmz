#include <dmzEntityConsts.h>
#include "dmzEntityPluginFollow.h"
#include <dmzInputEventMasks.h>
#include <dmzObjectAttributeMasks.h>
#include <dmzObjectModule.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzTypesMatrix.h>
#include <dmzTypesVector.h>

/*!

\class dmz::EntityPluginFollow
\ingroup Entity
\brief Follows the selected object.
\details Maintains a maximum distance. Plugin does not account for terrain and is best
suited for airborne objects.

*/

//! \cond
dmz::EntityPluginFollow::EntityPluginFollow (
      const PluginInfo &Info,
      Config &local) :
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
      _range (50.0),
      _active (0) {

   _init (local);
}


dmz::EntityPluginFollow::~EntityPluginFollow () {

}


// Plugin Interface
void
dmz::EntityPluginFollow::update_plugin_state (
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
dmz::EntityPluginFollow::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

   }
   else if (Mode == PluginDiscoverRemove) {

   }
}


// TimeSlice Interface
void
dmz::EntityPluginFollow::update_time_slice (const Float64 TimeDelta) {

   ObjectModule *module (get_object_module ());

   if ((_active > 0) && _hil && _target && module) {

      Vector pos, targetPos;

      module->lookup_position (_hil, _defaultAttrHandle, pos);

      if (module->lookup_position (_target, _defaultAttrHandle, targetPos)) {

         if ((targetPos - pos).magnitude () > _range) {

            Vector dir = pos - targetPos;
            dir.normalize_in_place ();
            dir *= _range;
            pos = targetPos + dir;

            module->store_position (_hil, _defaultAttrHandle, pos);
         }
      }
      else if (!module->is_object (_target)) { _target = 0; }
   }
}


// Message Observer Interface
void
dmz::EntityPluginFollow::receive_message (
      const Message &Type,
      const UInt32 MessageSendHandle,
      const Handle TargetObserverHandle,
      const Data *InData,
      Data *outData) {

   _target = _convert.to_handle (InData);
}


// Input Observer Interface
void
dmz::EntityPluginFollow::update_channel_state (
      const Handle Channel,
      const Boolean State) {

   if (State) { _active++; }
   else { _active--; }
}


// Object Observer Interface
void
dmz::EntityPluginFollow::update_object_flag (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Boolean Value,
      const Boolean *PreviousValue) {

   if (Value) { _hil = ObjectHandle; }
   else if (ObjectHandle == _hil) { _hil = 0; }
}


void
dmz::EntityPluginFollow::_init (Config &local) {

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

   _range = config_to_float64 ("range.value", local, _range);
}
//! \endcond


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzEntityPluginFollow (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::EntityPluginFollow (Info, local);
}

};
