#include <dmzEntityModulePortal.h>
#include "dmzEntityPluginPortalArticulate.h"
#include <dmzEventConsts.h>
#include <dmzInputEventMasks.h>
#include <dmzInputEventController.h>
#include <dmzObjectAttributeMasks.h>
#include <dmzObjectConsts.h>
#include <dmzObjectModule.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeConfigToVector.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>

/*!

\class dmz::EntityPluginPortalArticulate
\ingroup Entity
\brief First-person view of human-in-th-loop object with the render and audio portals.
\details
\code
<dmz>
<dmzEntityPluginPortalArticulate>
   <transform>
      <component name="Attribute Name" axis="Axis of Rotation" x="X Offset" y="Y Offset" z="Z Offset"/>
   </transform>
</dmzEntityPluginPortalArticulate>
</dmz>
\endcode

*/

//! \cond
dmz::EntityPluginPortalArticulate::EntityPluginPortalArticulate (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      TimeSlice (Info),
      InputObserverUtil (Info, local),
      ObjectObserverUtil (Info, local),
      _log (Info),
      _portal (0),
      _transform (0),
      _active (0),
      _defaultAttrHandle (0),
      _hilAttrHandle (0),
      _hil (0) {

   stop_time_slice ();

   _init (local);
}


dmz::EntityPluginPortalArticulate::~EntityPluginPortalArticulate () {

   _componentTable.clear ();

   if (_transform) { delete _transform; _transform = 0; }
}


// Plugin Interface
void
dmz::EntityPluginPortalArticulate::update_plugin_state (
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
dmz::EntityPluginPortalArticulate::discover_plugin (
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
dmz::EntityPluginPortalArticulate::update_time_slice (const Float64 TimeDelta) {

   ObjectModule *objMod (get_object_module ());

   if (_hil && objMod) {

      Vector pos, vel;
      Matrix ori;
      objMod->lookup_position (_hil, _defaultAttrHandle, pos);
      objMod->lookup_velocity (_hil, _defaultAttrHandle, vel);
      objMod->lookup_orientation (_hil, _defaultAttrHandle, ori);

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
      ori = ori * rot;

      if (_portal) { _portal->set_view (pos, ori, vel); }
   }
}


// Input Observer Interface
void
dmz::EntityPluginPortalArticulate::update_channel_state (
      const Handle Channel,
      const Boolean State) {

   _active += (State ? 1 : -1);

   if (_active == 1) { start_time_slice (); }
   else if (_active == 0) { stop_time_slice (); }
}


// Object Observer Interface
void
dmz::EntityPluginPortalArticulate::update_object_flag (
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
dmz::EntityPluginPortalArticulate::update_object_scalar (
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


void
dmz::EntityPluginPortalArticulate::_init (Config &local) {

   RuntimeContext *context (get_plugin_runtime_context ());

   Definitions defs (context, &_log);

   _defaultAttrHandle = defs.create_named_handle (ObjectAttributeDefaultName);

   _hilAttrHandle = activate_object_attribute (
      ObjectAttributeHumanInTheLoopName,
      ObjectFlagMask);

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

   init_input_channels (local, InputEventChannelStateMask, &_log);
}
//! \endcond


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzEntityPluginPortalArticulate (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::EntityPluginPortalArticulate (Info, local);
}

};
