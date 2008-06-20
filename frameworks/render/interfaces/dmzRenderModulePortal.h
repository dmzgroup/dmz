#ifndef DMZ_RENDER_MODULE_PORTAL_DOT_H
#define DMZ_RENDER_MODULE_PORTAL_DOT_H

#include <dmzRuntimePlugin.h>
#include <dmzRuntimeRTTI.h>
#include <dmzTypesBase.h>

#define DMZ_RENDER_MODULE_PORTAL_INTERFACE_NAME "RenderModulePortalInterface"

namespace dmz {

   class Matrix;
   class Vector;

   class RenderModulePortal {

      public:
         static RenderModulePortal *cast (
            const Plugin *PluginPtr,
            const String &PluginName = "");

         virtual String get_render_portal_name ();
         virtual UInt32 get_render_portal_handle ();

         virtual Boolean is_master_portal () = 0;

         virtual void set_view (const Vector &Pos, const Matrix &Ori) = 0;
         virtual void get_view (Vector &pos, Matrix &ori) const = 0;

         virtual void set_fov (const Float32 Value) = 0;
         virtual Float32 get_fov () const = 0;

         virtual void set_near_clip_plane (const Float32 Value) = 0;
         virtual Float32 get_near_clip_plane () const = 0;

         virtual void set_far_clip_plane (const Float32 Value) = 0;
         virtual Float32 get_far_clip_plane () const = 0;

      protected:
         RenderModulePortal (const PluginInfo &Info);
         ~RenderModulePortal ();

      private:
         const PluginInfo &__Info;
   };
};


inline dmz::String
dmz::RenderModulePortal::get_render_portal_name () {

   String result;

   return result;
}

inline dmz::UInt32
dmz::RenderModulePortal::get_render_portal_handle () {

   UInt32 result (0);

   return result;
}


inline dmz::RenderModulePortal *
dmz::RenderModulePortal::cast (const Plugin *PluginPtr, const String &PluginName) {

   return (RenderModulePortal *)lookup_rtti_interface (
      DMZ_RENDER_MODULE_PORTAL_INTERFACE_NAME,
      PluginName,
      PluginPtr);
}


inline
dmz::RenderModulePortal::RenderModulePortal (const PluginInfo &Info) :
      __Info (Info) {

   store_rtti_interface (DMZ_RENDER_MODULE_PORTAL_INTERFACE_NAME, __Info, (void *)this);
}


inline
dmz::RenderModulePortal::~RenderModulePortal () {

   remove_rtti_interface (DMZ_RENDER_MODULE_PORTAL_INTERFACE_NAME, __Info);
}

#endif //  DMZ_RENDER_MODULE_PORTAL_DOT_H

