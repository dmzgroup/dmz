#include "dmzEntityPluginPortalFollow3D.h"
#include <dmzInputEventMasks.h>
#include <dmzObjectAttributeMasks.h>
#include <dmzObjectConsts.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeConfigToVector.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzTypesBase.h>

#include <math.h>

/*!

\class dmz::EntityPluginPortalFollow3D
\ingroup Entity
\brief Follows human-in-th-loop object.
\details This plugin uses the dmz::EntityModulePortal interface to update the portal view.

*/


//! \cond
dmz::EntityPluginPortalFollow3D::EntityPluginPortalFollow3D (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      TimeSlice (Info),
      ObjectObserverUtil (Info, local),
      InputObserverUtil (Info, local),
      _HistoryCount (config_to_int32 ("history-count.value", local, 10)),
      _hil (0),
      _defaultHandle (0),
      _hilAttrHandle (0),
      _history (0),
      _which (0),
      _offset (0.0, 2.5, 10.0),
      _portal (0),
      _active (0),
      _log (Info) {

   stop_time_slice ();

   _init (local);
}


dmz::EntityPluginPortalFollow3D::~EntityPluginPortalFollow3D () {

   if (_history) { delete []_history; _history = 0; }
}


// Plugin Interface
void
dmz::EntityPluginPortalFollow3D::discover_plugin (
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
dmz::EntityPluginPortalFollow3D::update_time_slice (const Float64 TimeDelta) {

   ObjectModule *objMod (get_object_module ());

   if (_history && objMod && _hil && _defaultHandle) {

      Vector pos, vel, tmp;
      Matrix ori, cmat (_history[_which]);

      objMod->lookup_position (_hil, _defaultHandle, pos);
      objMod->lookup_orientation (_hil, _defaultHandle, ori);
      objMod->lookup_velocity (_hil, _defaultHandle, vel);

      _history[_which] = ori;
      _which++;
      if (_which >= _HistoryCount) { _which = 0; }

      Vector value (_offset);
      pos += cmat.transform_vector (value);

      if (_portal) { _portal->set_view (pos, cmat, vel); }
   }
}


void
dmz::EntityPluginPortalFollow3D::update_object_flag (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Boolean Value,
      const Boolean *PreviousValue) {

   if ((AttributeHandle == _hilAttrHandle) && Value) { _hil = ObjectHandle; }
}


// Input Observer Interface
void
dmz::EntityPluginPortalFollow3D::update_channel_state (
      const Handle Channel,
      const Boolean State) {

   _active += State ? 1 : -1;

   if (_active == 1) {

      if (_history) { delete []_history; _history = 0; }
      _history = new Matrix[_HistoryCount > 0 ? _HistoryCount : 1];
      _which = 0;

      ObjectModule *module = get_object_module ();

      if (module && _hilAttrHandle) {

         Matrix ori;

         module->lookup_orientation (_hil, _defaultHandle, ori);

         for (Int32 ix = 0; ix < _HistoryCount; ix++) { _history[ix] = ori; }
      }

      start_time_slice ();
   }
   else if (_active == 0) { stop_time_slice (); }
}


void
dmz::EntityPluginPortalFollow3D::_init (Config &local) {

   Definitions defs (get_plugin_runtime_context (), &_log);

   _defaultHandle = defs.create_named_handle (ObjectAttributeDefaultName);

   _hilAttrHandle = activate_object_attribute (
      ObjectAttributeHumanInTheLoopName,
      ObjectFlagMask);

   Boolean foundOffset (False);

   Config cameraParams;

   if (local.lookup_config ("offset", cameraParams)) {

      foundOffset = True;

      _offset = config_to_vector (cameraParams, _offset);
   }

   init_input_channels (local, InputEventChannelStateMask, &_log);

   _log.info << "Using " << (foundOffset ? "" : "default ") << "camera offset: "
      << _offset << endl;
}
//! \endcond


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzEntityPluginPortalFollow3D (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::EntityPluginPortalFollow3D (Info, local);
}

};
