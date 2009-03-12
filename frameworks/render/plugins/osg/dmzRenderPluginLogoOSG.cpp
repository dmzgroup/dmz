#include <dmzRenderConsts.h>
#include <dmzRenderModuleCoreOSG.h>
#include "dmzRenderPluginLogoOSG.h"
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>

#include <osg/Material>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/BlendFunc>
#include <osg/Depth>
#include <osg/PolygonOffset>
#include <osg/Matrix>
#include <osg/Camera>
#include <osg/RenderInfo>
#include <osg/Texture2D>

#include <osgDB/ReadFile>

#include <osgViewer/Viewer>


dmz::RenderPluginLogoOSG::RenderPluginLogoOSG (const PluginInfo &Info, Config &local) :
      Plugin (Info),
      TimeSlice (Info),
      _log (Info),
      _rc (Info, &_log),
      _core (0) {

   _init (local);
}


dmz::RenderPluginLogoOSG::~RenderPluginLogoOSG () {

}


// Plugin Interface
void
dmz::RenderPluginLogoOSG::update_plugin_state (
      const PluginStateEnum State,
      const UInt32 Level) {

   if (State == PluginStateInit) {

   }
   else if (State == PluginStateStart) {

   }
   else if (State == PluginStateStop) {

   }
   else if (State == PluginStateShutdown) {

   }
}


void
dmz::RenderPluginLogoOSG::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      if (!_core) {

         _core = RenderModuleCoreOSG::cast (PluginPtr);

         if (_core) {
  
            osgViewer::Viewer *viewer = _core->lookup_viewer (RenderMainPortalName);
            _masterCamera = viewer ? viewer->getCamera () : 0;
            _create_logo ();
         }
      }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_core &&  (_core == RenderModuleCoreOSG::cast (PluginPtr))) {

         _masterCamera = 0;
         _camera = 0;
         _core = 0;
      }
   }
}


// Time Slice Interface
void
dmz::RenderPluginLogoOSG::update_time_slice (const Float64 TimeDelta) {

   if (_masterCamera.valid () && _camera.valid ()) {

      osg::Viewport *vp = _masterCamera->getViewport ();

      if (vp) {

         _camera->setProjectionMatrix (
            osg::Matrix::ortho2D (0.0, vp->width (), 0.0, vp->height ()));
      }
   }
}


void
dmz::RenderPluginLogoOSG::_create_logo () {

   const String FoundFile (_rc.find_file (_imageResource));

   osg::Image *img = (FoundFile ? osgDB::readImageFile (FoundFile.get_buffer ()) : 0);

   if (img) {

      _camera = new osg::Camera;
      _camera->setDataVariance (osg::Object::DYNAMIC);
      _camera->setProjectionMatrix (osg::Matrix::ortho2D (0.0, 640.0, 0.0, 480.0));
      _camera->setReferenceFrame (osg::Transform::ABSOLUTE_RF);
      _camera->setViewMatrix (osg::Matrix::identity ());
      _camera->setClearMask (GL_DEPTH_BUFFER_BIT);
      _camera->setRenderOrder (osg::Camera::POST_RENDER);
      _camera->setAllowEventFocus (false);

      osg::Geode* geode = new osg::Geode ();
      osg::StateSet* stateset = geode->getOrCreateStateSet ();
      stateset->setMode (GL_LIGHTING, osg::StateAttribute::OFF);

      osg::Geometry* geom = new osg::Geometry;

      osg::Vec3Array* normals = new osg::Vec3Array;
      normals->push_back (osg::Vec3 (0.0f, 0.0f, 1.0f));
      geom->setNormalArray (normals);
      geom->setNormalBinding (osg::Geometry::BIND_OVERALL);

      osg::Vec4Array* colors = new osg::Vec4Array;
      colors->push_back (osg::Vec4 (1.0f, 1.0, 1.0f, 1.0f));
      geom->setColorArray (colors);
      geom->setColorBinding (osg::Geometry::BIND_OVERALL);

      geom->addPrimitiveSet (new osg::DrawArrays (GL_QUADS, 0, 4));

      stateset = geom->getOrCreateStateSet ();
      stateset->setMode (GL_BLEND, osg::StateAttribute::ON);
      stateset->setRenderingHint (osg::StateSet::TRANSPARENT_BIN);

      float w = (float)img->s ();
      float h = (float)img->t ();
      float d = -0.1f;

      osg::Vec3Array* vertices = new osg::Vec3Array;
      vertices->push_back (osg::Vec3 (0.0, h, d));
      vertices->push_back (osg::Vec3 (0.0, 0.0, d));
      vertices->push_back (osg::Vec3 (w, 0.0, d));
      vertices->push_back (osg::Vec3 (w, h, d));
      geom->setVertexArray (vertices);

      osg::Texture2D *tex = new osg::Texture2D (img);
      tex->setWrap (osg::Texture2D::WRAP_S, osg::Texture2D::CLAMP_TO_EDGE);
      tex->setWrap (osg::Texture2D::WRAP_T, osg::Texture2D::CLAMP_TO_EDGE);

      stateset->setTextureAttributeAndModes (0, tex, osg::StateAttribute::ON);
      osg::Vec2Array *tcoords = new osg::Vec2Array (4);
      (*tcoords)[0].set (0, 1.0);
      (*tcoords)[1].set (0, 0);
      (*tcoords)[2].set (1.0, 0);
      (*tcoords)[3].set (1.0, 1.0);
      geom->setTexCoordArray (0, tcoords);

      geode->addDrawable (geom);

      _camera->addChild (geode);

      if (_core) {

         osg::Group *s = _core->get_overlay ();

         if (s) { s->addChild (_camera.get ()); }
      }
   }
   else if (FoundFile) {

      _log.error << "Unable to load overlay image: " << FoundFile << endl;
   }
   else {

      _log.error << "Unable to find resource: " << _imageResource << endl;
   }
}


void
dmz::RenderPluginLogoOSG::_init (Config &local) {

   _imageResource = config_to_string ("image.resource", local, "logo");
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzRenderPluginLogoOSG (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::RenderPluginLogoOSG (Info, local);
}

};
