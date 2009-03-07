#include "dmzRenderPluginPortalSize.h"
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>

//! \cond
dmz::RenderPluginPortalSize::RenderPluginPortalSize (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      PortalSizeObserver (Info, local),
      _log (Info) {

   _init (local);
}


dmz::RenderPluginPortalSize::~RenderPluginPortalSize () {

}


// Plugin Interface
void
dmz::RenderPluginPortalSize::update_plugin_state (
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
dmz::RenderPluginPortalSize::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

   }
   else if (Mode == PluginDiscoverRemove) {

   }
}


void
dmz::RenderPluginPortalSize::update_portal_size (
      const Handle WindowHandle,
      const Int32 SizeX,
      const Int32 SizeY) {

   _log.error << "Window[" << WindowHandle << "]: " << SizeX << ", " << SizeY << endl;
}


void
dmz::RenderPluginPortalSize::_init (Config &local) {

}
//! \endcond


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzRenderPluginPortalSize (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::RenderPluginPortalSize (Info, local);
}

};
