#ifndef DMZ_RENDER_MODULE_OVERLAY_DOT_H
#define DMZ_RENDER_MODULE_OVERLAY_DOT_H

#include <dmzRuntimePlugin.h>
#include <dmzRuntimeRTTI.h>
#include <dmzTypesBase.h>
#include <dmzTypesString.h>

namespace dmz {

   enum RenderOverlayTypeEnum {
      RenderOverlayUnknown,
      RenderOverlayGroup,
      RenderOverlaySwitch,
      RenderOverlayTransform,
   };

   class RenderModuleOverlay {

      public:
         static RenderModuleOverlay *cast (
            const Plugin *PluginPtr,
            const String &PluginName = "");

         String get_render_module_overlay_name () const;
         Handle get_render_module_overlay_handle () const;

         // RenderModuleOverlay Interface
         virtual Handle lookup_overlay_handle (const String &Name) = 0;
         virtual String lookup_overlay_name (const Handle Overlay) = 0;
         virtual RenderOverlayTypeEnum lookup_overlay_type (const Handle Overlay) = 0;

         virtual Boolean is_of_overlay_type (
            const Handle Overlay,
            const RenderOverlayTypeEnum Type) = 0;

         // Overlay Group API

         // Overlay Switch API
         virtual Boolean store_overlay_switch_state (
            const Handle Overlay,
            const Int32 Which,
            const Boolean SwitchState) = 0;

         virtual Boolean store_overlay_all_switch_state (
            const Handle Overlay,
            const Boolean SwitchState) = 0;

         virtual Boolean enable_overlay_single_switch_state (
            const Handle Overlay,
            const Int32 Which) = 0;

         virtual Boolean lookup_overlay_switch_state (
            const Handle Overlay,
            const Int32 Which) = 0;

         virtual Int32 lookup_overlay_switch_count (const Handle Overlay) = 0;

         // Overlay Transform API
         virtual Boolean store_overlay_position (
            const Handle Overlay,
            const Float64 ValueX,
            const Float64 ValueY) = 0;

         virtual Boolean lookup_overlay_position (
            const Handle Overlay,
            Float64 &valueX,
            Float64 &valueY) = 0;

         virtual Boolean store_overlay_rotation (
            const Handle Overlay,
            const Float64 Value) = 0;

         virtual Boolean lookup_overlay_rotation (
            const Handle Overlay,
            Float64 &value) = 0;

         virtual Boolean store_overlay_scale (
            const Handle Overlay,
            const Float64 ValueX,
            const Float64 ValueY) = 0;

         virtual Boolean lookup_overlay_scale (
            const Handle Overlay,
            Float64 &valueX,
            Float64 &valueY) = 0;

      protected:
         RenderModuleOverlay (const PluginInfo &Info);
         ~RenderModuleOverlay ();

      private:
         RenderModuleOverlay ();
         RenderModuleOverlay (const RenderModuleOverlay &);
         RenderModuleOverlay &operator= (const RenderModuleOverlay &);

         const PluginInfo &__Info;
   };

   //! \cond
   const char RenderModuleOverlayInterfaceName[] = "RenderModuleOverlayInterface";
   //! \endcond
};


inline dmz::RenderModuleOverlay *
dmz::RenderModuleOverlay::cast (const Plugin *PluginPtr, const String &PluginName) {

   return (RenderModuleOverlay *)lookup_rtti_interface (
      RenderModuleOverlayInterfaceName,
      PluginName,
      PluginPtr);
}


inline
dmz::RenderModuleOverlay::RenderModuleOverlay (const PluginInfo &Info) :
      __Info (Info) {

   store_rtti_interface (RenderModuleOverlayInterfaceName, __Info, (void *)this);
}


inline
dmz::RenderModuleOverlay::~RenderModuleOverlay () {

   remove_rtti_interface (RenderModuleOverlayInterfaceName, __Info);
}


inline dmz::String
dmz::RenderModuleOverlay::get_render_module_overlay_name () const {

   return __Info.get_name ();
}


inline dmz::Handle
dmz::RenderModuleOverlay::get_render_module_overlay_handle () const {

   return __Info.get_handle ();
}

#endif // DMZ_RENDER_MODULE_OVERLAY_DOT_H
