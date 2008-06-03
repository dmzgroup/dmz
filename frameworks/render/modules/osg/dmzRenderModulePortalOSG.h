#ifndef DMZ_RENDER_MODULE_PORTAL_OSG_DOT_H
#define DMZ_RENDER_MODULE_PORTAL_OSG_DOT_H

#include <dmzRenderModuleCoreOSG.h>
#include <dmzRenderModulePortal.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzTypesHashTableStringTemplate.h>

#include <osg/ref_ptr>

namespace osg { class Camera; }


namespace dmz {

   class RenderCameraManipulatorOSG;


   class RenderModulePortalOSG : public Plugin, private RenderModulePortal {

      public:
         RenderModulePortalOSG (const PluginInfo &Info, const Config &Local);
         ~RenderModulePortalOSG ();

         virtual void discover_plugin (const Plugin *PluginPtr);
         virtual void start_plugin ();
         virtual void stop_plugin ();
         virtual void shutdown_plugin ();
         virtual void remove_plugin (const Plugin *PluginPtr);

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
         void _update_camera ();

         String _portalName;
         RenderModuleCoreOSG *_core;
         osg::ref_ptr<RenderCameraManipulatorOSG> _manipulator;
         osg::ref_ptr<osg::Camera> _camera;
         Log _log;
         Float32 _nearClip;
         Float32 _farClip;
         Float32 _fov;
   };
};

#endif //  DMZ_RENDER_MODULE_PORTAL_OSG_DOT_H
