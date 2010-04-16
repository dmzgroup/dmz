#include <dmzEventConsts.h>
#include <dmzInputEventMasks.h>
#include <dmzInputEventController.h>
#include <dmzObjectAttributeMasks.h>
#include <dmzObjectConsts.h>
#include <dmzObjectModule.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeConfigToState.h>
#include <dmzRuntimeConfigToVector.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include "dmzWeaponPluginFixedLauncher.h"

/*!

\class dmz::WeaponPluginFixedLauncher
\ingroup Weapon
\brief Creates a munition object.
\details
\code
<dmz>
<dmzWeaponPluginFixedLauncher>
   <state>
      <dead value="Dead State Name"/>
   </state>
   <munitions>
      <object-type name="Munitions ObjectType Name"/>
   </munitions>
   <delay value="Delay time between launches"/>
   <offset x="X Value" y="Y Value" z="Z Value"/>
   <rotation x="Pitch Value" y="Heading Value" z="Roll Value"/>
   <button value="Button ID"/>
</dmzWeaponPluginFixedLauncher>
</dmz>
\endcode

*/

//! \cond
dmz::WeaponPluginFixedLauncher::WeaponPluginFixedLauncher (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      TimeSlice (Info),
      MessageObserver (Info),
      InputObserverUtil (Info, local),
      ObjectObserverUtil (Info, local),
      _log (Info),
      _time (Info),
      _convert (Info),
      _ammo (Info, local, &_log),
      _delay (0.5),
      _hilActive (True),
      _hilAttrHandle (0),
      _hil (0),
      _defaultHandle (0),
      _launchButton (2) {

   _init (local);
}


dmz::WeaponPluginFixedLauncher::~WeaponPluginFixedLauncher () {

   _launchTable.empty ();
}


// Plugin Interface
void
dmz::WeaponPluginFixedLauncher::update_plugin_state (
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
dmz::WeaponPluginFixedLauncher::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

   }
   else if (Mode == PluginDiscoverRemove) {

   }
}


// TimeSlice Interface
void
dmz::WeaponPluginFixedLauncher::update_time_slice (const Float64 TimeDelta) {

   const Float64 CTime (_time.get_frame_time ());

   HashTableHandleIterator it;

   LaunchStruct *ls (_launchTable.get_first (it));

   while (ls) {

      if ((ls->activeCount > 0) && ((ls->lastLaunchTime + _delay) < CTime)) {

         _create_munition (ls->Source);
         ls->lastLaunchTime = CTime;
      }

      ls = _launchTable.get_next (it);
   }
}


// Message Observer Interface
void
dmz::WeaponPluginFixedLauncher::receive_message (
      const Message &Type,
      const UInt32 MessageSendHandle,
      const Handle TargetObserverHandle,
      const Data *InData,
      Data *outData) {

   const Handle Source = _convert.to_handle (InData);

   if (Source) {

      LaunchStruct *ls (_get_struct (Source));

      if (ls) {

         const Float64 CTime (_time.get_frame_time ());

         if (CTime > (ls->lastLaunchTime + _delay)) {

            _create_munition (ls->Source);
            ls->lastLaunchTime = CTime;
         }
      }
   }
}


// Input Observer Interface
void
dmz::WeaponPluginFixedLauncher::update_channel_state (
      const Handle Channel,
      const Boolean State) {

}


void
dmz::WeaponPluginFixedLauncher::receive_button_event (
      const Handle Channel,
      const InputEventButton &Value) {

   if (_hil) {

      if (Value.get_button_id () == _launchButton) {

         LaunchStruct *ls (_get_struct (_hil));

         if (ls) {

            if (Value.get_button_value ()) {

               const Float64 CTime (_time.get_frame_time ());

               ls->activeCount++;

               if (CTime > (ls->lastLaunchTime + _delay)) {

                  _create_munition (ls->Source);
                  ls->lastLaunchTime = CTime;
               }
            }
            else { ls->activeCount--; }
         }
      }
   }
}


void
dmz::WeaponPluginFixedLauncher::receive_switch_event (
      const Handle Channel,
      const InputEventSwitch &Value) {

}


void
dmz::WeaponPluginFixedLauncher::receive_key_event (
      const Handle Channel,
      const InputEventKey &Value) {

}


void
dmz::WeaponPluginFixedLauncher::receive_mouse_event (
      const Handle Channel,
      const InputEventMouse &Value) {

}


