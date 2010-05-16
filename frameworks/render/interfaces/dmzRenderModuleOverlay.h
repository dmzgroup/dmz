#ifndef DMZ_RENDER_MODULE_OVERLAY_DOT_H
#define DMZ_RENDER_MODULE_OVERLAY_DOT_H

#include <dmzRuntimePlugin.h>
#include <dmzRuntimeRTTI.h>
#include <dmzTypesBase.h>
#include <dmzTypesString.h>

namespace dmz {

   //! Overlay node type enumerations.
   //! \ingroup Render
   enum RenderOverlayTypeEnum {
      RenderOverlayUnknown, //!< Unknown node type.
      RenderOverlayNode, //!< Basic node.
      RenderOverlayText, //!< Text node.
      RenderOverlayBox, //!< Box node.
      RenderOverlayGroup, //!< Group node.
      RenderOverlaySwitch, //!< Switch node.
      RenderOverlayTransform, //!< Transform node.
   };

   class RenderModuleOverlay {

      public:
         static RenderModuleOverlay *cast (
            const Plugin *PluginPtr,
            const String &PluginName = "");

         String get_render_module_overlay_name () const;
         Handle get_render_module_overlay_handle () const;

         // RenderModuleOverlay Interface
         virtual Boolean lookup_named_color (
            const String &Name,
            Float64 &red,
            Float64 &green,
            Float64 &blue,
            Float64 &alpha) = 0;

         virtual Handle lookup_node_handle (const String &Name) = 0;

         virtual Handle lookup_node_clone_sub_handle (
            const Handle CloneHandle,
            const String &Name) = 0;

         virtual String lookup_node_name (const Handle Overlay) = 0;
         virtual RenderOverlayTypeEnum lookup_node_type (const Handle Overlay) = 0;

         virtual Boolean is_of_node_type (
            const Handle Overlay,
            const RenderOverlayTypeEnum Type) = 0;

         virtual Handle clone_template (const String &Name) = 0;

         virtual Boolean destroy_node (const Handle Overlay) = 0;

         // Overlay Node API
         virtual Boolean store_color (
            const Handle Overlay,
            const Float64 Red,
            const Float64 Green,
            const Float64 Blue,
            const Float64 Alpha) = 0;

         // Overlay Text API
         virtual Boolean store_text (const Handle Overlay, const String &Value) = 0;
         virtual Boolean lookup_text (const Handle Overlay, String &value) = 0;

         // Overlay Group API
         virtual Boolean add_group_child (const Handle Parent, const Handle Child) = 0;
         virtual Boolean remove_group_child (const Handle Parent, const Handle Child) = 0;

         virtual Int32 lookup_group_child_count (const Handle Overlay) = 0;

         // Overlay Switch API
         virtual Boolean store_switch_state (
            const Handle Overlay,
            const Int32 Which,
            const Boolean SwitchState) = 0;

         virtual Boolean store_switch_state_all (
            const Handle Overlay,
            const Boolean SwitchState) = 0;

         virtual Boolean enable_switch_state_single (
            const Handle Overlay,
            const Int32 Which) = 0;

         virtual Boolean lookup_switch_state (
            const Handle Overlay,
            const Int32 Which) = 0;

         // Overlay Transform API
         virtual Boolean store_transform_position (
            const Handle Overlay,
            const Float64 ValueX,
            const Float64 ValueY) = 0;

         virtual Boolean lookup_transform_position (
            const Handle Overlay,
            Float64 &valueX,
            Float64 &valueY) = 0;

         virtual Boolean store_transform_rotation (
            const Handle Overlay,
            const Float64 Value) = 0;

         virtual Boolean lookup_transform_rotation (
            const Handle Overlay,
            Float64 &value) = 0;

         virtual Boolean store_transform_scale (
            const Handle Overlay,
            const Float64 ValueX,
            const Float64 ValueY) = 0;

         virtual Boolean lookup_transform_scale (
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
