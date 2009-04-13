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
#include "dmzWeaponPluginLauncher.h"

/*!

\class dmz::WeaponPluginLauncher
\ingroup Weapon
\brief Creates a munition object.
\details
\code
<dmz>
<dmzWeaponPluginLauncher>
   <state>
      <dead value="Dead State Name"/>
   </state>
   <munitions>
      <object-type name="Munitions ObjectType Name"/>
   </munitions>
   <delay value="Delay time between launches"/>
   <transform>
      <component name="Attribute Name" axis="Axis of Rotation" x="X Offset" y="Y Offset" z="Z Offset"/>
   </transform>
   <button value="Button ID"/>
</dmzWeaponPluginLauncher>
</dmz>
\endcode

*/

//! \cond
dmz::WeaponPluginLauncher::WeaponPluginLauncher (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      TimeSlice (Info),
      MessageObserver (Info),
      InputObserverUtil (Info, local),
      ObjectObserverUtil (Info, local),
      _log (Info),
      _time (Info),
      _ammo (Info, local, &_log),
      _transform (0),
      _delay (0.5),
      _hilActive (True),
      _hilAttrHandle (0),
      _hil (0),
      _defaultHandle (0),
      _launchButton (2) {

   _init (local);
}


dmz::WeaponPluginLauncher::~WeaponPluginLauncher () {

   _componentTable.clear ();
   _launchTable.empty ();

   if (_transform) { delete _transform; _transform = 0; }
}


// Plugin Interface
void
dmz::WeaponPluginLauncher::update_plugin_state (
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
dmz::WeaponPluginLauncher::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

   }
   else if (Mode == PluginDiscoverRemove) {

   }
}


// TimeSlice Interface
void
dmz::WeaponPluginLauncher::update_time_slice (const Float64 TimeDelta) {

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
dmz::WeaponPluginLauncher::receive_message (
      const Message &Type,
      const UInt32 MessageSendHandle,
      const Handle TargetObserverHandle,
      const Data *InData,
      Data *outData) {

}


// Input Observer Interface
void
dmz::WeaponPluginLauncher::update_channel_state (
      const Handle Channel,
      const Boolean State) {

}


void
dmz::WeaponPluginLauncher::receive_button_event (
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
dmz::WeaponPluginLauncher::receive_switch_event (
      const Handle Channel,
      const InputEventSwitch &Value) {

}


void
dmz::WeaponPluginLauncher::receive_key_event (
      const Handle Channel,
      const InputEventKey &Value) {

}


void
dmz::WeaponPluginLauncher::receive_mouse_event (
      const Handle Channel,
      const InputEventMouse &Value) {

}


// Object Observer Interface
void
dmz::WeaponPluginLauncher::destroy_object (
      const UUID &Identity,
      const Handle ObjectHandle) {

   if (ObjectHandle == _hil) { _hil = 0; }

   LaunchStruct *ls (_launchTable.remove (ObjectHandle));

   if (ls) { delete ls; ls = 0; }
}


void
dmz::WeaponPluginLauncher::update_object_state (
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
dmz::WeaponPluginLauncher::update_object_flag (
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


void
dmz::WeaponPluginLauncher::update_object_scalar (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Float64 Value,
      const Float64 *PreviousValue) {

   if (_hil && (ObjectHandle == _hil)) {

      ComponentStruct *cs = _componentTable.lookup (AttributeHandle);

      if (cs) { cs->angle = Value; }
   }
}


dmz::WeaponPluginLauncher::LaunchStruct *
dmz::WeaponPluginLauncher::_get_struct (const Handle Source) {

   LaunchStruct *result (_launchTable.lookup (Source));

   if (!result) {

      result = new LaunchStruct (Source);
      if (result && !_launchTable.store (Source, result)) { delete result; result = 0; }
   }

   return result;
}


void
dmz::WeaponPluginLauncher::_create_munition (const Handle SourceHandle) {

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

         Vector offset;
         Matrix rot;

         ComponentStruct *current = _transform;

         while (current) {

            offset += current->Offset;

            if (current->AttrHandle) {

               Matrix mat (current->Axis, current->angle);
               mat.transform_vector (offset);
               rot = mat * rot;
            }

            current = current->next;
         }

         ori.transform_vector (offset);
         pos += offset;

         objMod->store_orientation (AmmoHandle, _defaultHandle, ori * rot);
         objMod->store_position (AmmoHandle, _defaultHandle, pos);
         objMod->store_velocity (AmmoHandle, _defaultHandle, vel);

         objMod->activate_object (AmmoHandle);
      }
   }
}


void
dmz::WeaponPluginLauncher::_init (Config &local) {

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

   _launchButton = config_to_uint32 ("button.value", local, _launchButton);

   Config compList;

   if (local.lookup_all_config ("transform.component", compList)) {

      ConfigIterator it;
      Config comp;

      while (compList.get_next_config (it, comp)) {

         const Vector Offset = config_to_vector (comp);
         const VectorComponentEnum Axis = config_to_vector_component ("axis", comp);
         const String Name = config_to_string ("name", comp);

         const Handle AttrHandle =
            (Name ?  activate_object_attribute (Name, ObjectScalarMask) : 0);

         Vector axis;
         axis.set (Axis, 1.0);

         ComponentStruct *cs = new ComponentStruct (AttrHandle, axis, Offset);

         if (cs) {

            cs->next = _transform;
            _transform = cs;

            if (cs->AttrHandle) {

               _componentTable.store (cs->AttrHandle, cs);
            }
         }
      }
   }

   init_input_channels (
      local,
      InputEventButtonMask | InputEventChannelStateMask,
      &_log);

}
//! \endcond


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzWeaponPluginLauncher (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::WeaponPluginLauncher (Info, local);
}

};
