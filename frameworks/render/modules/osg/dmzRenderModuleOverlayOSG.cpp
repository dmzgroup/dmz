#include "dmzRenderModuleOverlayOSG.h"
#include <dmzRenderModuleCoreOSG.h>
#include <dmzRenderUtilOSG.h>
#include <dmzRenderConfigToOSG.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeConfigToVector.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>

#include <osg/Texture2D>
#include <osgDB/ReadFile>


dmz::RenderModuleOverlayOSG::RenderModuleOverlayOSG (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      RenderModuleOverlay (Info),
      PortalSizeObserver (Info, local),
      _log (Info),
      _rc (Info),
      _core (0) {

   _rootNode = new osg::Group;

   _init (local);
}


dmz::RenderModuleOverlayOSG::~RenderModuleOverlayOSG () {

}


// Plugin Interface
void
dmz::RenderModuleOverlayOSG::update_plugin_state (
      const PluginStateEnum State,
      const UInt32 Level) {

   if (State == PluginStateInit) {

      if (_core) {

         osg::ref_ptr<osg::Group> node = _core->get_overlay ();

         if (node.valid () && _camera.valid ())  {

            node->addChild (_camera.get ());
         }
      }
   }
   else if (State == PluginStateStart) {

   }
   else if (State == PluginStateStop) {

   }
   else if (State == PluginStateShutdown) {

      if (_core) {

         osg::ref_ptr<osg::Group> node = _core->get_overlay ();

         if (node.valid () && _camera.valid ())  {

            node->removeChild (_camera.get ());
         }

         _camera = 0;
         _rootNode = 0;
         _textureTable.empty ();
      }
   }
}


void
dmz::RenderModuleOverlayOSG::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      if (!_core) { _core = RenderModuleCoreOSG::cast (PluginPtr); }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_core && (_core == RenderModuleCoreOSG::cast (PluginPtr))) { _core = 0; }
   }
}


// RenderModuleOverlay Interface
dmz::Handle
dmz::RenderModuleOverlayOSG::lookup_overlay_handle (const String &Name) {

   Handle result (0);

   return result;
}


dmz::String
dmz::RenderModuleOverlayOSG::lookup_overlay_name (const Handle Overlay) {

   String result;

   return result;
}


dmz::RenderOverlayTypeEnum
dmz::RenderModuleOverlayOSG::lookup_overlay_type (const Handle Overlay) {

   RenderOverlayTypeEnum result (RenderOverlayUnknown);

   return result;
}


dmz::Boolean
dmz::RenderModuleOverlayOSG::is_of_overlay_type (
      const Handle Overlay,
      const RenderOverlayTypeEnum Type) {

   Boolean result (False);

   return result;
}


// Overlay Switch API
dmz::Boolean
dmz::RenderModuleOverlayOSG::store_overlay_switch_state (
      const Handle Overlay,
      const Int32 Which,
      const Boolean SwitchState) {

   Boolean result (False);

   return result;
}


dmz::Boolean
dmz::RenderModuleOverlayOSG::store_overlay_all_switch_state (
      const Handle Overlay,
      const Boolean SwitchState) {

   Boolean result (False);

   return result;
}


dmz::Boolean
dmz::RenderModuleOverlayOSG::enable_overlay_single_switch_state (
      const Handle Overlay,
      const Int32 Which) {

   Boolean result (False);

   return result;
}


dmz::Boolean
dmz::RenderModuleOverlayOSG::lookup_overlay_switch_state (
      const Handle Overlay,
      const Int32 Which) {

   Boolean result (False);

   return result;
}


dmz::Int32
dmz::RenderModuleOverlayOSG::lookup_overlay_switch_count (const Handle Overlay) {

   Int32 result (0);

   return result;
}


// Overlay Transform API
dmz::Boolean
dmz::RenderModuleOverlayOSG::store_overlay_position (
      const Handle Overlay,
      const Float64 ValueX,
      const Float64 ValueY) {

   Boolean result (False);

   return result;
}


