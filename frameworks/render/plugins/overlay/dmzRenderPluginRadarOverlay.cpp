#include <dmzInputEventMasks.h>
#include <dmzInputEventKey.h>
#include <dmzObjectAttributeMasks.h>
#include <dmzObjectConsts.h>
#include <dmzObjectModule.h>
#include <dmzRenderModuleOverlay.h>
#include <dmzRenderModulePortal.h>
#include "dmzRenderPluginRadarOverlay.h"
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzTypesMath.h>
#include <dmzTypesMatrix.h>
#include <dmzTypesVector.h>

dmz::RenderPluginRadarOverlay::RenderPluginRadarOverlay (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      TimeSlice (Info),
      InputObserverUtil (Info, local),
      ObjectObserverUtil (Info, local),
      _log (Info),
      _overlay (0),
      _portal (0),
      _rootName ("radar"),
      _root (0),
      _defaultAttrHandle (0),
      _hilAttrHandle (0),
      _hil (0),
      _radius (64.0),
      _scale (0.064),
      _scaleMin (0.0001),
      _scaleMax (1.0e100),
      _scaleRate (1.0),
      _scaleCount (0) {

   _init (local);
}


dmz::RenderPluginRadarOverlay::~RenderPluginRadarOverlay () {

   _defTable.clear ();
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

      if (!_overlay) {

         _overlay = RenderModuleOverlay::cast (PluginPtr);

         if (_overlay) {

            _root = _overlay->lookup_node_handle (_rootName);
         }
      }

      if (!_portal) { _portal = RenderModulePortal::cast (PluginPtr); }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_overlay && (_overlay == RenderModuleOverlay::cast (PluginPtr))) {

         _overlay = 0;
         _root = 0;
      }

      if (_portal && (_portal == RenderModulePortal::cast (PluginPtr))) { _portal = 0; }
   }
}


// TimeSlice Interface
void
dmz::RenderPluginRadarOverlay::update_time_slice (const Float64 TimeDelta) {

   if (_overlay && _root && _hil) {

      if (_scaleCount > 0) { _scale += _scaleRate * TimeDelta; }
      else if (_scaleCount < 0) { _scale -= _scaleRate * TimeDelta; }

      if (_scale < _scaleMin) { _scale = _scaleMin; }
      else if (_scale > _scaleMax) { _scale = _scaleMax; }

      Vector hilPos;
      Matrix hilOri;

      ObjectModule *objMod (get_object_module ());

      if (objMod) {

         objMod->lookup_position (_hil, _defaultAttrHandle, hilPos);

         if (_portal) {

            Vector vec;
            _portal->get_view (vec, hilOri);
         }
         else { objMod->lookup_orientation (_hil, _defaultAttrHandle, hilOri); }
      }

      const Float64 Heading = get_heading (hilOri);
      const Matrix XForm (Vector (0.0, 1.0, 0.0), Heading);

      HashTableHandleIterator it;
      ObjectStruct *os (0);

      while (_objTable.get_next (it, os)) {

         if (it.get_hash_key () == _hil) { _set_visiblity (False, *os); }
         else {

            Vector pos = (os->pos - hilPos) * _scale;
            pos.set_y (0.0);
            XForm.transform_vector (pos);

            if (pos.magnitude () <= _radius) { _set_visiblity (True, *os); }
            else { _set_visiblity (False, *os); }

            _overlay->store_transform_position (
               os->xformHandle,
               pos.get_x (),
               -pos.get_z ());
         }
      }

//      _overlay->store_transform_rotation (_root, heading);
   }
}


// Input Observer Interface
void
dmz::RenderPluginRadarOverlay::update_channel_state (
      const Handle Channel,
      const Boolean State) {

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

   const UInt32 Key = Value.get_key ();

   if ((Key == '-') || (Key == '_')) {

      _scaleCount += (Value.get_key_state () ? 1 : -1);
   }
   else if ((Key == '+') || (Key == '=')) {

      _scaleCount += (Value.get_key_state () ? -1 : 1);
   }
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

   if (_overlay && _root && !_ignoreTypes.contains_type (Type)) {

      ObjectDefStruct *ods = _lookup_def (Type);

      if (ods) {

         ObjectStruct *os = new ObjectStruct;

         if (os) {

            os->model = _overlay->clone_template (ods->CloneName);

            if (os->model) {

               os->switchHandle = _overlay->lookup_node_clone_sub_handle (
                  os->model,
                  ods->SwitchName);

               os->xformHandle = _overlay->lookup_node_clone_sub_handle (
                  os->model,
                  ods->XFormName);

               if (os->switchHandle && os->xformHandle) {

                  if (!_objTable.store (ObjectHandle, os)) { delete os; os = 0; }
                  else {

                     _overlay->store_switch_state_all (os->switchHandle, False);
                     _overlay->add_group_child (_root, os->model);
                  }
               }
               else {

                  _overlay->destroy_node (os->model);
                  delete os; os = 0;
               }
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

   ObjectStruct *os (_objTable.remove (ObjectHandle));

   if (os) {

      if (_overlay) { _overlay->destroy_node (os->switchHandle); }

      delete os; os = 0;
   }
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


void
dmz::RenderPluginRadarOverlay::_set_visiblity (const Boolean Value, ObjectStruct &os) {

   if (_overlay) {

      if (Value != os.visible) {

         _overlay->store_switch_state_all (os.switchHandle, Value);
         os.visible = Value;
      }
   }
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

         if (current.get_config ().lookup_config ("render.overlay", data)) {

            result = new ObjectDefStruct (
               config_to_string ("name", data),
               config_to_string ("switch.name", data, "switch"),
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

   init_input_channels (local, InputEventKeyMask);

   _rootName = config_to_string ("root.name", local, _rootName);

   _defaultAttrHandle = activate_default_object_attribute (
      ObjectCreateMask | ObjectDestroyMask | ObjectPositionMask);

   _hilAttrHandle = activate_object_attribute (
      ObjectAttributeHumanInTheLoopName,
      ObjectFlagMask);

   _radius = config_to_float64 ("radius.value", local, _radius);
   _scale = config_to_float64 ("scale.value", local, _scale);
   _scaleMin = config_to_float64 ("scale.min", local, _scaleMin);
   _scaleMax = config_to_float64 ("scale.max", local, _scaleMax);
   _scaleRate = config_to_float64 ("scale.rate", local, _scaleRate);
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
