#include "dmzRenderModuleOverlayOSG.h"
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>

dmz::RenderModuleOverlayOSG::RenderModuleOverlayOSG (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      RenderModuleOverlay (Info),
      _log (Info) {

   _init (local);
}


dmz::RenderModuleOverlayOSG::~RenderModuleOverlayOSG () {

}


// Plugin Interface
void
dmz::RenderModuleOverlayOSG::update_plugin_state (
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
dmz::RenderModuleOverlayOSG::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

   }
   else if (Mode == PluginDiscoverRemove) {

   }
}


// RenderModuleOverlay Interface
dmz::Handle
dmz::RenderModuleOverlayOSG::lookup_overlay_handle (const String &Name) {

   Handle result (0);

   return result;
}


dmz::String
dmz::RenderModuleOverlayOSG::lookup_overlay_name (const Handle Overlay) {

   String result;

   return result;
}


dmz::RenderOverlayTypeEnum
dmz::RenderModuleOverlayOSG::lookup_overlay_type (const Handle Overlay) {

   RenderOverlayTypeEnum result (RenderOverlayUnknown);

   return result;
}


dmz::Boolean
dmz::RenderModuleOverlayOSG::is_of_overlay_type (
      const Handle Overlay,
      const RenderOverlayTypeEnum Type) {

   Boolean result (False);

   return result;
}


// Overlay Switch API
dmz::Boolean
dmz::RenderModuleOverlayOSG::store_overlay_switch_state (
      const Handle Overlay,
      const Int32 Which,
      const Boolean SwitchState) {

   Boolean result (False);

   return result;
}


dmz::Boolean
dmz::RenderModuleOverlayOSG::store_overlay_all_switch_state (
      const Handle Overlay,
      const Boolean SwitchState) {

   Boolean result (False);

   return result;
}


dmz::Boolean
dmz::RenderModuleOverlayOSG::enable_overlay_single_switch_state (
      const Handle Overlay,
      const Int32 Which) {

   Boolean result (False);

   return result;
}


dmz::Boolean
dmz::RenderModuleOverlayOSG::lookup_overlay_switch_state (
      const Handle Overlay,
      const Int32 Which) {

   Boolean result (False);

   return result;
}


dmz::Int32
dmz::RenderModuleOverlayOSG::lookup_overlay_switch_count (const Handle Overlay) {

   Int32 result (0);

   return result;
}


// Overlay Transform API
dmz::Boolean
dmz::RenderModuleOverlayOSG::store_overlay_position (
      const Handle Overlay,
      const Float64 ValueX,
      const Float64 ValueY) {

   Boolean result (False);

   return result;
}


dmz::Boolean
dmz::RenderModuleOverlayOSG::lookup_overlay_position (
      const Handle Overlay,
      Float64 &valueX,
      Float64 &valueY) {

   Boolean result (False);

   return result;
}


dmz::Boolean
dmz::RenderModuleOverlayOSG::store_overlay_rotation (
      const Handle Overlay,
      const Float64 Value) {

   Boolean result (False);

   return result;
}


dmz::Boolean
dmz::RenderModuleOverlayOSG::lookup_overlay_rotation (
      const Handle Overlay,
      Float64 &value) {

   Boolean result (False);

   return result;
}


dmz::Boolean
dmz::RenderModuleOverlayOSG::store_overlay_scale (
      const Handle Overlay,
      const Float64 ValueX,
      const Float64 ValueY) {

   Boolean result (False);

   return result;
}


dmz::Boolean
dmz::RenderModuleOverlayOSG::lookup_overlay_scale (
      const Handle Overlay,
      Float64 &valueX,
      Float64 &valueY) {

   Boolean result (False);

   return result;
}


void
dmz::RenderModuleOverlayOSG::_init (Config &local) {

}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzRenderModuleOverlayOSG (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::RenderModuleOverlayOSG (Info, local);
}

};
