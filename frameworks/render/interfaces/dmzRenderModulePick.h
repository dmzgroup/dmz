#ifndef DMZ_RENDER_MODULE_PICK_DOT_H
#define DMZ_RENDER_MODULE_PICK_DOT_H

#include <dmzRuntimePlugin.h>
#include <dmzRuntimeRTTI.h>
#include <dmzTypesBase.h>

#define DMZ_RENDER_MODULE_PICK_INTERFACE_NAME "RenderModulePickInterface"


namespace dmz {

   class RenderPick2d;
   class RenderPick3d;

   class RenderModulePick {

      public:
         static RenderModulePick *cast (
            const Plugin *PluginPtr,
            const String &PluginName = "");

         virtual Boolean register_pick_2d (const Handle Source, RenderPick2d &pick) = 0;
         virtual Boolean release_pick_2d (const Handle Source, RenderPick2d &pick) = 0;

//         virtual Boolean register_pick_3d (const Handle Source, RenderPick3d &pick) = 0;
//         virtual Boolean release_pick_3d (const Handle Source, RenderPick3d &pick) = 0;

         // RenderPick2d
         virtual Boolean screen_to_world (
            const Handle Source,
            const Int32 ScreenPosX,
            const Int32 ScreenPosY,
            Vector &worldPosition,
            Handle &objectHandle) = 0;

         virtual Boolean world_to_screen (
            const Handle Source,
            const Vector &WorldPosition,
            Int32 &screenPosX,
            Int32 &screenPosY) = 0;

         virtual Boolean source_to_world (
            const Handle Source,
            const Int32 ScreenPosX,
            const Int32 ScreenPosY,
            Vector &worldPosition,
            Handle &objectHandle) = 0;

         virtual Boolean world_to_source (
            const Handle Source,
            const Vector &WorldPosition,
            Int32 &screenPosX,
            Int32 &screenPosY) = 0;

         // RenderPick3d
         // TODO

      protected:
         RenderModulePick (const PluginInfo &Info);
         ~RenderModulePick ();

      private:
         const PluginInfo &__Info;
   };
};


inline dmz::RenderModulePick *
dmz::RenderModulePick::cast (const Plugin *PluginPtr, const String &PluginName) {

   return (RenderModulePick *)lookup_rtti_interface (
      DMZ_RENDER_MODULE_PICK_INTERFACE_NAME,
      PluginName,
      PluginPtr);
}


inline
dmz::RenderModulePick::RenderModulePick (const PluginInfo &Info) :
      __Info (Info) {

   store_rtti_interface (DMZ_RENDER_MODULE_PICK_INTERFACE_NAME, __Info, (void *)this);
}


inline
dmz::RenderModulePick::~RenderModulePick () {

   remove_rtti_interface (DMZ_RENDER_MODULE_PICK_INTERFACE_NAME, __Info);
}


#endif // DMZ_RENDER_MODULE_PICK_DOT_H
