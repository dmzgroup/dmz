#include "dmzEntityPluginArticulate.h"
#include <dmzInputEventController.h>
#include <dmzInputEventMasks.h>
#include <dmzObjectAttributeMasks.h>
#include <dmzObjectConsts.h>
#include <dmzObjectModule.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>

dmz::EntityPluginArticulate::EntityPluginArticulate (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      TimeSlice (Info),
      InputObserverUtil (Info, local),
      ObjectObserverUtil (Info, local),
      _log (Info),
      _hil (0),
      _hilAttrHandle (0) {

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

   if (_hil && objMod) {

      ComponentStruct *cs (0);
      HashTableUInt32Iterator it;

      while (_compTable.get_next (it, cs)) {

         Float64 value (0.0);
         objMod->lookup_scalar (_hil, cs->AttrHandle, value);
         value += (cs->rate * cs->axis * TimeDelta);

         if (cs->limit) {

            if (value > cs->max) { value = cs->max; }
            else if (value < cs->min) { value = cs->min; }
         }
         else {

            if (value > TwoPi64) { value -= TwoPi64; }
            else if (value < -TwoPi64) { value += TwoPi64; }
         }

         objMod->store_scalar (_hil, cs->AttrHandle, value);
      }
   }
}


// Input Observer Interface
void
dmz::EntityPluginArticulate::update_channel_state (
      const Handle Channel,
      const Boolean State) {

}


void
dmz::EntityPluginArticulate::receive_axis_event (
      const Handle Channel,
      const InputEventAxis &Value) {

   ComponentStruct *cs = _compTable.lookup (Value.get_axis_id ());

   if (cs) { cs->axis = Value.get_axis_value (); }
}


void
dmz::EntityPluginArticulate::receive_button_event (
      const Handle Channel,
      const InputEventButton &Value) {

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

      ConfigIterator it;
      Config comp;

      while (compList.get_next_config (it, comp)) {

         const String Name = config_to_string ("name", comp);
         const UInt32 Id = config_to_uint32 ("id", comp);

         if (Name && Id) {

            ComponentStruct *cs = new ComponentStruct (
               activate_object_attribute (Name, ObjectScalarMask));

            if (cs) {

               cs->rate = config_to_float64 ("rate", comp, cs->rate);
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

   activate_default_object_attribute (ObjectDestroyMask);

   _hilAttrHandle = activate_object_attribute (
      ObjectAttributeHumanInTheLoopName,
      ObjectFlagMask);

   init_input_channels (
      local,
      InputEventAxisMask | InputEventChannelStateMask,
      &_log);
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzEntityPluginArticulate (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::EntityPluginArticulate (Info, local);
}

};
