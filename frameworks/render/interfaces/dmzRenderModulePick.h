#ifndef DMZ_RENDER_MODULE_PICK_DOT_H
#define DMZ_RENDER_MODULE_PICK_DOT_H

#include <dmzRuntimePlugin.h>
#include <dmzRuntimeRTTI.h>
#include <dmzTypesBase.h>

namespace dmz {

   const char RenderModulePickInterfaceName[] = "RenderModulePickInterface";

   class RenderPick;

   class RenderModulePick {

      public:
         static RenderModulePick *cast (
            const Plugin *PluginPtr,
            const String &PluginName = "");

         virtual Boolean register_pick (const Handle Source, RenderPick &pick) = 0;
         virtual Boolean release_pick (const Handle Source, RenderPick &pick) = 0;

         // RenderPick
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
      RenderModulePickInterfaceName,
      PluginName,
      PluginPtr);
}


inline
dmz::RenderModulePick::RenderModulePick (const PluginInfo &Info) :
      __Info (Info) {

   store_rtti_interface (RenderModulePickInterfaceName, __Info, (void *)this);
}


inline
dmz::RenderModulePick::~RenderModulePick () {

   remove_rtti_interface (RenderModulePickInterfaceName, __Info);
}


#endif // DMZ_RENDER_MODULE_PICK_DOT_H
