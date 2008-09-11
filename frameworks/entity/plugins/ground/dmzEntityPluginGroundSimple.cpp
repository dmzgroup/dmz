#include "dmzEntityPluginGroundSimple.h"
#include <dmzEventModuleCommon.h>
#include <dmzInputEventController.h>
#include <dmzInputEventMasks.h>
#include <dmzObjectAttributeMasks.h>
#include <dmzObjectConsts.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigRead.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzTypesMask.h>
#include <dmzTypesMath.h>

dmz::EntityPluginGroundSimple::EntityPluginGroundSimple (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      TimeSlice (Info),
      ObjectObserverUtil (Info, local),
      InputObserverUtil (Info, local),
      _hil (0),
      _defaultHandle (0),
      _hilHandle (0),
      _throttleHandle (0),
      _isect (0),
      _eventMod (0),
      _wasAirborn (False),
      _isDead (False),
      _active (0),
      _time (Info),
      _log (Info) {

   _init (local);
}


dmz::EntityPluginGroundSimple::~EntityPluginGroundSimple () {

}


// Plugin Interface
void
dmz::EntityPluginGroundSimple::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      if (!_isect) { _isect = RenderModuleIsect::cast (PluginPtr); }
      if (!_eventMod) { _eventMod = EventModuleCommon::cast (PluginPtr); }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_isect && (_isect == RenderModuleIsect::cast (PluginPtr))) { _isect = 0; }

      if (_eventMod && (_eventMod == EventModuleCommon::cast (PluginPtr))) {

         _eventMod = 0;
      }
   }
}


// TimeSlice Interface
void
dmz::EntityPluginGroundSimple::update_time_slice (const Float64 TimeDelta) {

   ObjectModule *objMod (get_object_module ());

   if ((_active > 0) && objMod && _hil && _defaultHandle) {

      Vector pos, vel;
      Matrix ori;

      objMod->lookup_position (_hil, _defaultHandle, pos);
      objMod->lookup_velocity (_hil, _defaultHandle, vel);
      objMod->lookup_orientation (_hil, _defaultHandle, ori);

      const Vector OldPos (pos);
      const Matrix OldOri (ori);

      const Vector Up (0.0, 1.0, 0.0);

      Vector point (pos), normal (Up);

      Boolean airborn (False);

      if (_find_point (pos, ori, point, normal)) {

         Float64 Diff (pos.get_y () - point.get_y ());

         Float64 Drop (EarthGravity64 * TimeDelta * TimeDelta);

         if (Diff > Drop) { airborn = True; }
      }

      if (_eventMod && _wasAirborn && !airborn) {

         _eventMod->create_collision_event (_hil, 0);
      }

      _wasAirborn = airborn;

      Float64 heading (0.0);

      _move_entity (TimeDelta, airborn, pos, ori, vel, heading);

      _validate_move (TimeDelta, airborn, OldPos, pos, ori, vel, heading);

      if (_find_point (pos, ori, point, normal)) {

         if ((point.get_y () > pos.get_y ()) || (point - pos).is_zero (0.01)) {

            pos = point;

            Matrix hm (Up, heading);
            Matrix nmat (Up, normal);

            ori = nmat * hm;

            get_ortho (normal, vel, vel);
         }
      }

      objMod->store_position (_hil, _defaultHandle, pos);
      objMod->store_velocity (_hil, _defaultHandle, vel);
      objMod->store_orientation (_hil, _defaultHandle, ori);

      Float64 target (_move.throttle + 0.3);

      if (_move.turboModifier > 0.0) { target += 0.3; }

      if (airborn) { target = (1.5 * _move.throttle) + 0.3; }

      Float64 current (1.0);

      objMod->lookup_scalar (_hil, _throttleHandle, current);

      if (current > target) {

         current -= 0.6 * TimeDelta;

         if (current < target) { current = target; }
      }
      else if (current < target) {

         current += 0.4 * TimeDelta;

         if (current > target) { current = target; }
      }

      if (_isDead) { current = 0.0; }

      objMod->store_scalar (_hil, _throttleHandle, current);

      const Float64 Time (_time.get_frame_time ());

      if ((_move.turboModifier > 0.0) && (Time > _move.turboEndTime)) {

         _move.turboModifier = 0.0;
         _move.turboRefillTime = Time + _move.turboRefillDuration;
      }
   }
}



