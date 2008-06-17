#ifndef DMZ_RENDER_MODULE_PORTAL_OGRE_DOT_H
#define DMZ_RENDER_MODULE_PORTAL_OGRE_DOT_H

#include <dmzRenderModuleCoreOgre.h>
#include <dmzRenderModulePortal.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzTypesHashTableStringTemplate.h>

namespace Ogre { class Camera; }


namespace dmz {


   class RenderModulePortalOgre : public Plugin, private RenderModulePortal {

      public:
         RenderModulePortalOgre (const PluginInfo &Info, const Config &Local);
         ~RenderModulePortalOgre ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level) {;}

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // RenderPortal Interface
         virtual Boolean is_master_portal ();

         virtual void set_view (const Vector &Pos, const Matrix &Ori);
         virtual void get_view (Vector &pos, Matrix &ori) const;

         virtual void set_fov (const Float32 Value);
         virtual Float32 get_fov () const;

         virtual void set_near_clip_plane (const Float32 Value);
         virtual Float32 get_near_clip_plane () const;

         virtual void set_far_clip_plane (const Float32 Value);
         virtual Float32 get_far_clip_plane () const;

      protected:
         void _init (const Config &Local);

         String _portalName;
         Boolean _isMasterPortal;
         RenderModuleCoreOgre *_core;
         Ogre::Camera *_camera;
         Log _log;
         Float32 _nearClip;
         Float32 _farClip;
         Float32 _fov;

      private:
         RenderModulePortalOgre ();
         RenderModulePortalOgre (const RenderModulePortalOgre &);
         RenderModulePortalOgre &operator= (const RenderModulePortalOgre &);
   };
};

#endif //  DMZ_RENDER_MODULE_PORTAL_OGRE_DOT_H
