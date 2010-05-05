#include <dmzAudioModulePortal.h>
#include "dmzEntityPluginPortalWatch.h"
#include <dmzInputEventMasks.h>
#include <dmzObjectAttributeMasks.h>
#include <dmzObjectModule.h>
#include <dmzRenderModulePortal.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzTypesMatrix.h>
#include <dmzTypesVector.h>

dmz::EntityPluginPortalWatch::EntityPluginPortalWatch (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      TimeSlice (Info),
      MessageObserver (Info),
      InputObserverUtil (Info, local),
      ObjectObserverUtil (Info, local),
      _log (Info),
      _convert (Info),
      _render (0),
      _audio (0),
      _hil (0),
      _target (0),
      _defaultAttrHandle (0),
      _active (0) {

   _init (local);
}


dmz::EntityPluginPortalWatch::~EntityPluginPortalWatch () {

}


// Plugin Interface
void
dmz::EntityPluginPortalWatch::update_plugin_state (
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
dmz::EntityPluginPortalWatch::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      if (!_render) { _render = RenderModulePortal::cast (PluginPtr); }
      if (!_audio) { _audio = AudioModulePortal::cast (PluginPtr); }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_render && (_render == RenderModulePortal::cast (PluginPtr))) { _render = 0; }
      if (_audio && (_audio == AudioModulePortal::cast (PluginPtr))) { _audio = 0; }
   }
}


// TimeSlice Interface
void
dmz::EntityPluginPortalWatch::update_time_slice (const Float64 TimeDelta) {

   if ((_active > 0) && _hil) {

      Vector pos, vel;
      Matrix ori;
      
      ObjectModule *module (get_object_module ());

      if (module) {

         module->lookup_position (_hil, _defaultAttrHandle, pos);
         module->lookup_velocity (_hil, _defaultAttrHandle, vel);

         if (_target) {

            Vector targetPos;

            if (!module->lookup_position (_target, _defaultAttrHandle, targetPos)) {

               if (!module->is_object (_target)) { _target = 0; }
               else { module->lookup_orientation (_hil, _defaultAttrHandle, ori); }
            }
            else { ori.from_vector (targetPos - pos); }
         }

         if (!_target) { module->lookup_orientation (_hil, _defaultAttrHandle, ori); }
      }

      if (_render) { _render->set_view (pos, ori); }
      if (_audio) { _audio->set_view (pos, ori, vel); }
   }
}


// Message Observer Interface
void
dmz::EntityPluginPortalWatch::receive_message (
      const Message &Type,
      const UInt32 MessageSendHandle,
      const Handle TargetObserverHandle,
      const Data *InData,
      Data *outData) {

   _target = _convert.to_handle (InData);
}


// Input Observer Interface
void
dmz::EntityPluginPortalWatch::update_channel_state (
      const Handle Channel,
      const Boolean State) {

   if (State) { _active++; }
   else { _active--; }
}


// Object Observer Interface
void
dmz::EntityPluginPortalWatch::update_object_flag (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Boolean Value,
      const Boolean *PreviousValue) {

   if (Value) { _hil = ObjectHandle; }
   else if (ObjectHandle == _hil) { _hil = 0; }
}


void
dmz::EntityPluginPortalWatch::_init (Config &local) {

   Definitions defs (get_plugin_runtime_context (), &_log);

   _defaultAttrHandle = defs.create_named_handle (ObjectAttributeDefaultName);

   activate_object_attribute (ObjectAttributeHumanInTheLoopName, ObjectFlagMask);

   init_input_channels (local, InputEventChannelStateMask, &_log);

   _targetMessage = config_create_message (
      "target-message.name",
      local,
      "Portal_Watch_Target_Message",
      get_plugin_runtime_context ());

   if (_targetMessage) { subscribe_to_message (_targetMessage); }
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzEntityPluginPortalWatch (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::EntityPluginPortalWatch (Info, local);
}

};
