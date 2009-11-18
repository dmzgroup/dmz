#ifndef DMZ_RENDER_MODULE_PICK_CONVERT_DOT_H
#define DMZ_RENDER_MODULE_PICK_CONVERT_DOT_H

#include <dmzRuntimePlugin.h>
#include <dmzRuntimeRTTI.h>
#include <dmzTypesBase.h>
#include <dmzTypesString.h>
#include <dmzTypesVector.h>


namespace dmz {
   //! \cond
   class RenderModulePickConvert {

      public:
         static RenderModulePickConvert *cast (
            const Plugin *PluginPtr,
            const String &PluginName = "");

         String get_render_module_pick_convert_name () const;
         Handle get_render_module_pick_convert_handle () const;

         // RenderModulePickConvert Interface
         virtual Boolean source_to_world (
            const Int32 ScreenPosX,
            const Int32 ScreenPosY,
            Vector &worldPosition,
            Vector &normal,
            Handle &objectHandle) = 0;

         virtual Boolean world_to_source (
            const Vector &WorldPosition,
            Int32 &screenPosX,
            Int32 &screenPosY) = 0;

      protected:
         RenderModulePickConvert (const PluginInfo &Info);
         ~RenderModulePickConvert ();

      private:
         RenderModulePickConvert ();
         RenderModulePickConvert (const RenderModulePickConvert &);
         RenderModulePickConvert &operator= (const RenderModulePickConvert &);

         const PluginInfo &__Info;
   };
   //! \endcond

   //! \cond
   const char RenderModulePickConvertInterfaceName[] = "RenderModulePickConvertInterface";
   //! \endcond
};


//! \cond
inline dmz::RenderModulePickConvert *
dmz::RenderModulePickConvert::cast (const Plugin *PluginPtr, const String &PluginName) {

   return (RenderModulePickConvert *)lookup_rtti_interface (
      RenderModulePickConvertInterfaceName,
      PluginName,
      PluginPtr);
}


inline
dmz::RenderModulePickConvert::RenderModulePickConvert (const PluginInfo &Info) :
      __Info (Info) {

   store_rtti_interface (RenderModulePickConvertInterfaceName, __Info, (void *)this);
}


inline
dmz::RenderModulePickConvert::~RenderModulePickConvert () {

   remove_rtti_interface (RenderModulePickConvertInterfaceName, __Info);
}


inline dmz::String
dmz::RenderModulePickConvert::get_render_module_pick_convert_name () const { return __Info.get_name (); }


inline dmz::Handle
dmz::RenderModulePickConvert::get_render_module_pick_convert_handle () const { return __Info.get_handle (); }
//! \endcond


#endif // DMZ_RENDER_MODULE_PICK_CONVERT_DOT_H