dmz::Boolean
dmz::RenderModuleOverlayOSG::lookup_overlay_position (
      const Handle Overlay,
      Float64 &valueX,
      Float64 &valueY) {

   Boolean result (False);

   return result;
}


dmz::Boolean
dmz::RenderModuleOverlayOSG::store_overlay_rotation (
      const Handle Overlay,
      const Float64 Value) {

   Boolean result (False);

   return result;
}


dmz::Boolean
dmz::RenderModuleOverlayOSG::lookup_overlay_rotation (
      const Handle Overlay,
      Float64 &value) {

   Boolean result (False);

   return result;
}


dmz::Boolean
dmz::RenderModuleOverlayOSG::store_overlay_scale (
      const Handle Overlay,
      const Float64 ValueX,
      const Float64 ValueY) {

   Boolean result (False);

   return result;
}


dmz::Boolean
dmz::RenderModuleOverlayOSG::lookup_overlay_scale (
      const Handle Overlay,
      Float64 &valueX,
      Float64 &valueY) {

   Boolean result (False);

   return result;
}


// PortalSizeObserver Interface
void
dmz::RenderModuleOverlayOSG::update_portal_size (
      const Handle PortalHandle,
      const Int32 TheX,
      const Int32 TheY) {

   if (_camera.valid ()) {

      _camera->setProjectionMatrix (
         osg::Matrix::ortho2D (0.0, Float64 (TheX), 0.0, Float64 (TheY)));
   }
}

// Protected methods
dmz::RenderModuleOverlayOSG::TextureStruct *
dmz::RenderModuleOverlayOSG::_create_texture (const String &Name) {

   TextureStruct *result (_textureTable.lookup (Name));

   if (!result) {

      const String ImageName = _rc.find_file (Name);

      if (ImageName) {

         osg::ref_ptr<osg::Image> img = osgDB::readImageFile (ImageName.get_buffer ());

         if (img.valid ()) {

            result = new TextureStruct;

            if (result && _textureTable.store (Name, result)) { result->img = img; }
            else if (result) { delete result; result = 0; }
         }
      }
   }

   return result;
}


void
dmz::RenderModuleOverlayOSG::_add_children (
      osg::ref_ptr<osg::Group> &parent,
      Config &node) {

   Config nodeList;

   if (node.lookup_all_config ("node", nodeList)) {

      ConfigIterator it;
      Config child;

      while (nodeList.get_next_config (it, child)) { _add_node (parent, child); }
   }
}


void
dmz::RenderModuleOverlayOSG::_add_node (osg::ref_ptr<osg::Group> &parent, Config &node) {

   const String TypeName = config_to_string ("type", node);

   if (TypeName == "group") { _add_group (parent, node); }
   else if (TypeName == "switch") { _add_switch (parent, node); }
   else if (TypeName == "transform") { _add_transform (parent, node); }
   else if (TypeName == "box") { _add_box (parent, node); }
   else {

      _log.error << "Unknown overlay node type: " << TypeName << endl;
   }
}


void
dmz::RenderModuleOverlayOSG::_add_group (
      osg::ref_ptr<osg::Group> &parent,
      Config &node) {

   osg::ref_ptr<osg::Group> child = new osg::Group;
   parent->addChild (child.get ());
   _add_children (child, node);
}


void
dmz::RenderModuleOverlayOSG::_add_switch (
      osg::ref_ptr<osg::Group> &parent,
      Config &node) {

   osg::ref_ptr<osg::Switch> ptr = new osg::Switch;
   parent->addChild (ptr.get ());
   
   osg::ref_ptr<osg::Group> child = ptr.get ();
   _add_children (child, node);
}


void
dmz::RenderModuleOverlayOSG::_add_transform (
      osg::ref_ptr<osg::Group> &parent,
      Config &node) {


}


