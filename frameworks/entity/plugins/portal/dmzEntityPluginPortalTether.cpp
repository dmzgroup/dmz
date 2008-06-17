#include "dmzEntityPluginPortalTether.h"
#include <dmzInputEventMasks.h>
#include <dmzObjectAttributeMasks.h>
#include <dmzObjectConsts.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigRead.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>

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
      _renderPortal (0),
      _audioPortal (0),
      _active (0),
      _log (Info) {

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

      if (!_renderPortal) { _renderPortal = RenderModulePortal::cast (PluginPtr); }
      if (!_audioPortal) { _audioPortal = AudioModulePortal::cast (PluginPtr); }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_renderPortal && (_renderPortal == RenderModulePortal::cast (PluginPtr))) { 
      
         _renderPortal = 0; 
      }
      if (_audioPortal && (_audioPortal == AudioModulePortal::cast (PluginPtr))) { 

         _audioPortal = 0; 
      }
   }
}


// TimeSlice Interface
void
dmz::EntityPluginPortalTether::update_time_slice (const Float64 TimeDelta) {

   if (_active > 0) {

      ObjectModule *objMod (get_object_module ());

      if (objMod && _handle && _defaultHandle) {

         Vector pos;
         Matrix ori; ori.set_identity ();

         objMod->lookup_position (_handle, _defaultHandle, pos);
         objMod->lookup_orientation (_handle, _defaultHandle, ori);
   
         Vector forward (_offset);
         ori.transform_vector (forward);

         pos += forward;

         if (_renderPortal) { _renderPortal->set_view (pos, ori); }
         if (_audioPortal) { _audioPortal->set_view (pos, ori); }
      }
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

   if (State) { _active++; }
   else { _active--; }
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


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzEntityPluginPortalTether (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::EntityPluginPortalTether (Info, local);
}

};
