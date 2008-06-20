#ifndef DMZ_RENDER_PICK_2D_DOT_H
#define DMZ_RENDER_PICK_2D_DOT_H

#include <dmzRuntimePlugin.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzRuntimeRTTI.h>
#include <dmzTypesBase.h>
#include <dmzTypesString.h>

#define DMZ_RENDER_PICK_2D_INTERFACE_NAME "RenderPick2dInteface"

namespace dmz {

   class RenderModulePick;
   class Vector;

   class RenderPick2d {

      public:
         static RenderPick2d *cast (
            const Plugin *PluginPtr,
            const String &PluginName = "");

         static Boolean is_valid (const Handle Pick2dHandle, RuntimeContext *context);

         String get_pick_2d_name () const;
         Handle get_pick_2d_handle () const;

         virtual void store_render_module_pick (
            const String &Name,
            RenderModulePick &module) = 0;

         virtual void remove_render_module_pick (
            const String &Name,
            RenderModulePick &module) = 0;

         virtual Boolean screen_to_world (
            const Int32 ScreenPosX,
            const Int32 ScreenPosY,
            Vector &worldPosition,
            Handle &objectHandle) = 0;

         virtual Boolean world_to_screen (
            const Vector &WorldPosition,
            Int32 &screenPosX,
            Int32 &screenPosY) = 0;

         virtual Boolean source_to_world (
            const Int32 SourcePosX,
            const Int32 SourcePosY,
            Vector &worldPosition,
            Handle &objectHandle) = 0;

         virtual Boolean world_to_source (
            const Vector &WorldPosition,
            Int32 &sourcePosX,
            Int32 &sourcePosY) = 0;

      protected:
         RenderPick2d (const PluginInfo &Info);
         ~RenderPick2d ();

      private:
         RenderPick2d ();
         RenderPick2d (const RenderPick2d &);
         RenderPick2d &operator= (const RenderPick2d &);

         const PluginInfo &__Info;
   };
};


inline dmz::RenderPick2d *
dmz::RenderPick2d::cast (const Plugin *PluginPtr, const String &PluginName) {

   return (RenderPick2d *)lookup_rtti_interface (
      DMZ_RENDER_PICK_2D_INTERFACE_NAME,
      PluginName,
      PluginPtr);
}


inline dmz::Boolean
dmz::RenderPick2d::is_valid (const Handle Pick2dHandle, RuntimeContext *context) {

   return (RenderPick2d *)lookup_rtti_interface (
      DMZ_RENDER_PICK_2D_INTERFACE_NAME,
      Pick2dHandle,
      context) != 0;
}


inline
dmz::RenderPick2d::RenderPick2d (const PluginInfo &Info) :
      __Info (Info) {

   store_rtti_interface (DMZ_RENDER_PICK_2D_INTERFACE_NAME, __Info, (void *)this);
}


inline
dmz::RenderPick2d::~RenderPick2d () {

   remove_rtti_interface (DMZ_RENDER_PICK_2D_INTERFACE_NAME, __Info);
}


inline dmz::String
dmz::RenderPick2d::get_pick_2d_name () const { return __Info.get_name (); }


inline dmz::Handle
dmz::RenderPick2d::get_pick_2d_handle () const { return __Info.get_handle (); }


#endif // DMZ_RENDER_PICK_2D_DOT_H

