#include <dmzAudioModulePortal.h>
#include "dmzEntityModulePortalBasic.h"
#include <dmzRenderModulePortal.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>

/*!

\class dmz::EntityModulePortalBasic
\ingroup Entity
\brief Provides basic implementation of the dmz::EntityModulePortal interface.

*/

//! \cond
dmz::EntityModulePortalBasic::EntityModulePortalBasic (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      EntityModulePortal (Info),
      _log (Info),
      _audio (0),
      _render (0) {

   _init (local);
}


dmz::EntityModulePortalBasic::~EntityModulePortalBasic () {

}


// Plugin Interface
void
dmz::EntityModulePortalBasic::update_plugin_state (
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
dmz::EntityModulePortalBasic::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      if (!_audio) { _audio = AudioModulePortal::cast (PluginPtr); }
      if (!_render) { _render = RenderModulePortal::cast (PluginPtr); }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_audio && (_audio == AudioModulePortal::cast (PluginPtr))) { _audio = 0; }
      if (_render && (_render == RenderModulePortal::cast (PluginPtr))) { _render = 0; }
   }
}


// EntityModulePortal Interface
void
dmz::EntityModulePortalBasic::set_view (
            const Vector &Position,
            const Matrix &Orientation,
            const Vector &Velocity) {

   if (_audio) { _audio->set_view (Position, Orientation, Velocity); }
   if (_render) { _render->set_view (Position, Orientation); }

   _position = Position;
   _orientation = Orientation;
   _velocity = Velocity;
}


void
dmz::EntityModulePortalBasic::get_view (
            Vector &position,
            Matrix &orientation,
            Vector &velocity) {

   if (_render) {

      _render->get_view (position, orientation);
      velocity = _velocity;
   }
   else if (_audio) { _audio->get_view (position, orientation, velocity); }
   else {

      position = _position;
      orientation = _orientation;
      velocity = _velocity;
   }
}


void
dmz::EntityModulePortalBasic::_init (Config &local) {

}
//! \endcond


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzEntityModulePortalBasic (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::EntityModulePortalBasic (Info, local);
}

};
