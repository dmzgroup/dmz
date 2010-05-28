#include "dmzEntityPluginPortalOrbit.h"
#include <dmzEntityModulePortal.h>
#include <dmzInputEventMasks.h>
#include <dmzInputEventMouse.h>
#include <dmzObjectAttributeMasks.h>
#include <dmzObjectConsts.h>
#include <dmzObjectModule.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzTypesMatrix.h>
#include <dmzTypesVector.h>

dmz::EntityPluginPortalOrbit::EntityPluginPortalOrbit (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      TimeSlice (Info),
      InputObserverUtil (Info, local),
      ObjectObserverUtil (Info, local),
      _log (Info),
      _portal (0),
      _active (0),
      _hil (0),
      _defaultAttrHandle (0),
      _bvRadiusAttrHandle (0),
      _heading (0.0),
      _pitch (0.0),
      _dir (0.0, 0.0, -1.0),
      _radius (20.0) {

   _init (local);

   stop_time_slice ();
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

      Vector hilPos;
      module->lookup_position (_hil, _defaultAttrHandle, hilPos);
      Vector vel;

      module->lookup_velocity (_hil, _defaultAttrHandle, vel);

      Matrix ori (_dir);

      Float64 hy (0.0), px (0.0), rz (0.0);

      ori.to_euler_angles (hy, px, rz);

      hy = normalize_angle (hy + _heading);
      px = normalize_angle (px + _pitch);

      ori.from_euler_angles (hy, px, 0.0);

      _dir.set_xyz (0.0, 0.0, -1.0);

      ori.transform_vector (_dir);

      Vector portalPos (hilPos + (_dir * -_radius));

      Matrix portalOri (_dir);

      _portal->set_view (portalPos, portalOri, vel);
   }

   _heading = _pitch = 0.0f;
}


// Input Observer Interface
void
dmz::EntityPluginPortalOrbit::update_channel_state (
      const Handle Channel,
      const Boolean State) {

   _active += State ? 1 : -1;

   if (_active == 1) { start_time_slice (); }
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

      _heading -= PercentX * Pi64;
      _pitch -= PercentY * Pi64;
   }

   if (Value.is_button_pressed (3)) {

      const Float64 PercentY =
         Float64 (Value.get_mouse_delta_y ()) / (SizeY > 0.0 ? SizeY : 0.0);

      _radius += PercentY * 5.0;
   }

   const Int32 ZoomInt = Value.get_scroll_delta_y ();

   if (ZoomInt != 0) {

      _radius += Float64 (ZoomInt) * 0.2;
   }
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
dmz::EntityPluginPortalOrbit::_init (Config &local) {

   Definitions defs (get_plugin_runtime_context ());

   _defaultAttrHandle = defs.create_named_handle (ObjectAttributeDefaultName);

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