void
dmz::RenderModuleOverlayOSG::_add_box (osg::ref_ptr<osg::Group> &parent, Config &node) {

   const String ImageResource = config_to_string ("texture.resource", node);

   TextureStruct *ts = _create_texture (ImageResource);

   const Vector DefaultSize (
      ts ? Float64 (ts->img->s ()) : 1.0,
      ts ? Float64 (ts->img->t ()) : 1.0,
      0.0);

   const Vector Min = config_to_vector ("min", node);
   const Vector Max = config_to_vector ("max", node, Min + DefaultSize);
   const Float64 Depth = config_to_float64 ("depth.value", node);
   const Vector TMin = config_to_vector ("texture.min", node);
   const Vector TMax = config_to_vector ("texture.max", node, Vector (1.0, 1.0, 1.0));

   osg::Geode* geode = new osg::Geode ();
   osg::StateSet* stateset = geode->getOrCreateStateSet ();
   stateset->setMode (GL_LIGHTING, osg::StateAttribute::OFF);

   osg::Geometry* geom = new osg::Geometry;

   osg::Vec3Array* normals = new osg::Vec3Array;
   normals->push_back (osg::Vec3 (0.0f, 0.0f, 1.0f));
   geom->setNormalArray (normals);
   geom->setNormalBinding (osg::Geometry::BIND_OVERALL);

   osg::Vec4Array* colors = new osg::Vec4Array;

   colors->push_back (
      config_to_osg_vec4_color ("color", node, osg::Vec4 (1.0f, 1.0, 1.0f, 1.0f)));

   geom->setColorArray (colors);
   geom->setColorBinding (osg::Geometry::BIND_OVERALL);

   geom->addPrimitiveSet (new osg::DrawArrays (GL_QUADS, 0, 4));

   stateset = geom->getOrCreateStateSet ();
   stateset->setMode (GL_BLEND, osg::StateAttribute::ON);
   stateset->setRenderingHint (osg::StateSet::TRANSPARENT_BIN);

   float minw = Min.get_x ();
   float maxw = Max.get_x ();
   float minh = Min.get_y ();
   float maxh = Max.get_y ();

   osg::Vec3Array* vertices = new osg::Vec3Array;
   vertices->push_back (osg::Vec3 (minw, maxh, Depth));
   vertices->push_back (osg::Vec3 (minw, minh, Depth));
   vertices->push_back (osg::Vec3 (maxw, minh, Depth));
   vertices->push_back (osg::Vec3 (maxw, maxh, Depth));
   geom->setVertexArray (vertices);

   if (ts) {

      osg::Texture2D *tex = new osg::Texture2D (ts->img.get ());
      tex->setWrap (osg::Texture2D::WRAP_S, osg::Texture2D::CLAMP_TO_EDGE);
      tex->setWrap (osg::Texture2D::WRAP_T, osg::Texture2D::CLAMP_TO_EDGE);

      stateset->setTextureAttributeAndModes (0, tex, osg::StateAttribute::ON);
      osg::Vec2Array *tcoords = new osg::Vec2Array (4);
      (*tcoords)[0].set (TMin.get_x (), TMax.get_y());
      (*tcoords)[1].set (TMin.get_x (), TMin.get_y ());
      (*tcoords)[2].set (TMax.get_x (), TMin.get_y ());
      (*tcoords)[3].set (TMax.get_x (), TMax.get_y ());
      geom->setTexCoordArray (0, tcoords);
   }

   geode->addDrawable (geom);

   parent->addChild (geode);
}


void
dmz::RenderModuleOverlayOSG::_init (Config &local) {

   _camera = new osg::Camera;
   _camera->setDataVariance (osg::Object::DYNAMIC);

   _camera->setProjectionMatrix (osg::Matrix::ortho2D (
      0.0,
      Float64 (get_portal_x ()),
      0.0,
      Float64 (get_portal_y ())));

   _camera->setReferenceFrame (osg::Transform::ABSOLUTE_RF);
   _camera->setViewMatrix (osg::Matrix::identity ());
   _camera->setClearMask (GL_DEPTH_BUFFER_BIT);
   _camera->setRenderOrder (osg::Camera::POST_RENDER);
   _camera->setAllowEventFocus (false);

   _add_children (_rootNode, local);

   _camera->addChild (_rootNode.get ());
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzRenderModuleOverlayOSG (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::RenderModuleOverlayOSG (Info, local);
}

};
