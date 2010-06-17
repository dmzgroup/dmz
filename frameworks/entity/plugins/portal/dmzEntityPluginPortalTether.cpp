#include <dmzEntityModulePortal.h>
#include "dmzEntityPluginPortalTether.h"
#include <dmzInputEventMasks.h>
#include <dmzObjectAttributeMasks.h>
#include <dmzObjectConsts.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeConfigToVector.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>

/*!

\class dmz::EntityPluginPortalTether
\ingroup Entity
\brief Tethers the portal to the human-in-the-loop object.
\details This plugin uses the dmz::EntityModulePortal interface to update the portal view.

*/

//! \cond
dmz::EntityPluginPortalTether::EntityPluginPortalTether (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      TimeSlice (Info),
      ObjectObserverUtil (Info, local),
      InputObserverUtil (Info, local),
      _handle (0),
      _defaultHandle (0),
      _hilHandle (0),
      _offset (0.0, 2.5, 10.0),
      _portal (0),
      _active (0),
      _log (Info) {

   stop_time_slice ();

   _init (local);
}


dmz::EntityPluginPortalTether::~EntityPluginPortalTether () {

}


// Plugin Interface
void
dmz::EntityPluginPortalTether::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      if (!_portal) { _portal = EntityModulePortal::cast (PluginPtr); }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_portal && (_portal == EntityModulePortal::cast (PluginPtr))) { 

         _portal = 0; 
      }
   }
}


// TimeSlice Interface
void
dmz::EntityPluginPortalTether::update_time_slice (const Float64 TimeDelta) {

   ObjectModule *objMod (get_object_module ());

   if (objMod && _handle && _defaultHandle) {

      Vector pos, vel;
      Matrix ori; ori.set_identity ();

      objMod->lookup_position (_handle, _defaultHandle, pos);
      objMod->lookup_orientation (_handle, _defaultHandle, ori);
      objMod->lookup_velocity (_handle, _defaultHandle, vel);

      Vector forward (_offset);
      ori.transform_vector (forward);

      pos += forward;

      if (_portal) { _portal->set_view (pos, ori, vel); }
   }
}


void
dmz::EntityPluginPortalTether::update_object_flag (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Boolean Value,
      const Boolean *PreviousValue) {

   if ((AttributeHandle == _hilHandle) && Value) { _handle = ObjectHandle; }
}


// Input Observer Interface
void
dmz::EntityPluginPortalTether::update_channel_state (
      const Handle Channel,
      const Boolean State) {

   _active += State ? 1 : -1;

   if (_active == 1) { start_time_slice (); }
   else if (_active == 0) { stop_time_slice (); }
}



void
dmz::EntityPluginPortalTether::_init (Config &local) {

   Definitions defs (get_plugin_runtime_context (), &_log);

   _defaultHandle = defs.create_named_handle (ObjectAttributeDefaultName);

   _hilHandle = activate_object_attribute (
      ObjectAttributeHumanInTheLoopName,
      ObjectFlagMask);

   Boolean foundOffset (False);

   Config cameraParams;

   if (local.lookup_all_config ("offset", cameraParams)) {

      ConfigIterator it;
      Config cd;

      Boolean found (cameraParams.get_first_config (it, cd));

      if (found)  {

         _offset = config_to_vector (cd, _offset);
         foundOffset = True;
      }
   }

   init_input_channels (local, InputEventChannelStateMask, &_log);

   _log.info << "Using " << (foundOffset ? "" : "default ") << "camera offset: "
      << _offset << endl;
}
//! \endcond


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzEntityPluginPortalTether (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::EntityPluginPortalTether (Info, local);
}

};
