#include "dmzEntityPluginArticulate.h"
#include <dmzInputEventController.h>
#include <dmzInputEventMasks.h>
#include <dmzObjectAttributeMasks.h>
#include <dmzObjectConsts.h>
#include <dmzObjectModule.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigToState.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>

/*!

\class dmz::EntityPluginArticulate
\ingroup Entity
\brief Sets the values of an entity's articulated parts such as turrets and cannons.
\details
\code
<dmz>
<dmzEntityPluginArticulate>
   <zero id="Button id that resets all components to their default value"/>
   <component
      id="Controller axis id to use to update the component"
      name="Component Attribute Name"
      rate="Rate component can move"
      name-rate="[Optional]Component Rate Attribute Name"
      center="[Optional]Component default value"
      min="[Optional]Minimum component value"
      max="[Optional]Maximum component value"
   />
</dmzEntityPluginArticulate>
</dmz>
\endcode
*/

//! \cond
dmz::EntityPluginArticulate::EntityPluginArticulate (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      TimeSlice (Info),
      InputObserverUtil (Info, local),
      ObjectObserverUtil (Info, local),
      _log (Info),
      _defaultAttrHandle (0),
      _isDead (False),
      _hil (0),
      _hilAttrHandle (0),
      _zero (False),
      _zeroId (5),
      _active (0) {

   _init (local);
}


dmz::EntityPluginArticulate::~EntityPluginArticulate () {

   _compTable.empty ();
}


// Plugin Interface
void
dmz::EntityPluginArticulate::update_plugin_state (
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
dmz::EntityPluginArticulate::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

   }
   else if (Mode == PluginDiscoverRemove) {

   }
}


// TimeSlice Interface
void
dmz::EntityPluginArticulate::update_time_slice (const Float64 TimeDelta) {

   ObjectModule *objMod (get_object_module ());

   if (!_isDead && (_active > 0) && _hil && objMod) {

      ComponentStruct *cs (0);
      HashTableUInt32Iterator it;

      Int32 count (0);

      while (_compTable.get_next (it, cs)) {

         Float64 value (0.0);
         Float64 rate (0.0);
         objMod->lookup_scalar (_hil, cs->AttrHandle, value);

         if (_zero) {

            if (value < -Pi64) { value += TwoPi64; }
            else if (value > Pi64) { value -= TwoPi64; }

            const Float64 Center = cs->center;

            if (value < Center) {
 
               rate = cs->rate;
               value += (rate * TimeDelta);
               if (value >= Center) { value = Center; count++; rate = 0.0; }
            }
            else {

               rate = -cs->rate;
               value += (rate * TimeDelta);
               if (value <= Center) { value = Center; count++; rate = 0.0; }
            }
         }
         else {

            rate = cs->rate * cs->axis;
            value += (rate * TimeDelta);

            if (cs->limit) {

               if (value > cs->max) { value = cs->max; rate = 0.0; }
               else if (value < cs->min) { value = cs->min; rate = 0.0; }
            }
            else {

               if (value > TwoPi64) { value -= TwoPi64; }
               else if (value < -TwoPi64) { value += TwoPi64; }
            }
         }

         objMod->store_scalar (_hil, cs->AttrHandle, value);

         if (cs->RateHandle) { objMod->store_scalar (_hil, cs->RateHandle, rate); }
      }

      if (_zero && count == _compTable.get_count ()) { _zero = False; }
   }
}


// Input Observer Interface
void
dmz::EntityPluginArticulate::update_channel_state (
      const Handle Channel,
      const Boolean State) {

   _active += (State ? 1 : -1);
}


void
dmz::EntityPluginArticulate::receive_axis_event (
      const Handle Channel,
      const InputEventAxis &Value) {

   ComponentStruct *cs = _compTable.lookup (Value.get_axis_id ());

   if (cs) {

      cs->axis = Value.get_axis_value ();
      _zero = False;
   }
}


void
dmz::EntityPluginArticulate::receive_button_event (
      const Handle Channel,
      const InputEventButton &Value) {

   if ((Value.get_button_id () == _zeroId) && Value.get_button_value ()) {

      _zero = True;
   }
}


// Object Observer Interface
void
dmz::EntityPluginArticulate::destroy_object (
      const UUID &Identity,
      const Handle ObjectHandle) {

   if (ObjectHandle == _hil) { _hil = 0; }
}


void
dmz::EntityPluginArticulate::update_object_state (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Mask &Value,
      const Mask *PreviousValue) {

   if (ObjectHandle == _hil) {

      if (Value.contains (_dead)) { _isDead = True; }
      else { _isDead = False; }
   }
}


void
dmz::EntityPluginArticulate::update_object_flag (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Boolean Value,
      const Boolean *PreviousValue) {

   if ((AttributeHandle == _hilAttrHandle) && Value) {

      _hil = ObjectHandle;

      ObjectModule *objMod (get_object_module ());

      if (objMod) {

         Mask state;

         if (objMod->lookup_state (_hil, _defaultAttrHandle, state)) {

            if (state.contains (_dead)) { _isDead = True; }
         }
      }
   }
}


void
dmz::EntityPluginArticulate::update_object_scalar (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Float64 Value,
      const Float64 *PreviousValue) {

}


void
dmz::EntityPluginArticulate::_init (Config &local) {

   Config compList;

   if (local.lookup_all_config ("component", compList)) {

      Definitions defs (get_plugin_runtime_context ());

      ConfigIterator it;
      Config comp;

      while (compList.get_next_config (it, comp)) {

         const String Name = config_to_string ("name", comp);
         const String RateName = config_to_string ("rate-name", comp);
         const UInt32 Id = config_to_uint32 ("id", comp);

         if (Name && Id) {

            ComponentStruct *cs = new ComponentStruct (
               defs.create_named_handle (Name),
               defs.create_named_handle (RateName));

            if (cs) {

               cs->rate = config_to_float64 ("rate", comp, cs->rate);
               cs->center = config_to_float64 ("center", comp, cs->center);
               cs->min = config_to_float64 ("min", comp, cs->min);
               cs->max = config_to_float64 ("max", comp, cs->max);

               if (cs->min < cs->max) { cs->limit = True; }

               if (!_compTable.store (Id, cs)) {

                  _log.error << "Duplicate component with id: " << Id
                     << " and attribute: " << Name << endl;

                  delete cs; cs = 0;
               }
            }
         }
      }
   }

   _defaultAttrHandle =
      activate_default_object_attribute (ObjectDestroyMask | ObjectStateMask);

   _hilAttrHandle = activate_object_attribute (
      ObjectAttributeHumanInTheLoopName,
      ObjectFlagMask);

   _zeroId = config_to_uint32 ("zero.id", local, _zeroId);

   init_input_channels (
      local,
      InputEventAxisMask | InputEventButtonMask | InputEventChannelStateMask,
      &_log);

   _dead = config_to_state (
      "state.dead.value",
      local,
      DefaultStateNameDead,
      get_plugin_runtime_context ());
}
//! \endcond


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzEntityPluginArticulate (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::EntityPluginArticulate (Info, local);
}

};