// Object Observer Interface
void
dmz::WeaponPluginFixedLauncher::destroy_object (
      const UUID &Identity,
      const Handle ObjectHandle) {

   if (ObjectHandle == _hil) { _hil = 0; }

   LaunchStruct *ls (_launchTable.remove (ObjectHandle));

   if (ls) { delete ls; ls = 0; }
}


void
dmz::WeaponPluginFixedLauncher::update_object_state (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Mask &Value,
      const Mask *PreviousValue) {

   if (_hil && (ObjectHandle == _hil) && (AttributeHandle == _defaultHandle)) {

      if (Value.contains (_deadState)) { _hilActive = False; }
      else { _hilActive = True; }
   }
}


void
dmz::WeaponPluginFixedLauncher::update_object_flag (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Boolean Value,
      const Boolean *PreviousValue) {

   if (Value) {

      _hil = ObjectHandle;

      Mask state;
      ObjectModule *objMod (get_object_module ());

      if (_hil && objMod && objMod->lookup_state (_hil, _defaultHandle, state)) {

         if (state.contains (_deadState)) { _hilActive = False; }
         else { _hilActive = True; }
      }
   }
   else if (ObjectHandle == _hil) { _hil = 0; }
}


dmz::WeaponPluginFixedLauncher::LaunchStruct *
dmz::WeaponPluginFixedLauncher::_get_struct (const Handle Source) {

   LaunchStruct *result (_launchTable.lookup (Source));

   if (!result) {

      result = new LaunchStruct (Source);
      if (result && !_launchTable.store (Source, result)) { delete result; result = 0; }
   }

   return result;
}


void
dmz::WeaponPluginFixedLauncher::_create_munition (const Handle SourceHandle) {

   ObjectModule *objMod (get_object_module ());

   Boolean active (True);

   if ((SourceHandle == _hil) && !_hilActive) { active = False; }

   if (active && objMod) {

      const Handle AmmoHandle = _ammo.create_munition (SourceHandle, *objMod);

      if (AmmoHandle) {

         Matrix ori;
         Vector pos, vel;
         objMod->lookup_orientation (SourceHandle, _defaultHandle, ori);
         objMod->lookup_position (SourceHandle, _defaultHandle, pos);
         objMod->lookup_velocity (SourceHandle, _defaultHandle, vel);

         Vector offset (_launcherOffset);
         ori = ori * _launcherRotation;
         ori.transform_vector (offset);
         pos += offset;

         objMod->store_orientation (AmmoHandle, _defaultHandle, ori);
         objMod->store_position (AmmoHandle, _defaultHandle, pos);
         objMod->store_velocity (AmmoHandle, _defaultHandle, vel);

         objMod->activate_object (AmmoHandle);
      }
   }
}


void
dmz::WeaponPluginFixedLauncher::_init (Config &local) {

   RuntimeContext *context (get_plugin_runtime_context ());

   Definitions defs (context, &_log);

   _defaultHandle = activate_default_object_attribute (
      ObjectStateMask);

   _hilAttrHandle = activate_object_attribute (
      ObjectAttributeHumanInTheLoopName,
      ObjectFlagMask);

   _deadState = config_to_state (
      "state.dead.value",
      local,
      DefaultStateNameDead,
      context);

   _delay = config_to_float64 ("delay.value", local, _delay);
   _launcherOffset = config_to_vector ("offset", local);
   const Vector HPR = config_to_vector ("rotation", local);

   _launcherRotation.roll_in_place (HPR.get_z ());
   _launcherRotation.pitch_in_place (HPR.get_x ());
   _launcherRotation.yaw_in_place (HPR.get_y ());

   Config convergeConfig;

   if (local.lookup_config ("converge", convergeConfig)) {

      const Vector Converge = config_to_vector (convergeConfig);
      const Vector Dir = (Converge - _launcherOffset);
      _launcherRotation.from_two_vectors (Vector (0, 0, -1), Dir.normalize ());
   }

   _launchButton = config_to_uint32 ("button.value", local, _launchButton);

   init_input_channels (
      local,
      InputEventButtonMask | InputEventChannelStateMask,
      &_log);

   Message launch = config_create_message ("launch-message.name", local, "", context);

   if (launch) { subscribe_to_message (launch); }
}
//! \endcond


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzWeaponPluginFixedLauncher (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::WeaponPluginFixedLauncher (Info, local);
}

};
