#include <dmzRenderConsts.h>
#include "dmzRenderModulePortalOSG.h"
#include <dmzRenderUtilOSG.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzTypesVector.h>
#include <dmzTypesMatrix.h>

#include <osg/Camera>
#include <osgViewer/Viewer>
#include <osg/Depth>
#include <iostream>


dmz::RenderModulePortalOSG::RenderModulePortalOSG (
      const PluginInfo &Info,
      const Config &Local) :
      Plugin (Info),
      RenderModulePortal (Info),
      _portalName (RenderMainPortalName),
      _core (0),
      _log (Info),
      _viewer (0),
      _camera (0),
      _nearClip (0.0),
      _farClip (0.0),
      _fov (0.0) {

   _init (Local);
}


dmz::RenderModulePortalOSG::~RenderModulePortalOSG () {

}


void
dmz::RenderModulePortalOSG::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      if (!_core) {

         _core = RenderModuleCoreOSG::cast (PluginPtr);
         if (_core) {

            _viewer = _core->lookup_viewer (_portalName);

            if (_viewer.valid ()) { _camera = _viewer->getCamera (); }

            if (_camera.valid ()) {

               _camera->setComputeNearFarMode (
                  osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);

               _update_camera ();
            }
         }
      }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_core && (_core == RenderModuleCoreOSG::cast (PluginPtr))) {

         _core = 0;
         _camera = 0;
         _viewer = 0;
      }
   }
}


dmz::Boolean
dmz::RenderModulePortalOSG::is_master_portal () {

   Boolean result (False);

   return result;
}


void
dmz::RenderModulePortalOSG::set_view (const Vector &Pos, const Matrix &Ori) {

   if (_camera.valid ()) {

      _pos = Pos;
      _ori = Ori;
      _camera->setViewMatrix (
         osg::Matrix::translate (to_osg_vector (-Pos)) * to_osg_inverse_matrix (Ori));
   }
}


void
dmz::RenderModulePortalOSG::get_view (Vector &pos, Matrix &ori) const {

   pos = _pos;
   ori = _ori;
}


void
dmz::RenderModulePortalOSG::set_fov (const Float32 Value) {

   _fov = Value;
   _update_camera ();
}


dmz::Float32
dmz::RenderModulePortalOSG::get_fov () const {

   return _fov;
}


void
dmz::RenderModulePortalOSG::set_near_clip_plane (const Float32 Value) {

   _nearClip = Value;
   _update_camera ();
}


dmz::Float32
dmz::RenderModulePortalOSG::get_near_clip_plane () const {

   return _nearClip;
}


void
dmz::RenderModulePortalOSG::set_far_clip_plane (const Float32 Value) {

   _farClip = Value;
   _update_camera ();
}


dmz::Float32
dmz::RenderModulePortalOSG::get_far_clip_plane () const {

   return _farClip;
}


void
dmz::RenderModulePortalOSG::_init (const Config &Local) {

   _portalName = config_to_string ("portal.name", Local, _portalName);

   Config cullRangeData;
   if (Local.lookup_all_config ("visibility", cullRangeData)) {

      ConfigIterator it;
      Config cd;

      Boolean found (cullRangeData.get_first_config (it, cd));
      if (found)  {
         _fov = config_to_float32 ("fov", cd);
         _nearClip = config_to_float32 ("near", cd);
         _farClip = config_to_float32 ("far", cd);
      }
   }
   else {

      _fov = 60.0;
      _nearClip = 1.0f;
      _farClip = 10000.0f;
   }

   _log.info << "FOV: " << _fov << endl;
   _log.info << "Clip Planes: " << _nearClip << " / " << _farClip << endl;
}


void
dmz::RenderModulePortalOSG::_update_camera () {

   if (_camera.valid ()) {

      _camera->setProjectionMatrixAsPerspective(
         _fov,
         _camera->getViewport ()->aspectRatio (),
         _nearClip,
         _farClip);
   }
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzRenderModulePortalOSG (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::RenderModulePortalOSG (Info, local);
}

};