// Object Observer Interface
void
dmz::EntityPluginGroundSimple::update_object_state (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Mask &Value,
      const Mask *PreviousValue) {

   if (_deadState && (AttributeHandle == _defaultHandle) && (ObjectHandle == _hil)) {

      const Boolean IsDead (Value.contains (_deadState));
      const Boolean WasDead (
         PreviousValue ? PreviousValue->contains (_deadState) : False);

      if (IsDead && !WasDead) { _isDead = True; }
      else if (!IsDead && WasDead) { _isDead = False; }
   }
}


void
dmz::EntityPluginGroundSimple::update_object_flag (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Boolean Value,
      const Boolean *PreviousValue) {

   if ((AttributeHandle == _hilHandle) && Value) { _hil = ObjectHandle; }
}


void
dmz::EntityPluginGroundSimple::update_object_scalar (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Float64 Value,
      const Float64 *PreviousValue) {;}


// Input Observer Interface
void
dmz::EntityPluginGroundSimple::update_channel_state (
      const Handle Channel,
      const Boolean State) {

   _active += (State ? 1 : -1);

   if (_active == 0) {

      _wasAirborn = False;

      if (_hil && _throttleHandle) {

         ObjectModule *objMod (get_object_module ());

         if (objMod) { objMod->store_scalar (_hil, _throttleHandle, 0.0); } 
      }
   }
}


void
dmz::EntityPluginGroundSimple::receive_axis_event (
      const Handle Channel,
      const InputEventAxis &Value) {

   const Float64 AxisValue =
      ((Value.get_axis_value () < 0.1) && (Value.get_axis_value () > -0.1)) ? 0.0 :
      Value.get_axis_value () * fabs (Value.get_axis_value ());

   const Handle Axis (Value.get_axis_id ());

   if (Axis ==  1) { _move.turn = AxisValue; }
   else if (Axis == 2) {

      if (AxisValue < 0.0) {

         _move.throttle = -AxisValue;
         _move.brake = 0.0;
      }
      else if (AxisValue > 0.0) {

         _move.brake = AxisValue;
         _move.throttle = 0.0;
      }
      else { _move.brake = _move.throttle = 0.0; }
   }
}


void
dmz::EntityPluginGroundSimple::receive_button_event (
      const Handle Channel,
      const InputEventButton &Value) {

   if ((Value.get_button_id () == 1) && Value.get_button_value ()) {

      const Float64 Time (_time.get_frame_time ());

      if (!(_move.turboModifier > 0.0) && (Time > _move.turboRefillTime)) {

         _move.turboModifier = _move.turboBoost;
         _move.turboEndTime = Time + _move.turboDuration;
      }
   }
   else if ((Value.get_button_id () == 3) && Value.get_button_value ()) {

      ObjectModule *objMod (get_object_module ());

      if (objMod) {

         Vector pos;
         Matrix ori;

         objMod->lookup_position (_hil, _defaultHandle, pos);
         objMod->lookup_orientation (_hil, _defaultHandle, ori);

         _log.out << "\n<position x=\"" << pos.get_x ()
            << "\" y=\"" << pos.get_y ()
            << "\" z=\"" << pos.get_z () << "\"/>\n";

         _log.out << "<orientation\n";

         Float64 array[9];

         ori.to_array (array);

         for (Int32 ix = 0; ix < 9; ix++) {

            if (!(ix % 3)) { _log.out << "   "; }

            _log.out << " v" << ix << "=\"" << array[ix] << "\"";

            if (!((ix + 1) % 3)) { _log.out << "\n"; }
         }

         _log.out << "/>" << endl;
      }
   }
}


void
dmz::EntityPluginGroundSimple::receive_key_event (
      const Handle Channel,
      const InputEventKey &Value) {

}


void
dmz::EntityPluginGroundSimple::receive_mouse_event (
      const Handle Channel,
      const InputEventMouse &Value) {

}


void
dmz::EntityPluginGroundSimple::receive_switch_event (
      const Handle Channel,
      const InputEventSwitch &Value) {

}


