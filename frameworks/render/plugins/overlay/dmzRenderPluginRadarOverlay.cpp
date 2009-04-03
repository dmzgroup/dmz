#include <dmzObjectAttributeMasks.h>
#include <dmzObjectConsts.h>
#include <dmzRenderModuleOverlay.h>
#include "dmzRenderPluginRadarOverlay.h"
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>

dmz::RenderPluginRadarOverlay::RenderPluginRadarOverlay (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      TimeSlice (Info),
      InputObserverUtil (Info, local),
      ObjectObserverUtil (Info, local),
      _log (Info),
      _overlay (0),
      _hilAttrHandle (0),
      _hil (0),
      _radius (0.0) {

   _init (local);
}


dmz::RenderPluginRadarOverlay::~RenderPluginRadarOverlay () {

   _defTable.empty ();
   _defMasterTable.empty ();
   _objTable.empty ();
}


// Plugin Interface
void
dmz::RenderPluginRadarOverlay::update_plugin_state (
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
dmz::RenderPluginRadarOverlay::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      if (!_overlay) { _overlay = RenderModuleOverlay::cast (PluginPtr); }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_overlay && (_overlay == RenderModuleOverlay::cast (PluginPtr))) {

         _overlay = 0;
      }
   }
}


// TimeSlice Interface
void
dmz::RenderPluginRadarOverlay::update_time_slice (const Float64 TimeDelta) {


}


// Input Observer Interface
void
dmz::RenderPluginRadarOverlay::update_channel_state (const Handle Channel, const Boolean State) {

}


void
dmz::RenderPluginRadarOverlay::receive_axis_event (
      const Handle Channel,
      const InputEventAxis &Value) {

}


void
dmz::RenderPluginRadarOverlay::receive_button_event (
      const Handle Channel,
      const InputEventButton &Value) {

}


void
dmz::RenderPluginRadarOverlay::receive_switch_event (
      const Handle Channel,
      const InputEventSwitch &Value) {

}


void
dmz::RenderPluginRadarOverlay::receive_key_event (
      const Handle Channel,
      const InputEventKey &Value) {

}


void
dmz::RenderPluginRadarOverlay::receive_mouse_event (
      const Handle Channel,
      const InputEventMouse &Value) {

}


void
dmz::RenderPluginRadarOverlay::receive_data_event (
      const Handle Channel,
      const Handle Source,
      const Data &Value) {

}


// Object Observer Interface
void
dmz::RenderPluginRadarOverlay::create_object (
      const UUID &Identity,
      const Handle ObjectHandle,
      const ObjectType &Type,
      const ObjectLocalityEnum Locality) {

   if (_overlay && !_ignoreTypes.contains_type (Type)) {

      ObjectDefStruct *ods = _lookup_def (Type);

      if (ods) {

         ObjectStruct *os = new ObjectStruct;

         if (os) {

            os->switchHandle = _overlay->clone_template (ods->CloneName);

            if (os->switchHandle) {

               os->xformHandle = _overlay->lookup_node_clone_sub_handle (
                  os->switchHandle,
                  ods->XFormName);

               if (os->xformHandle) {

                  if (!_objTable.store (ObjectHandle, os)) { delete os; os = 0; }
               }
               else { delete os; os = 0; }
            }
            else { delete os; os = 0; }
         }
      }
      else { _ignoreTypes.add_object_type (Type); }
   }
}


void
dmz::RenderPluginRadarOverlay::destroy_object (
      const UUID &Identity,
      const Handle ObjectHandle) {

}


void
dmz::RenderPluginRadarOverlay::update_object_state (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Mask &Value,
      const Mask *PreviousValue) {

}


void
dmz::RenderPluginRadarOverlay::update_object_flag (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Boolean Value,
      const Boolean *PreviousValue) {

   if (AttributeHandle == _hilAttrHandle) {

      if (Value) { _hil = ObjectHandle; }
      else if (ObjectHandle == _hil) { _hil = 0; }
   }
}


void
dmz::RenderPluginRadarOverlay::update_object_position (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Value,
      const Vector *PreviousValue) {

   ObjectStruct *os = _objTable.lookup (ObjectHandle);

   if (os) { os->pos = Value; }
}


dmz::RenderPluginRadarOverlay::ObjectDefStruct *
dmz::RenderPluginRadarOverlay::_lookup_def (const ObjectType &Type) {

   ObjectDefStruct *result (_defTable.lookup (Type.get_handle ()));

   if (!result) {

      result = _create_def (Type);

      if (result) { _defTable.store (Type.get_handle (), result); }
   }

   return result;
}


dmz::RenderPluginRadarOverlay::ObjectDefStruct *
dmz::RenderPluginRadarOverlay::_create_def (const ObjectType &Type) {

   ObjectDefStruct *result (0);

   ObjectType current (Type);

   while (current && !result) {

      result = _defMasterTable.lookup (current.get_handle ());

      if (!result) {

         Config data;

         if (Type.get_config ().lookup_config ("render.overlay", data)) {

            result = new ObjectDefStruct (
               config_to_string ("name", data),
               config_to_string ("transform.name", data, "transform"));

            if (result) {

               if (!result->CloneName ||
                     !_defMasterTable.store (current.get_handle (), result)) {

                  if (!result->CloneName) {

                     _log.error << "Overlay missing name in object type: "
                        << current.get_name () << endl;
                  }

                  delete result; result = 0;
               }
            }
         }
      }

      current.become_parent ();
   }

   return result;
}


void
dmz::RenderPluginRadarOverlay::_init (Config &local) {

   activate_default_object_attribute (
      ObjectCreateMask | ObjectDestroyMask | ObjectPositionMask);

   _hilAttrHandle = activate_object_attribute (
      ObjectAttributeHumanInTheLoopName,
      ObjectFlagMask);
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzRenderPluginRadarOverlay (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::RenderPluginRadarOverlay (Info, local);
}

};
