#ifndef DMZ_RENDER_MODULE_OVERLAY_OSG_DOT_H
#define DMZ_RENDER_MODULE_OVERLAY_OSG_DOT_H

#include <dmzRenderModuleOverlay.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>

namespace dmz {

   class RenderModuleOverlayOSG :
         public Plugin,
         public RenderModuleOverlay {

      public:
         RenderModuleOverlayOSG (const PluginInfo &Info, Config &local);
         ~RenderModuleOverlayOSG ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level);

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // RenderModuleOverlay Interface
         virtual Handle lookup_overlay_handle (const String &Name);
         virtual String lookup_overlay_name (const Handle Overlay);
         virtual RenderOverlayTypeEnum lookup_overlay_type (const Handle Overlay);

         virtual Boolean is_of_overlay_type (
            const Handle Overlay,
            const RenderOverlayTypeEnum Type);

         // Overlay Group API

         // Overlay Switch API
         virtual Boolean store_overlay_switch_state (
            const Handle Overlay,
            const Int32 Which,
            const Boolean SwitchState);

         virtual Boolean store_overlay_all_switch_state (
            const Handle Overlay,
            const Boolean SwitchState);

         virtual Boolean enable_overlay_single_switch_state (
            const Handle Overlay,
            const Int32 Which);

         virtual Boolean lookup_overlay_switch_state (
            const Handle Overlay,
            const Int32 Which);

         virtual Int32 lookup_overlay_switch_count (const Handle Overlay);

         // Overlay Transform API
         virtual Boolean store_overlay_position (
            const Handle Overlay,
            const Float64 ValueX,
            const Float64 ValueY);

         virtual Boolean lookup_overlay_position (
            const Handle Overlay,
            Float64 &valueX,
            Float64 &valueY);

         virtual Boolean store_overlay_rotation (
            const Handle Overlay,
            const Float64 Value);

         virtual Boolean lookup_overlay_rotation (
            const Handle Overlay,
            Float64 &value);

         virtual Boolean store_overlay_scale (
            const Handle Overlay,
            const Float64 ValueX,
            const Float64 ValueY);

         virtual Boolean lookup_overlay_scale (
            const Handle Overlay,
            Float64 &valueX,
            Float64 &valueY);

      protected:
         void _init (Config &local);

         Log _log;

      private:
         RenderModuleOverlayOSG ();
         RenderModuleOverlayOSG (const RenderModuleOverlayOSG &);
         RenderModuleOverlayOSG &operator= (const RenderModuleOverlayOSG &);

   };
};

#endif // DMZ_RENDER_MODULE_OVERLAY_OSG_DOT_H
