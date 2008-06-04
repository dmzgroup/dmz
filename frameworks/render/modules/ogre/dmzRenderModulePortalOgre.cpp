#include "dmzRenderModulePortalOgre.h"
#include <dmzRenderUtilOgre.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigRead.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzTypesVector.h>
#include <dmzTypesMatrix.h>

#include <Ogre/Ogre.h>


dmz::RenderModulePortalOgre::RenderModulePortalOgre (
      const PluginInfo &Info, 
      const Config &Local) :
      Plugin (Info),
      RenderModulePortal (Info),
      _portalName (DefaultPortalNameOgre),
      _isMasterPortal (False),
      _core (0),
      _log (Info),
      _camera (0),
      _nearClip (1.0f),
      _farClip (10000.0f), 
      _fov (60.0f) {

   _init (Local);
}


dmz::RenderModulePortalOgre::~RenderModulePortalOgre () {
   
}


void
dmz::RenderModulePortalOgre::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {
      
      if (!_core) {

         _core = RenderModuleCoreOgre::cast (PluginPtr);

         if (_core) {
         
            _camera = _core->lookup_camera (_portalName);

            set_fov (_fov);
            set_near_clip_plane (_nearClip);
            set_far_clip_plane (_farClip);
         }
      }
   }
   else if (Mode == PluginDiscoverRemove) {
      
      if (_core && (_core == RenderModuleCoreOgre::cast (PluginPtr))) {

         _camera = 0;
         _core = 0;
      }
   }

}


dmz::Boolean 
dmz::RenderModulePortalOgre::is_master_portal () {

   return _isMasterPortal;
}


void 
dmz::RenderModulePortalOgre::set_view (const Vector &Pos, const Matrix &Ori) {

   if (_camera) {
      
      _camera->setPosition (to_ogre_vector (Pos));
      _camera->setOrientation (to_ogre_quaternion (Ori));
   }
}


void 
dmz::RenderModulePortalOgre::get_view (Vector &pos, Matrix &ori) const {

   if (_camera) {
      
      //pos = to_dmz_vector (_camera->getPosition ());
      //ori = to_dmz_matrix (_camera->getOrientation ());
   }
}


void 
dmz::RenderModulePortalOgre::set_fov (const Float32 Value) {

   _fov = Value;
   if (_camera) { _camera->setFOVy (Ogre::Degree (_fov)); }
}


dmz::Float32 
dmz::RenderModulePortalOgre::get_fov () const {

//   if (_camera) { retVal = _camera->getFOVy ().valueDegrees (); }
   return _fov;
}


void 
dmz::RenderModulePortalOgre::set_near_clip_plane (const Float32 Value) {

   _nearClip = Value;
   if (_camera) { _camera->setNearClipDistance (Value); }
}


dmz::Float32 
dmz::RenderModulePortalOgre::get_near_clip_plane () const {

//   if (_camera) { retVal = _camera->getNearClipDistance (); }
   return _nearClip;
}


void 
dmz::RenderModulePortalOgre::set_far_clip_plane (const Float32 Value) {

   _farClip = Value;
   if (_camera) { _camera->setFarClipDistance (Value); }
}


dmz::Float32 
dmz::RenderModulePortalOgre::get_far_clip_plane () const {

//   if (_camera) { retVal = _camera->getFarClipDistance (); }
   return _farClip;
}


void
dmz::RenderModulePortalOgre::_init (const Config &Local) {

   _portalName = config_to_string ("portal.name", Local, _portalName);
   _isMasterPortal = config_to_boolean ("portal.master", Local, _isMasterPortal);
   
   Config cullRangeData;
   if (Local.lookup_all_config ("visibility", cullRangeData)) {

      ConfigIterator it;
      Config cd;

      Boolean found (cullRangeData.get_first_config (it, cd));
      if (found)  {

         _fov = config_to_float32 ("fov", cd, _fov);
         _nearClip = config_to_float32 ("near", cd, _nearClip);
         _farClip = config_to_float32 ("far", cd, _farClip);
      }
   }
   
   _log.info << "FOV: " << _fov << endl;
   _log.info << "Clip Planes: " << _nearClip << " / " << _farClip << endl;
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzRenderModulePortalOgre (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::RenderModulePortalOgre (Info, local);
}

};