dmz::Boolean
dmz::EntityPluginGroundSimple::_find_point (
      const Vector &Pos,
      const Matrix &Ori,
      Vector &point,
      Vector &normal) {

   Boolean result (False);

   if (_isect) {

      _isect->disable_isect (_hil);

      const Vector Up (0.0, 1.0, 0.0);
      const Vector Down (0.0, -1.0, 0.0);
      Vector offset (0.0, 1.5, 0.0);
      Vector rdown (Down);

      Ori.transform_vector (offset);
      Ori.transform_vector (rdown);

      Boolean testRelDown (True);

      if (rdown.get_angle (Down) > HalfPi64) {

         testRelDown = False;
         rdown = Down;
      }

      Vector start (Pos + offset);

      _isectTestContainer.set_test (
         1, 
         IsectRayTest,
         start,
         rdown);

      IsectResultContainer isectResults;

      if (testRelDown) {
   
         if (_isect->do_isect (_isectParameters, _isectTestContainer, isectResults)) {

            result = _validate_isect_result (start, rdown, isectResults, point, normal);
         }
      }

      if (!result) {

         const Vector DownStart (start.get_x (), start.get_y () + 1.0, start.get_z ());

         _isectTestContainer.set_test (
            1, 
            IsectRayTest,
            DownStart,
            Down);

         if (_isect->do_isect (_isectParameters, _isectTestContainer, isectResults)) {

            result = _validate_isect_result (start, Down, isectResults, point, normal);
         }
      }

      if (!result) {

         const Vector UpStart (start.get_x (), start.get_y () - 1.0, start.get_z ());

         _isectTestContainer.set_test (
            1, 
            IsectRayTest,
            UpStart,
            Up);

         if (_isect->do_isect (_isectParameters, _isectTestContainer, isectResults)) {

            result = _validate_isect_result (start, Up, isectResults, point, normal);
         }
      }

      _isect->enable_isect (_hil);
   }

   return result;
}


dmz::Boolean
dmz::EntityPluginGroundSimple::_validate_isect_result (
      const Vector &Start,
      const Vector &Dir,
      const IsectResultContainer &IsectResults,
      Vector &point,
      Vector &normal) {

   Boolean result (False);

   const Vector Up (0.0, 1.0, 0.0);

   const Boolean TestingUp (Dir.get_angle (Up) < HalfPi64 ? True : False);
   
   IsectResult isResult;

   Boolean found (IsectResults.get_first (isResult));

   while (found) {

      Vector cpoint, cnormal;

      Handle handle (0);

      isResult.get_object_handle (handle);

      if (!handle && isResult.get_point (cpoint) && isResult.get_normal (cnormal)) { 

         if ((!result) ||
               ((Start - point).magnitude_squared () >
                  (Start - cpoint).magnitude_squared ())) {

            Handle cullMode (0);

            isResult.get_cull_mode (cullMode);

            if (!(cullMode & IsectPolygonBackCulledMask) &&
                  !(cullMode & IsectPolygonFrontCulledMask)) {

               if (Dir.get_angle (cnormal) < HalfPi64) {

                  cnormal = -cnormal;
               }
            }
            else if (cullMode & IsectPolygonFrontCulledMask) { cnormal = -cnormal; }

            if (TestingUp) {

               if (Dir.get_angle (cnormal) < HalfPi64) {

                  result = True;
                  point = cpoint;
                  normal = cnormal;
               }
            }
            else {

               if (Dir.get_angle (cnormal) > HalfPi64) {

                  result = True;
                  point = cpoint;
                  normal = cnormal;
               }
            }
         }
      }

      found = IsectResults.get_next (isResult);
   }

   return result;
}


