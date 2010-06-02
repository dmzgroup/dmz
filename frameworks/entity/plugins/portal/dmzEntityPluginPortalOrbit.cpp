#include <dmzEntityConsts.h>
#include "dmzEntityPluginPortalOrbit.h"
#include <dmzEntityModulePortal.h>
#include <dmzInputEventMasks.h>
#include <dmzInputEventMouse.h>
#include <dmzObjectAttributeMasks.h>
#include <dmzObjectConsts.h>
#include <dmzObjectModule.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzTypesMatrix.h>
#include <dmzTypesVector.h>

dmz::EntityPluginPortalOrbit::EntityPluginPortalOrbit (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      TimeSlice (Info),
      MessageObserver (Info),
      InputObserverUtil (Info, local),
      ObjectObserverUtil (Info, local),
      _log (Info),
      _convert (Info),
      _portal (0),
      _active (0),
      _hil (0),
      _defaultAttrHandle (0),
      _bvrAttrHandle (0),
      _updateRadius (False),
      _heading (0.0),
      _pitch (0.0),
      _radius (20.0) {

   stop_time_slice ();

   _init (local);
}


dmz::EntityPluginPortalOrbit::~EntityPluginPortalOrbit () {

}


// Plugin Interface
void
dmz::EntityPluginPortalOrbit::update_plugin_state (
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
dmz::EntityPluginPortalOrbit::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      if (!_portal) { _portal = EntityModulePortal::cast (PluginPtr); }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_portal && (_portal == EntityModulePortal::cast (PluginPtr))) { _portal = 0; }
   }
}


// TimeSlice Interface
void
dmz::EntityPluginPortalOrbit::update_time_slice (const Float64 TimeDelta) {

   ObjectModule *module (get_object_module ());

   if (_portal && module && _hil) {

      if (_updateRadius) { _update_radius (); }

      Vector hilPos;
      module->lookup_position (_hil, _defaultAttrHandle, hilPos);
      Vector vel;

      module->lookup_velocity (_hil, _defaultAttrHandle, vel);

      Matrix ori (_heading, _pitch, 0.0);

      Vector dir (0.0, 0.0, -1.0);

      ori.transform_vector (dir);

      Vector pos (hilPos + (dir * -_radius));

      _portal->set_view (pos, ori, vel);
   }
}


// Message Observer Interface
void
dmz::EntityPluginPortalOrbit::receive_message (
      const Message &Type,
      const Handle MessageSendHandle,
      const Handle TargetObserverHandle,
      const Data *InData,
      Data *outData) {

   _target = _convert.to_handle (InData);

   _updateRadius = True;
}


// Input Observer Interface
void
dmz::EntityPluginPortalOrbit::update_channel_state (
      const Handle Channel,
      const Boolean State) {

   _active += State ? 1 : -1;

   if (_active == 1) {

      start_time_slice ();
      _updateRadius = True;
   }
   else if (_active == 0) { stop_time_slice (); }
}


void
dmz::EntityPluginPortalOrbit::receive_axis_event (
      const Handle Channel,
      const InputEventAxis &Value) {

}


void
dmz::EntityPluginPortalOrbit::receive_mouse_event (
      const Handle Channel,
      const InputEventMouse &Value) {

   const Float64 SizeY = Float64 (Value.get_window_size_y ());

   if (Value.is_button_pressed (1)) {

      const Float64 SizeX = Float64 (Value.get_window_size_x ());

      const Float64 PercentX =
         Float64 (Value.get_mouse_delta_x ()) / (SizeX > 0.0 ? SizeX : 0.0);

      const Float64 PercentY =
         Float64 (Value.get_mouse_delta_y ()) / (SizeY > 0.0 ? SizeY : 0.0);

      _heading = normalize_angle (_heading - (PercentX * Pi64));
      _pitch = normalize_angle (_pitch - (PercentY * Pi64));
   }

   if (Value.is_button_pressed (3)) {

      const Float64 PercentY =
         Float64 (Value.get_mouse_delta_y ()) / (SizeY > 0.0 ? SizeY : 0.0);

      _radius += PercentY * _radius * 2.0;
   }

   const Int32 ZoomInt = Value.get_scroll_delta_y ();

   if (ZoomInt != 0) {

      _radius += _radius * 0.2 * (ZoomInt > 0 ? 1.0 : -1.0);
   }

   if (_radius < 0.001) { _radius = 0.001; }
}


// Object Observer Interface
void
dmz::EntityPluginPortalOrbit::update_object_flag (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Boolean Value,
      const Boolean *PreviousValue) {

   if (Value) { _hil = ObjectHandle; }
   else if (ObjectHandle == _hil) { _hil = 0; }
}


void
dmz::EntityPluginPortalOrbit::_update_radius () {

   if (_target) {

      ObjectModule *module (get_object_module ());

      if (module) {

         Float64 radius (0.0);

         if (module->lookup_scalar (_target, _bvrAttrHandle, radius)) {

            _radius = radius * 2.0;
            _updateRadius = False;
         }
         else { _updateRadius = True; }
      }
   }
}


void
dmz::EntityPluginPortalOrbit::_init (Config &local) {

   Definitions defs (get_plugin_runtime_context ());

   _defaultAttrHandle = defs.create_named_handle (ObjectAttributeDefaultName);

   _bvrAttrHandle = defs.create_named_handle (ObjectAttributeBoundingVolumeRaidusName);

   if (config_to_boolean ("update-radius-on-attach.value", local, True)) {

      Message msg = config_create_message (
         "attach-message.name",
         local,
         EntityMessageAttachName,
         get_plugin_runtime_context ());

      subscribe_to_message (msg);
   }

   init_input_channels (
      local,
      InputEventChannelStateMask | InputEventMouseMask | InputEventAxisMask,
      &_log);

   activate_object_attribute (ObjectAttributeHumanInTheLoopName, ObjectFlagMask);
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzEntityPluginPortalOrbit (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::EntityPluginPortalOrbit (Info, local);
}

};
