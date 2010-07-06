#include <dmzRenderConfigToOSG.h>
#include <dmzRenderConsts.h>
#include <dmzRenderModuleCoreOSG.h>
#include "dmzRenderPluginHeightMapOSG.h"
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeConfigToVector.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>

#include <osg/CullFace>
#include <osg/Shape>
#include <osg/ShapeDrawable>
#include <osg/Texture2D>
#include <osgDB/ReadFile>


dmz::RenderPluginHeightMapOSG::RenderPluginHeightMapOSG (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      _log (Info),
      _rc (Info) {

   _init (local);
}


dmz::RenderPluginHeightMapOSG::~RenderPluginHeightMapOSG () {

}


// Plugin Interface
void
dmz::RenderPluginHeightMapOSG::update_plugin_state (
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
dmz::RenderPluginHeightMapOSG::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      RenderModuleCoreOSG *core = RenderModuleCoreOSG::cast (PluginPtr);

      if (core) {

         osg::ref_ptr<osg::Group> scene = core->get_isect ();

         if (scene.valid () && _terrain.valid ()) {

            _terrain->setNodeMask (
               (_terrain->getNodeMask () & (~(core->get_master_isect_mask ()))) |
               core->lookup_isect_mask (RenderIsectStaticName));

            scene->addChild (_terrain);
         }
      }
   }
   else if (Mode == PluginDiscoverRemove) {

      RenderModuleCoreOSG *core = RenderModuleCoreOSG::cast (PluginPtr);

      if (core) {

         osg::ref_ptr<osg::Group> scene = core->get_isect ();

         if (scene.valid () && _terrain.valid ()) {

            _terrain->setNodeMask (
               _terrain->getNodeMask () | core->get_master_isect_mask ());

            scene->removeChild (_terrain);
         }
      }
   }
}


// RenderPluginHeightMapOSG Interface
void
dmz::RenderPluginHeightMapOSG::_init_height_map (Config &local) {

   const String MapName = config_to_string ("resource", local);
   const String MapFile = _rc.find_file (MapName);

   if (MapFile) {

      osg::ref_ptr<osg::Image> image = osgDB::readImageFile (MapFile.get_buffer ());

      if (image.valid ()) {

         osg::ref_ptr<osg::Geode> geode = new osg::Geode;

         osg::HeightField* heightField = new osg::HeightField;
         heightField->allocate(image->s (), image->t ());

         const Vector Origin = config_to_vector (local);

         heightField->setOrigin(
            osg::Vec3(Origin.get_x (), Origin.get_y (), Origin.get_z ()));


         const String Up = config_to_string ("up", local, "z");

         if (Up == "y") {

            osg::Quat rot;
            rot.makeRotate (-HalfPi64, osg::Vec3 (1.0, 0.0, 0.0));
            heightField->setRotation (rot);
         }

         heightField->setXInterval (config_to_float64 ("interval-x", local, 1.0));
         heightField->setYInterval (config_to_float64 ("interval-y", local, 1.0));
         heightField->setSkirtHeight (1.0f);

         const Float64 Min = config_to_float64 ("min", local, 0.0);
         const Float64 Max = config_to_float64 ("max", local, 1.0);
         const Float64 Diff = Max - Min;
 
         for (int r = 0; r < heightField->getNumRows(); r++) {

            for (int c = 0; c < heightField->getNumColumns(); c++) {

               heightField->setHeight (
                  c,
                  r,
                  (((*image->data(c, r)) / 255.0f) * Diff) + Min);
            }
         }

         osg::ref_ptr<osg::StateSet> stateset = geode->getOrCreateStateSet ();

         stateset->setAttributeAndModes (new osg::CullFace (osg::CullFace::BACK));
         stateset->setMode (GL_BLEND, osg::StateAttribute::ON);

         const String TexName = config_to_string ("texture", local);
         const String TexFile = _rc.find_file (TexName);

         if (TexFile) {

            osg::ref_ptr<osg::Image> image = osgDB::readImageFile (TexFile.get_buffer ());

            if (image.valid ()) {

               osg::Texture2D* tex = new osg::Texture2D(image);

               tex->setFilter (
                  osg::Texture2D::MIN_FILTER,
                  osg::Texture2D::LINEAR_MIPMAP_LINEAR);

               tex->setFilter (osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR);
               tex->setWrap (osg::Texture::WRAP_S, osg::Texture::REPEAT);
               tex->setWrap (osg::Texture::WRAP_T, osg::Texture::REPEAT);
               stateset->setTextureAttributeAndModes (0, tex);
            }
         }
         else if (TexName) {

            _log.error << "Unable to find height map texture resource: " << TexName
               << endl;
         }

         osg::ref_ptr<osg::ShapeDrawable> shape = new osg::ShapeDrawable (heightField);

         const osg::Vec4 Color = config_to_osg_vec4_color (
            local,
            osg::Vec4 (1.0, 1.0, 1.0, 1.0));

         if (Color.w () < 1.0) {

            stateset->setRenderingHint (osg::StateSet::TRANSPARENT_BIN);
         }

         shape->setColor (Color);

         geode->addDrawable (shape);

         _terrain->addChild (geode);
      }
   }
   else if (MapName) {

      _log.error << "Unable to find resource: " << MapName << endl;
   }
   else {

      _log.error << "No height map resource specified." << endl;
   }
}


void
dmz::RenderPluginHeightMapOSG::_init (Config &local) {

   _terrain = new osg::Group;

   Config list;

   if (local.lookup_all_config ("height-map", list)) {

      ConfigIterator it;
      Config map;

      while (list.get_next_config (it, map)) { _init_height_map (map); }
   }
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzRenderPluginHeightMapOSG (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::RenderPluginHeightMapOSG (Info, local);
}

};
