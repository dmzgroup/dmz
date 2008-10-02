#include <dmzInputEventKey.h>
#include <dmzInputEventMasks.h>
#include <dmzRenderModuleCoreOgre.h>
#include "dmzRenderPluginModeOgre.h"
#include <dmzRenderUtilOgre.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>

#include <Ogre/Ogre.h>

dmz::RenderPluginModeOgre::RenderPluginModeOgre (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      InputObserverUtil (Info, local),
      _name (DefaultPortalNameOgre),
      _core (0),
      _camera (0),
      _log (Info) {

   _init (local);
}


dmz::RenderPluginModeOgre::~RenderPluginModeOgre () {

}


// Plugin Interface
void
dmz::RenderPluginModeOgre::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      if (!_core) {

         _core = RenderModuleCoreOgre::cast (PluginPtr);

         if (_core) {

            _camera = _core->lookup_camera (_name);
         }
      }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_core && (_core == RenderModuleCoreOgre::cast (PluginPtr))) {

         _camera = 0;
         _core = 0;
      }
   }
}


// Input Observer Interface
void
dmz::RenderPluginModeOgre::update_channel_state (
      const UInt32 Channel,
      const Boolean State) {

}


void
dmz::RenderPluginModeOgre::receive_key_event (
      const UInt32 Channel,
      const InputEventKey &Value) {

   if (Value.get_key_state () && _camera) {

      if (Value.get_key () == KeyF1) {

         _camera->setPolygonMode(Ogre::PM_SOLID);
      }
      else if (Value.get_key () == KeyF2) {

         _camera->setPolygonMode(Ogre::PM_WIREFRAME);
      }
   }
}


void
dmz::RenderPluginModeOgre::_init (Config &local) {

   activate_default_input_channel (InputEventKeyMask);

   _name = config_to_string ("portal.name", local, _name);
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzRenderPluginModeOgre (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::RenderPluginModeOgre (Info, local);
}

};