void
dmz::EntityPluginGroundSimple::_move_entity (
      const Float64 TimeDelta,
      const Boolean Airborn,
      Vector &pos,
      Matrix &ori,
      Vector &vel,
      Float64 &heading) {

   const Vector Up (0.0, 1.0, 0.0);
   const Vector Forward (0.0, 0.0, -1.0);

   Vector vup (Up), vforward (Forward);

   ori.transform_vector (vup);
   ori.transform_vector (vforward);

   Matrix mat (vup, Up);

   mat.transform_vector (vforward);

   heading = vforward.get_angle (Forward);

   const Vector Cross (vforward.cross (Forward));

   if (Cross.get_y () > 0.0) { heading = TwoPi64 - heading; }

   if (Airborn) {

      vel.set_y (vel.get_y () - (EarthGravity64 * TimeDelta));

      pos += vel * TimeDelta;
   }
   else if (_isDead) { pos += vel * TimeDelta; }
   else {

      const Float64 ForwardFactor ((vforward.get_angle (vel) > HalfPi64) ? -1.0 : 1.0);

      mat.transpose_in_place ();

      heading -= _move.turn * _move.turnRate * TimeDelta;

      Matrix hm (Up, heading);

      ori = mat * hm;

      vforward = Forward;

      ori.transform_vector (vforward);

      Float64 speed (vel.magnitude () - (_move.brake * _move.brakeRate * TimeDelta));

      if (!is_zero64 (_move.brake) && (speed < 0.0)) { speed = 0.0; }

      if (!is_zero64 (_move.throttle)) {

         const Float64 TargetSpeed (
            (_move.maxSpeed + _move.turboModifier) * _move.throttle);

         if ((speed * ForwardFactor) < TargetSpeed) {

            speed += (_move.throttle * _move.maxAccel * TimeDelta);

            if (speed > TargetSpeed) { speed = TargetSpeed; }
         }
         else if ((speed * ForwardFactor) > TargetSpeed) {

            speed -= (_move.maxDecel * TimeDelta);

            if (speed < 0) { speed = 0; }
         }
      }

      if (is_zero64 (_move.throttle) && is_zero64 (_move.brake))  {

         if (is_zero64 (vforward.get_y (), 0.05)) {

            speed -= _move.maxDecel * TimeDelta;

            if (speed < 0.0) { speed = 0.0; }
         }
         else {

            speed -= EarthGravity64 * vforward.get_y () * TimeDelta * ForwardFactor;
         }

         speed = speed * ForwardFactor;
      }

      vel = vforward * speed;

      pos += vel * TimeDelta;
   }
}


void
dmz::EntityPluginGroundSimple::_validate_move (
      const Float64 TimeDelta,
      const Boolean Airborn,
      const Vector StartPos,
      Vector &pos,
      Matrix &ori,
      Vector &vel,
      Float64 &heading) {

   if (!Airborn && _isect && !(StartPos - pos).is_zero ()) {

      _isect->disable_isect (_hil);

      Vector right (1.0, 0.0, 0.0);
      Vector left (-1.0, 0.0, 0.0);
      Vector rightEnd (_move.whiskerLength, _move.whiskerHeight, 0.0);
      Vector centerOfSeg (0.0, _move.whiskerHeight, 0.0);
      Vector leftEnd (-_move.whiskerLength, _move.whiskerHeight, 0.0);

      ori.transform_vector (right);
      ori.transform_vector (left);
      ori.transform_vector (rightEnd);
      ori.transform_vector (centerOfSeg);
      ori.transform_vector (leftEnd);

      rightEnd += pos;
      centerOfSeg += pos;
      leftEnd += pos;

       _isectTestContainer.set_test (
         1, 
         IsectSegmentTest,
         rightEnd,
         leftEnd);

      IsectResultContainer isectResults;

      if (_isect->do_isect (_isectParameters, _isectTestContainer, isectResults)) {

         const Matrix TransOri (ori.transpose ());

         Boolean rightFound (False), leftFound (False);
         Float64 rightDistance (0.0), leftDistance (0.0);
         Vector rightPoint, rightNorm, leftPoint, leftNorm;
   
         IsectResult isResult;

         Boolean found (isectResults.get_first (isResult));

         while (found) {

            Handle handle (0);

            isResult.get_object_handle (handle);

            if (1) { // !handle) {

               Vector point, normal;
               Handle cullMode (0);

               isResult.get_point (point);
               isResult.get_normal (normal);
               isResult.get_cull_mode (cullMode);

               Vector transPoint (point - centerOfSeg);

               TransOri.transform_vector (transPoint);

               // Right side
               if (transPoint.get_x () > 0.0) {

                  _validate_move_isect_point (
                     cullMode,
                     rightDistance,
                     fabs (transPoint.get_x ()),
                     point,
                     right,
                     normal,
                     rightFound,
                     rightDistance,
                     rightPoint,
                     rightNorm);
               }
               else { // Left side

                  _validate_move_isect_point (
                     cullMode,
                     leftDistance,
                     fabs (transPoint.get_x ()),
                     point,
                     left,
                     normal,
                     leftFound,
                     leftDistance,
                     leftPoint,
                     leftNorm);
               }
            }
       
            found = isectResults.get_next (isResult);
         }

         Boolean calcNewOri (False);

         if (rightFound && leftFound) {

 _log.error << "Right and Left Collision." << endl;
         }
         else if (rightFound) {

            pos += rightPoint - rightEnd;
            calcNewOri = True;
         }
         else if (leftFound) {

            pos += leftPoint - leftEnd;
            calcNewOri = True;
         }

         if (calcNewOri) {

            const Float64 Speed (vel.magnitude ());
            Vector newDir ((pos - StartPos).normalize ());
            vel = newDir *  Speed;

            const Vector Forward (0.0, 0.0, -1.0);
            Vector vforward (Forward);

            ori.transform_vector (vforward);

            Matrix mat (vforward, newDir);

            ori = ori * mat;

            heading = newDir.get_angle (Forward);

            const Vector Cross (newDir.cross (Forward));

            if (Cross.get_y () < 0.0) { heading = TwoPi64 - heading; }
         }
      }

      _isect->enable_isect (_hil);
   }
}


