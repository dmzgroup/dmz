#ifndef DMZ_RENDER_PICK_DOT_H
#define DMZ_RENDER_PICK_DOT_H

#include <dmzRuntimePlugin.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzRuntimeRTTI.h>
#include <dmzTypesBase.h>
#include <dmzTypesString.h>

namespace dmz {

   //! \cond
   const char RenderPickInterfaceName[] = "RenderPickInteface";
   //! \endcond

   class RenderModulePick;
   class Vector;

   class RenderPick {

      public:
         static RenderPick *cast (
            const Plugin *PluginPtr,
            const String &PluginName = "");

         static Boolean is_valid (const Handle PickHandle, RuntimeContext *context);

         String get_pick_name () const;
         Handle get_pick_handle () const;

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
            Vector &normal,
            Handle &objectHandle) = 0;

         virtual Boolean world_to_screen (
            const Vector &WorldPosition,
            Int32 &screenPosX,
            Int32 &screenPosY) = 0;

         virtual Boolean source_to_world (
            const Int32 SourcePosX,
            const Int32 SourcePosY,
            Vector &worldPosition,
            Vector &normal,
            Handle &objectHandle) = 0;

         virtual Boolean world_to_source (
            const Vector &WorldPosition,
            Int32 &sourcePosX,
            Int32 &sourcePosY) = 0;

      protected:
         RenderPick (const PluginInfo &Info);
         ~RenderPick ();

      private:
         RenderPick ();
         RenderPick (const RenderPick &);
         RenderPick &operator= (const RenderPick &);

         const PluginInfo &__Info;
   };
};


inline dmz::RenderPick *
dmz::RenderPick::cast (const Plugin *PluginPtr, const String &PluginName) {

   return (RenderPick *)lookup_rtti_interface (
      RenderPickInterfaceName,
      PluginName,
      PluginPtr);
}


inline dmz::Boolean
dmz::RenderPick::is_valid (const Handle PickHandle, RuntimeContext *context) {

   return (RenderPick *)lookup_rtti_interface (
      RenderPickInterfaceName,
      PickHandle,
      context) != 0;
}


inline
dmz::RenderPick::RenderPick (const PluginInfo &Info) :
      __Info (Info) {

   store_rtti_interface (RenderPickInterfaceName, __Info, (void *)this);
}


inline
dmz::RenderPick::~RenderPick () {

   remove_rtti_interface (RenderPickInterfaceName, __Info);
}


inline dmz::String
dmz::RenderPick::get_pick_name () const { return __Info.get_name (); }


inline dmz::Handle
dmz::RenderPick::get_pick_handle () const { return __Info.get_handle (); }


#endif // DMZ_RENDER_PICK_DOT_H

