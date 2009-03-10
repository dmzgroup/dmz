#include <dmzRenderConsts.h>
#include <dmzRenderModuleCoreOSG.h>
#include "dmzRenderPluginPickOSG.h"
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>

#include <osgUtil/IntersectVisitor>

dmz::RenderPluginPickOSG::RenderPluginPickOSG (const PluginInfo &Info, Config &local) :
      Plugin (Info),
      RenderPickUtil (Info, local),
      _log (Info),
      _core (0),
      _viewerName (RenderMainPortalName) {

   _init (local);
}


dmz::RenderPluginPickOSG::~RenderPluginPickOSG () {

}


// Plugin Interface
void
dmz::RenderPluginPickOSG::update_plugin_state (
      const PluginStateEnum State,
      const UInt32 Level) {

   if (State == PluginStateInit) {

   }
   else if (State == PluginStateStart) {

      if (!_viewer.valid () && _core) {

         _viewer = _core->lookup_viewer (_viewerName);
      }
   }
   else if (State == PluginStateStop) {

      if (_viewer.valid ()) { _viewer = 0; }
   }
   else if (State == PluginStateShutdown) {

   }
}


void
dmz::RenderPluginPickOSG::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      if (!_core) { _core = dmz::RenderModuleCoreOSG::cast (PluginPtr); }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_core && (_core == dmz::RenderModuleCoreOSG::cast (PluginPtr))) { _core = 0; }
   }
}


// RenderPickUtil Interface
dmz::Boolean
dmz::RenderPluginPickOSG::screen_to_world (
      const Int32 ScreenPosX,
      const Int32 ScreenPosY,
      Vector &worldPosition,
      Handle &objectHandle) {

   Boolean result (False);

   return result;
}


dmz::Boolean
dmz::RenderPluginPickOSG::world_to_screen (
      const Vector &WorldPosition,
      Int32 &screenPosX,
      Int32 &screenPosY) {

   Boolean result (False);

   return result;
}


dmz::Boolean
dmz::RenderPluginPickOSG::source_to_world (
      const Int32 SourcePosX,
      const Int32 SourcePosY,
      Vector &worldPosition,
      Handle &objectHandle) {

   Boolean result (False);

   if (_core && _viewer.valid ()) {

   }

   return result;
}


dmz::Boolean
dmz::RenderPluginPickOSG::world_to_source (
      const Vector &WorldPosition,
      Int32 &sourcePosX,
      Int32 &sourcePosY) {

   Boolean result (False);

   return result;
}


void
dmz::RenderPluginPickOSG::_init (Config &local) {

   _viewerName = config_to_string ("portal.name", local, _viewerName);
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzRenderPluginPickOSG (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::RenderPluginPickOSG (Info, local);
}

};
