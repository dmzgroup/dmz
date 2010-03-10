#include "dmzEntityPluginFreeFly.h"
#include <dmzInputEventController.h>
#include <dmzInputEventMasks.h>
#include <dmzObjectAttributeMasks.h>
#include <dmzObjectConsts.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzTypesMask.h>

/*!

\defgroup Entity DMZ Entity Framework

\class dmz::EntityPluginFreeFly
\ingroup Entity
\brief Plugin provides free fly motion.
\details
\code
<movement speed="max speed" turn-rate="turn rate"/>
\endcode
*/

//! \cond
dmz::EntityPluginFreeFly::EntityPluginFreeFly (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      TimeSlice (Info),
      ObjectObserverUtil (Info, local),
      InputObserverUtil (Info, local),
      _handle (0),
      _defaultHandle (0),
      _hilHandle (0),
      _isect (0),
      _active (0),
      _log (Info.get_name (), Info.get_context ()) {

   _init (local);
}


dmz::EntityPluginFreeFly::~EntityPluginFreeFly () {

}


// Plugin Interface
void
dmz::EntityPluginFreeFly::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      if (!_isect) { _isect = RenderModuleIsect::cast (PluginPtr); }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_isect && (_isect == RenderModuleIsect::cast (PluginPtr))) { _isect = 0; }
   }
}


// TimeSlice Interface
void
dmz::EntityPluginFreeFly::update_time_slice (const Float64 TimeDelta) {

   ObjectModule *objMod (get_object_module ());

   if ((_active > 0) && objMod && _handle && _defaultHandle) {

      Vector pos, vel;
      Matrix ori;

      objMod->lookup_position (_handle, _defaultHandle, pos);
      objMod->lookup_velocity (_handle, _defaultHandle, vel);
      objMod->lookup_orientation (_handle, _defaultHandle, ori);

      const Vector Right (1.0, 0.0, 0.0);
      const Vector Up (0.0, 1.0, 0.0);
      const Vector Forward (0.0, 0.0, -1.0);
      Vector dir (Forward);

      ori.transform_vector (dir);

      Vector headingVec (dir);
      headingVec.set_y (0.0);
      headingVec.normalize_in_place ();

      Float64 heading (Forward.get_angle (headingVec));
      const Float64 HCross (Forward.cross (headingVec).normalize_in_place ().get_y ());

      if (HCross < 0.0) {

         heading += _move.turnAxis * TimeDelta;
         heading = TwoPi64 - heading;
      }
      else { heading -= _move.turnAxis * TimeDelta; }

      if (heading > Pi64) { heading -= TwoPi64; }
      else if (heading < -Pi64) { heading += TwoPi64; }

      Float64 pitch (dir.get_angle (headingVec));

      if (dir.get_y () < 0.0) {

         pitch += _move.pitchAxis * TimeDelta;
         pitch = TwoPi64 - pitch;
      }
      else {

         pitch -= _move.pitchAxis * TimeDelta;
      }

      if ((pitch > HalfPi64) && (pitch <= (Pi64))) { pitch = HalfPi64 - 0.001; }
      else if ((pitch < (Pi64 + HalfPi64)) && (pitch > Pi64)) {

         pitch = HalfPi64 + Pi64 + 0.001;
      }

      Matrix pm (Right, pitch);
      Matrix hm (Up, heading);

      ori = hm * pm;

      dir = Forward;
      Vector slide (Right);
      Vector ymove (Up);

      ori.transform_vector (dir);
      ori.transform_vector (slide);
      ori.transform_vector (ymove);

      const Vector OldPos (pos);

      pos -= (dir * (_move.speedAxis * TimeDelta));
      pos += (slide * (_move.strafeAxis * TimeDelta));
      pos += (ymove * (_move.ymoveAxis * TimeDelta));

      if (!is_zero64 (TimeDelta)) { vel = (pos - OldPos) * (1 /  TimeDelta); }

      objMod->store_position (_handle, _defaultHandle, pos);
      objMod->store_velocity (_handle, _defaultHandle, vel);
      objMod->store_orientation (_handle, _defaultHandle, ori);
   }
}



// Object Observer Interface
void
dmz::EntityPluginFreeFly::update_object_flag (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Boolean Value,
      const Boolean *PreviousValue) {

   if ((AttributeHandle == _hilHandle) && Value) { _handle = ObjectHandle; }
}


void
dmz::EntityPluginFreeFly::update_object_scalar (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Float64 Value,
      const Float64 *PreviousValue) {;}


// Input Observer Interface
void
dmz::EntityPluginFreeFly::update_channel_state (
      const Handle Channel,
      const Boolean State) {

   if (State) { _active++; }
   else { _active--; }
}


void
dmz::EntityPluginFreeFly::receive_axis_event (
      const Handle Channel,
      const InputEventAxis &Value) {

   const Float64 AxisValue =
      ((Value.get_axis_value () < 0.1) && (Value.get_axis_value () > -0.1)) ? 0.0 :
      Value.get_axis_value () * fabs (Value.get_axis_value ());

   if (!is_zero64 (AxisValue)) {

      //_log.out << "Axis: " << Value.get_axis_id () << " Value: " << AxisValue << endl;
   }

   switch (Value.get_axis_id ()) {

      case 2: { _move.speedAxis = AxisValue * _move.moveSpeed; break; }
      case 1: { _move.turnAxis = AxisValue * _move.turnRate; break; }
      case 6: { _move.strafeAxis = AxisValue * _move.moveSpeed; break; }
      case 7: { _move.pitchAxis = AxisValue * _move.turnRate; break; }
      case 8: { _move.ymoveAxis = AxisValue * _move.moveSpeed; break; }
   }
}


void
dmz::EntityPluginFreeFly::receive_button_event (
      const Handle Channel,
      const InputEventButton &Value) {

   _log.out << "B" << endl;
}


void
dmz::EntityPluginFreeFly::receive_key_event (
      const Handle Channel,
      const InputEventKey &Value) {

}


void
dmz::EntityPluginFreeFly::receive_mouse_event (
      const Handle Channel,
      const InputEventMouse &Value) {

}


void
dmz::EntityPluginFreeFly::receive_switch_event (
      const Handle Channel,
      const InputEventSwitch &Value) {

}



void
dmz::EntityPluginFreeFly::_init (Config &local) {

   Definitions defs (get_plugin_runtime_context (), &_log);

   _defaultHandle = defs.create_named_handle (ObjectAttributeDefaultName);

   _hilHandle = activate_object_attribute (
      ObjectAttributeHumanInTheLoopName,
      ObjectFlagMask);

   _move.moveSpeed = config_to_float64 ("movement.speed", local, _move.moveSpeed);
   _move.turnRate = config_to_float64 ("movement.turn-rate", local, _move.turnRate);

   init_input_channels (
      local,
      InputEventButtonMask | InputEventAxisMask | InputEventChannelStateMask,
      &_log);

   _isectParameters.set_test_result_type (IsectClosestPoint);
   _isectParameters.set_calculate_normal (False);
   _isectParameters.set_calculate_object_handle (False);
   _isectParameters.set_calculate_distance (True);
   _isectParameters.set_calculate_cull_mode (False);
}
//! \endcond


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzEntityPluginFreeFly (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::EntityPluginFreeFly (Info, local);
}

};