void
dmz::EntityPluginGroundSimple::_validate_move_isect_point (
      const Handle CullMode,
      const Float64 LastDistance,
      const Float64 Distance,
      const Vector &Point,
      const Vector &Dir,
      const Vector &Normal,
      Boolean &found,
      Float64 &outDistance,
      Vector &outPoint,
      Vector &outNorm) {

   Vector normal (Normal);

   if (!(CullMode & IsectPolygonBackCulledMask) &&
         !(CullMode & IsectPolygonFrontCulledMask)) {

      if (Dir.get_angle (normal) < HalfPi64) {

         normal = -normal;
      }
   }
   else if (CullMode & IsectPolygonFrontCulledMask) { normal = -normal; }

   if (Dir.get_angle (normal) > HalfPi64) {

      if (!found || (LastDistance > Distance)) {

         found = True;
         outDistance = Distance;
         outPoint = Point;
         outNorm = normal;
      }
   }
}


void
dmz::EntityPluginGroundSimple::_init (Config &local) {

   Definitions defs (get_plugin_runtime_context (), &_log);

   defs.lookup_state (DefaultStateNameDead, _deadState);

   _defaultHandle = activate_default_object_attribute (ObjectStateMask);

   _hilHandle = activate_object_attribute (
      ObjectAttributeHumanInTheLoopName,
      ObjectFlagMask);

   _throttleHandle = activate_object_attribute (
      ObjectAttributeScalarThrottleName,
      ObjectScalarMask);

   _move.maxSpeed = config_to_float64 ("movement.speed", local, _move.maxSpeed);
   _move.maxAccel = config_to_float64 ("movement.acceleration", local, _move.maxAccel);
   _move.maxDecel = config_to_float64 ("movement.deceleration", local, _move.maxDecel);
   _move.turnRate = config_to_float64 ("movement.turn", local, _move.turnRate);
   _move.brakeRate = config_to_float64 ("movement.brake", local, _move.brakeRate);
   _move.whiskerLength = config_to_float64 ("whisker.length", local, _move.whiskerLength);
   _move.whiskerHeight = config_to_float64 ("whisker.height", local, _move.whiskerHeight);

   init_input_channels (
      local,
      InputEventButtonMask | InputEventAxisMask | InputEventChannelStateMask,
      &_log);

   _isectParameters.set_test_result_type (IsectAllPoints);
   _isectParameters.set_calculate_normal (True);
   _isectParameters.set_calculate_object_handle (True);
   _isectParameters.set_calculate_distance (False);
   _isectParameters.set_calculate_cull_mode (True);
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzEntityPluginGroundSimple (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::EntityPluginGroundSimple (Info, local);
}

};
