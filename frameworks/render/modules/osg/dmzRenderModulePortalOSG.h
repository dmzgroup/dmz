#ifndef DMZ_RENDER_MODULE_PORTAL_OSG_DOT_H
#define DMZ_RENDER_MODULE_PORTAL_OSG_DOT_H

#include <dmzRenderModuleCoreOSG.h>
#include <dmzRenderModulePortal.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzTypesHashTableStringTemplate.h>
#include <dmzTypesMatrix.h>
#include <dmzTypesVector.h>

#include <osg/ref_ptr>
#include <osgViewer/View>

namespace osg { class Camera; }

namespace dmz {

   class RenderModulePortalOSG : public Plugin, private RenderModulePortal {

      public:
         RenderModulePortalOSG (const PluginInfo &Info, const Config &Local);
         ~RenderModulePortalOSG ();

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
         void _update_camera ();

         String _portalName;
         RenderModuleCoreOSG *_core;
         osg::ref_ptr<osgViewer::View> _view;
         osg::ref_ptr<osg::Camera> _camera;
         Log _log;
         Vector _pos;
         Matrix _ori;
         Float32 _nearClip;
         Float32 _farClip;
         Float32 _fov;
   };
};

#endif //  DMZ_RENDER_MODULE_PORTAL_OSG_DOT_H
