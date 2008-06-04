#include "dmzRenderModulePortalOSG.h"
#include <dmzRenderCameraManipulatorOSG.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigRead.h>
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
      _portalName (DefaultPortalNameOSG),
      _core (0),
      _log (Info),
      _manipulator (0),
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

            _manipulator = _core->lookup_camera_manipulator (_portalName);

            _camera = _core->lookup_camera (_portalName);
            if (_camera.valid ()) {

               _camera->setComputeNearFarMode (osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);
               _update_camera ();
            }
         }
      }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_core && (_core == RenderModuleCoreOSG::cast (PluginPtr))) {

         _core = 0;
         _camera.release ();
         _manipulator.release ();
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

   _manipulator->setByVectorAndMatrix (Pos, Ori);
}


void 
dmz::RenderModulePortalOSG::get_view (Vector &pos, Matrix &ori) const {

   _manipulator->getVectorAndMatrix (pos, ori);
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

   if (_camera.get ()) {

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
