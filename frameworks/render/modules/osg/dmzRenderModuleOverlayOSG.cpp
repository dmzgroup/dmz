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

namespace {

typedef dmz::RenderModuleOverlayOSG::LayoutAxis Layout;

class LayoutAbsolute : public Layout {

   public:
      LayoutAbsolute (const dmz::Float64 Offset, const dmz::Boolean Flipped);
      virtual ~LayoutAbsolute () {;}

      virtual dmz::Float64 update (const dmz::Int32 Value);

   protected:
      const dmz::Float64 _Offset;
      const dmz::Boolean _Flipped;
};

LayoutAbsolute::LayoutAbsolute (const dmz::Float64 Offset, const dmz::Boolean Flipped):
      _Offset (Offset),
      _Flipped (Flipped) {;}


dmz::Float64
LayoutAbsolute::update (const dmz::Int32 Value) {

   return _Flipped ? dmz::Float64 (Value) - _Offset : _Offset;
}


class LayoutRelative : public Layout {

   public:
      LayoutRelative (const dmz::Float64 Relative);
      virtual ~LayoutRelative () {;}

      virtual dmz::Float64 update (const dmz::Int32 Value);

   protected:
      const dmz::Float64 _Relative;
};


LayoutRelative::LayoutRelative (const dmz::Float64 Relative):
      _Relative (Relative) {;}


dmz::Float64
LayoutRelative::update (const dmz::Int32 Value) {

   return dmz::Float64(Value) * _Relative;
}

};


dmz::RenderModuleOverlayOSG::RenderModuleOverlayOSG (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      RenderModuleOverlay (Info),
      PortalSizeObserver (Info, local),
      _log (Info),
      _rc (Info, &_log),
      _core (0),
      _cloneStack (0),
      _groupStack (0) {

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
         _templateTable.empty ();
         _colorTable.empty ();
         _textureTable.empty ();
         _transformTable.clear ();
         _switchTable.clear ();
         _groupTable.clear ();
         _layoutTable.empty ();
         _nodeNameTable.clear ();
         _cloneTable.empty ();
         _nodeTable.empty ();
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
dmz::RenderModuleOverlayOSG::lookup_node_handle (const String &Name) {

   Handle result (0);

   NodeStruct *ns = _nodeNameTable.lookup (Name);

   if (ns) { result = ns->VHandle; }

   return result;
}


dmz::Handle
dmz::RenderModuleOverlayOSG::lookup_node_clone_sub_handle (
      const Handle CloneHandle,
      const String &Name) {

   Handle result (0);

   CloneStruct *cs = _cloneTable.lookup (CloneHandle);

   if (cs) {

      NodeStruct *ns = cs->nameTable.lookup (Name);

      result = (ns ? ns->VHandle : 0);
   }

   return result;
}


dmz::String
dmz::RenderModuleOverlayOSG::lookup_node_name (const Handle Overlay) {

   String result;

   NodeStruct *ns = _nodeTable.lookup (Overlay);

   if (ns) { result = ns->Name; }

   return result;
}


dmz::RenderOverlayTypeEnum
dmz::RenderModuleOverlayOSG::lookup_node_type (const Handle Overlay) {

   RenderOverlayTypeEnum result (RenderOverlayUnknown);

   NodeStruct *ns = _nodeTable.lookup (Overlay);

   if (ns) { result = ns->Type; }

   return result;
}


dmz::Boolean
dmz::RenderModuleOverlayOSG::is_of_node_type (
      const Handle Overlay,
      const RenderOverlayTypeEnum Type) {

   Boolean result (False);

   if ((Type == RenderOverlayNode) && _nodeTable.lookup (Overlay)) { result = True; }
   else if ((Type == RenderOverlayGroup) && _groupTable.lookup (Overlay)) {

      result = True;
   }
   else if ((Type == RenderOverlaySwitch) && _switchTable.lookup (Overlay)) {

      result = True;
   }
   else if ((Type == RenderOverlayTransform) && _transformTable.lookup (Overlay)) {

      result = True;
   }

   return result;
}


dmz::Handle
dmz::RenderModuleOverlayOSG::clone_template (const String &Name) {

   Handle result (0);

   Config *def = _templateTable.lookup (Name);

   if (def) {

      osg::ref_ptr<osg::Group> parent = new osg::Group;

      GroupStruct *gs =
         new GroupStruct ("", get_plugin_runtime_context (), parent.get ());

      if (!_register_group (gs)) { delete gs; gs = 0; }
      else if (gs) {

         result = gs->VHandle;

         CloneStruct *cs = new CloneStruct;

         if (cs && !_cloneTable.store (gs->VHandle, cs)) { delete cs; cs = 0; }

         if (cs) {

            cs->next = _cloneStack;
            _cloneStack = cs;
            _add_children (parent, *def);
            _cloneStack = cs->next;
            cs->next = 0;
         }
      }
   }

   return result;
}


dmz::Boolean
dmz::RenderModuleOverlayOSG::destroy_node (const Handle Overlay) {

   Boolean result (False);

   NodeStruct *ns = _nodeTable.lookup (Overlay);

   if (ns) {

      if (ns->Type == RenderOverlayTransform) { result = _remove_transform (Overlay); }
      else if (ns->Type == RenderOverlaySwitch) { result = _remove_switch (Overlay); }
      else if (ns->Type == RenderOverlayGroup) { result = _remove_group (Overlay); }
      else { result = _remove_node (Overlay); }
   }

   return result;
}


// Overlay Group API
dmz::Boolean
dmz::RenderModuleOverlayOSG::add_group_child (const Handle Parent, const Handle Child) {

   Boolean result (False);

   GroupStruct *gs = _groupTable.lookup (Parent);
   NodeStruct *ns = _nodeTable.lookup (Child);

   if (gs && ns) {

      gs->group->addChild (ns->node.get ());

      result = True;
   }

   return result;
}


dmz::Boolean
dmz::RenderModuleOverlayOSG::remove_group_child (
      const Handle Parent,
      const Handle Child) {

   Boolean result (False);

   GroupStruct *gs = _groupTable.lookup (Parent);
   NodeStruct *ns = _nodeTable.lookup (Child);

   if (gs && ns) {

      gs->group->removeChild (ns->node.get ());

      result = True;
   }

   return result;
}


dmz::Int32
dmz::RenderModuleOverlayOSG::lookup_group_child_count (const Handle Overlay) {

   Int32 result (0);

   GroupStruct *gs = _groupTable.lookup (Overlay);

   if (gs && gs->group.valid ()) {

      result = (Int32)gs->group->getNumChildren ();
   }

   return result;
}


// Overlay Switch API
dmz::Boolean
dmz::RenderModuleOverlayOSG::store_switch_state (
      const Handle Overlay,
      const Int32 Which,
      const Boolean SwitchState) {

   Boolean result (False);

   SwitchStruct *ss = _switchTable.lookup (Overlay);

   if (ss && ss->switchNode.valid ()) {

      const UInt32 UWhich (Which);

      if (UWhich < ss->switchNode->getNumChildren ()) {

         ss->switchNode->setValue (UWhich, SwitchState);

         result = True;
      }
   }

   return result;
}


dmz::Boolean
dmz::RenderModuleOverlayOSG::store_all_switch_state (
      const Handle Overlay,
      const Boolean SwitchState) {

   Boolean result (False);

   SwitchStruct *ss = _switchTable.lookup (Overlay);

   if (ss && ss->switchNode.valid ()) {

      if (SwitchState) { ss->switchNode->setAllChildrenOn (); }
      else { ss->switchNode->setAllChildrenOff (); }

      result = True;
   }

   return result;
}


dmz::Boolean
dmz::RenderModuleOverlayOSG::enable_single_switch_state (
      const Handle Overlay,
      const Int32 Which) {

   Boolean result (False);

   SwitchStruct *ss = _switchTable.lookup (Overlay);

   if (ss && ss->switchNode.valid ()) {

      const UInt32 UWhich (Which);

      if (UWhich < ss->switchNode->getNumChildren ()) {

         ss->switchNode->setSingleChildOn (UWhich);

         result = True;
      }
   }

   return result;
}


dmz::Boolean
dmz::RenderModuleOverlayOSG::lookup_switch_state (
      const Handle Overlay,
      const Int32 Which) {

   Boolean result (False);

   SwitchStruct *ss = _switchTable.lookup (Overlay);

   if (ss && ss->switchNode.valid ()) {

      const UInt32 UWhich (Which);

      if (UWhich < ss->switchNode->getNumChildren ()) {

         result = ss->switchNode->getValue (UWhich);
      }
   }

   return result;
}


// Overlay Transform API
dmz::Boolean
dmz::RenderModuleOverlayOSG::store_transform_position (
      const Handle Overlay,
      const Float64 ValueX,
      const Float64 ValueY) {

   Boolean result (False);

   TransformStruct *ts = _transformTable.lookup (Overlay);

   if (ts) {

      ts->pos.set_xyz (ValueX, ValueY, 0.0);

      _apply_transform (*ts);

      result = True;
   }

   return result;
}


dmz::Boolean
dmz::RenderModuleOverlayOSG::lookup_transform_position (
      const Handle Overlay,
      Float64 &valueX,
      Float64 &valueY) {

   Boolean result (False);

   TransformStruct *ts = _transformTable.lookup (Overlay);

   if (ts) {

      valueX = ts->pos.get_x ();
      valueY = ts->pos.get_y ();

      result = True;
   }

   return result;
}


dmz::Boolean
dmz::RenderModuleOverlayOSG::store_transform_rotation (
      const Handle Overlay,
      const Float64 Value) {

   Boolean result (False);

   TransformStruct *ts = _transformTable.lookup (Overlay);

   if (ts) {

      ts->rot = Value;

      _apply_transform (*ts);

      result = True;
   }

   return result;
}


dmz::Boolean
dmz::RenderModuleOverlayOSG::lookup_transform_rotation (
      const Handle Overlay,
      Float64 &value) {

   Boolean result (False);

   TransformStruct *ts = _transformTable.lookup (Overlay);

   if (ts) {

      value = ts->rot;

      result = True;
   }

   return result;
}


dmz::Boolean
dmz::RenderModuleOverlayOSG::store_transform_scale (
      const Handle Overlay,
      const Float64 ValueX,
      const Float64 ValueY) {

   Boolean result (False);

   TransformStruct *ts = _transformTable.lookup (Overlay);

   if (ts) {

      ts->scale.set_xyz (ValueX, ValueY, 0.0);

      _apply_transform (*ts);

      result = True;
   }

   return result;
}


dmz::Boolean
dmz::RenderModuleOverlayOSG::lookup_transform_scale (
      const Handle Overlay,
      Float64 &valueX,
      Float64 &valueY) {

   Boolean result (False);

   TransformStruct *ts = _transformTable.lookup (Overlay);

   if (ts) {

      valueX = ts->scale.get_x ();
      valueY = ts->scale.get_y ();

      result = True;
   }

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

   _update_layout (TheX, TheY);
}


// Protected methods
void
dmz::RenderModuleOverlayOSG::_update_layout (const Int32 TheX, const Int32 TheY) {

   HashTableHandleIterator it;
   LayoutStruct *ls (0);

   while (_layoutTable.get_next (it, ls)) {

      ls->ts.pos.set_xyz (ls->xaxis.update (TheX), ls->yaxis.update (TheY), 0.0);

      _apply_transform (ls->ts);
   }
}


void
dmz::RenderModuleOverlayOSG::_apply_transform (TransformStruct &ts) {

   if (ts.transform.valid ()) {

      osg::Matrix posMat;
      posMat.makeTranslate (ts.pos.get_x (), ts.pos.get_y (), 0.0);

      osg::Matrix rotMat;
      rotMat.makeRotate (ts.rot, osg::Vec3d (0.0, 0.0, 1.0));

      osg::Matrix scaleMat;
      scaleMat.makeScale (ts.scale.get_x (), ts.scale.get_y (), 0.0);

      ts.transform->setMatrix (scaleMat * rotMat * posMat);
   }
}


dmz::RenderModuleOverlayOSG::LayoutAxis *
dmz::RenderModuleOverlayOSG::_create_axis (const String &Prefix, Config &layout) {

   LayoutAxis *result (0);

   const String TypeName = config_to_string (Prefix + ".type", layout);
   const Float64 Value = config_to_float64 (Prefix + ".value", layout);

   if (TypeName == "relative") {

      result = new LayoutRelative (Value);
   }
   else if (TypeName == "absolute") {

      const Boolean Flipped = config_to_boolean (Prefix + ".flip", layout);

      result = new LayoutAbsolute (Value, Flipped);
   }
   else {

      _log.error << "Unknown layout axis type: " << TypeName << endl;
   }

   return result;
}


dmz::RenderModuleOverlayOSG::TextureStruct *
dmz::RenderModuleOverlayOSG::_create_texture (const String &Name) {

   TextureStruct *result (_textureTable.lookup (Name));

   if (Name && !result) {

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


dmz::Boolean
dmz::RenderModuleOverlayOSG::_register_node (NodeStruct *ptr) {

   Boolean result (False);

   if (ptr && _nodeTable.store (ptr->VHandle, ptr)) {

      result = True;

      if (ptr->Name) {

         if (_cloneStack) {

            if (!_cloneStack->nameTable.store (ptr->Name, ptr)) {

            }
         }
         else if (!_nodeNameTable.store (ptr->Name, ptr)) {

         }

         if (_groupStack) { _groupStack->gs.childTable.store (ptr->VHandle, ptr); }
      }
   }

   return result;
}


dmz::Boolean
dmz::RenderModuleOverlayOSG::_register_group (GroupStruct *ptr) {

   Boolean result (_register_node (ptr));

   if (result && ptr) {

      _groupTable.store (ptr->VHandle, ptr);
   }

   return result;
}


dmz::Boolean
dmz::RenderModuleOverlayOSG::_register_switch (SwitchStruct *ptr) {

   Boolean result (_register_group (ptr));

   if (result && ptr) {

      _switchTable.store (ptr->VHandle, ptr);
   }

   return result;
}


dmz::Boolean
dmz::RenderModuleOverlayOSG::_register_transform (TransformStruct *ptr) {

   Boolean result (_register_group (ptr));

   if (result && ptr) {

      result = _transformTable.store (ptr->VHandle, ptr);
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
   else if (TypeName == "clone") { _add_clone (parent, node); }
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

   const String Name = config_to_string ("name", node);

   Boolean popStack = False;

   if (Name) {

      GroupStruct *gs =
         new GroupStruct (Name, get_plugin_runtime_context (), child.get ());

      if (!_register_group (gs)) { delete gs; gs = 0; }
      else if (gs) {

         GroupStackStruct *gss = new GroupStackStruct (*gs);

         if (gss) {

            gss->next = _groupStack;
            _groupStack = gss;
            popStack = True;
         }
      }
   }

   _add_children (child, node);

   if (popStack && _groupStack) {

      GroupStackStruct *gss = _groupStack;
      _groupStack = _groupStack->next;

      if (gss) { delete gss; gss = 0; }
   }
}


void
dmz::RenderModuleOverlayOSG::_add_switch (
      osg::ref_ptr<osg::Group> &parent,
      Config &node) {

   osg::ref_ptr<osg::Switch> ptr = new osg::Switch;
   parent->addChild (ptr.get ());
 
   const String Name = config_to_string ("name", node);
   const UInt32 Which = config_to_uint32 ("which", node);
   SwitchStruct *ss = 0;

   if (Name) {

      ss = new SwitchStruct (Name, get_plugin_runtime_context (), ptr.get ());

      if (!_register_switch (ss)) { delete ss; ss = 0; }
   }  

   osg::ref_ptr<osg::Group> child = ptr.get ();
   _add_children (child, node);

   if (ss && ss->switchNode.valid ()) {

      if (Which < ss->switchNode->getNumChildren ()) {

         ss->switchNode->setSingleChildOn (Which);
      }
   }
}


void
dmz::RenderModuleOverlayOSG::_add_transform (
      osg::ref_ptr<osg::Group> &parent,
      Config &node) {

   osg::ref_ptr<osg::MatrixTransform> ptr = new osg::MatrixTransform;
   parent->addChild (ptr.get ());

   const Vector Pos = config_to_vector ("position", node);
   const Float64 Rot = config_to_float64 ("rotation.value", node);
   const Vector Scale = config_to_vector ("scale", node, Vector (1.0, 1.0, 1.0));

   osg::Matrix posMat;
   posMat.makeTranslate (Pos.get_x (), Pos.get_y (), 0.0);

   osg::Matrix rotMat;
   rotMat.makeRotate (Rot, osg::Vec3d (0.0, 0.0, 1.0));

   osg::Matrix scaleMat;
   scaleMat.makeScale (Scale.get_x (), Scale.get_y (), 0.0);

   ptr->setMatrix (scaleMat * rotMat * posMat);
 
   const String Name = config_to_string ("name", node);

   if (Name) {

      TransformStruct *ts =
         new TransformStruct (Name, get_plugin_runtime_context (), ptr.get ());

      if (!_register_transform (ts)) { delete ts; ts = 0; }
      else if (ts) {

         ts->pos = Pos;
         ts->rot = Rot;
         ts->scale = Scale;
      }
   }  

   osg::ref_ptr<osg::Group> child = ptr.get ();
   _add_children (child, node);
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
   const Int32 Depth = config_to_int32 ("depth.value", node);
   const Vector TMin = config_to_vector ("texture.min", node);
   const Vector TMax = config_to_vector ("texture.max", node, Vector (1.0, 1.0, 1.0));

   osg::Geode* geode = new osg::Geode ();
   osg::StateSet* stateset = geode->getOrCreateStateSet ();
   stateset->setMode (GL_LIGHTING, osg::StateAttribute::OFF);
   stateset->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);
   stateset->setRenderBinDetails (Depth, "RenderBin");

   osg::Geometry* geom = new osg::Geometry;

   osg::Vec3Array* normals = new osg::Vec3Array;
   normals->push_back (osg::Vec3 (0.0f, 0.0f, 1.0f));
   geom->setNormalArray (normals);
   geom->setNormalBinding (osg::Geometry::BIND_OVERALL);

   osg::Vec4Array* colors = new osg::Vec4Array;

   osg::Vec4 color (1.0, 1.0, 1.0, 1.0);

   const String ColorName = config_to_string ("color.name", node);

   if (ColorName) {

      osg::Vec4 *ptr = _colorTable.lookup (ColorName);

      if (ptr) { color = *ptr; }
      else { _log.error << "Unknown color: " << ColorName << endl; }
   }
   else { color = config_to_osg_vec4_color ("color", node, color); }

   colors->push_back (color);

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
   vertices->push_back (osg::Vec3 (minw, maxh, 0.0));
   vertices->push_back (osg::Vec3 (minw, minh, 0.0));
   vertices->push_back (osg::Vec3 (maxw, minh, 0.0));
   vertices->push_back (osg::Vec3 (maxw, maxh, 0.0));
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
dmz::RenderModuleOverlayOSG::_add_clone (osg::ref_ptr<osg::Group> &parent, Config &node) {

   osg::ref_ptr<osg::Group> child = new osg::Group;
   parent->addChild (child.get ());

   const String Name = config_to_string ("name", node);
   const String Clone = config_to_string ("template", node);

   Config *def = _templateTable.lookup (Clone);

   if (def) {

      GroupStruct *gs =
         new GroupStruct (Name, get_plugin_runtime_context (), child.get ());

      if (!_register_group (gs)) { delete gs; gs = 0; }
      else if (gs) {

         CloneStruct *cs = new CloneStruct;

         if (cs && !_cloneTable.store (gs->VHandle, cs)) {

            delete cs; cs = 0;
         }

         if (cs) {

            cs->next = _cloneStack;
            _cloneStack = cs;
            _add_children (child, *def);
            _cloneStack = cs->next;
            cs->next = 0;
         }
      }
   }
   else { _log.error << "Clone failed. Unknown template: " << Clone << endl; }
}


dmz::Boolean
dmz::RenderModuleOverlayOSG::_remove_node (const Handle Overlay) {

   Boolean result (False);

   NodeStruct *ns = _nodeTable.remove (Overlay);

   if (ns && ns->node.valid ()) {

      const unsigned int ParentCount = ns->node->getNumParents ();

      if (ParentCount) {

         for (unsigned int ix = 0; ix < ParentCount; ix--) {

            osg::Group *parent = ns->node->getParent (ix);

            if (parent) { parent->removeChild (ns->node.get ()); }
         }
      }

      result = True;
      delete ns; ns = 0;
   }

   return result;
}


dmz::Boolean
dmz::RenderModuleOverlayOSG::_remove_group (const Handle Overlay) {

   GroupStruct *gs = _groupTable.remove (Overlay);

   if (gs) {

      HashTableHandleIterator it;
      NodeStruct *ns (0);

      while (gs->childTable.get_next (it, ns)) { destroy_node (ns->VHandle); }
   }

   return (gs != 0) && _remove_node (Overlay);
}


dmz::Boolean
dmz::RenderModuleOverlayOSG::_remove_switch (const Handle Overlay) {

   const Boolean Result = (_switchTable.remove (Overlay) != 0);

   return Result && _remove_group (Overlay);
}


dmz::Boolean
dmz::RenderModuleOverlayOSG::_remove_transform (const Handle Overlay) {

   const Boolean Result = (_transformTable.remove (Overlay) != 0);

   return Result && _remove_group (Overlay);
}


void
dmz::RenderModuleOverlayOSG::_init_colors (Config &local) {

   Config colorList;

   if (local.lookup_all_config ("color", colorList)) {

      ConfigIterator it;
      Config def;

      while (colorList.get_next_config (it, def)) {

         const String Name = config_to_string ("name", def);

         if (Name) {

            osg::Vec4 *color = new osg::Vec4 (
               config_to_osg_vec4_color (def, osg::Vec4 (1.0f, 1.0, 1.0f, 1.0f)));

            if (color && !_colorTable.store (Name, color)) {

               delete color; color = 0;

               _log.error << "Failed to store color named: " << Name
                  << ". The name is probably already in use." << endl;
            }
         }
         else {

            _log.error << "Unable to store unnamed color." << endl;
         }
      }
   }
}


void
dmz::RenderModuleOverlayOSG::_init_templates (Config &local) {

   Config templateList;

   if (local.lookup_all_config ("template", templateList)) {

      ConfigIterator it;
      Config def;

      while (templateList.get_next_config (it, def)) {

         const String Name = config_to_string ("name", def);

         if (Name) {

            Config *ptr = new Config (def);

            if (ptr && !_templateTable.store (Name, ptr)) {

               delete ptr; ptr = 0;

               _log.error << "Failed to store overlay template named: " << Name
                  << ". The name is probably already in use." << endl;
            }
         }
         else {

            _log.error << "Unable to store unnamed overlay template." << endl;
         }
      }
   }
}


void
dmz::RenderModuleOverlayOSG::_init_layout (Config &local) {

   Config layoutList;

   if (local.lookup_all_config ("layout", layoutList)) {

      ConfigIterator it;
      Config layout;

      while (layoutList.get_next_config (it, layout)) {

         const String Name = config_to_string ("node", layout);

         NodeStruct *ns = _nodeNameTable.lookup (Name);

         TransformStruct *ts (ns ? _transformTable.lookup (ns->VHandle) : 0);

         if (ts) {

            LayoutAxis *xaxis = _create_axis ("x", layout);
            LayoutAxis *yaxis = _create_axis ("y", layout);

            if (xaxis && yaxis) {

               LayoutStruct *ls = new LayoutStruct (*xaxis, *yaxis, *ts);

               if (ls && !_layoutTable.store (ts->VHandle, ls)) {

                  delete ls; ls = 0;
               }
            }
         }
         else {

            _log.error << "Unable to find transform: " << Name << " for layout" << endl;
         }
      }
   }
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

   _init_colors (local);
   _init_templates (local);

   _add_children (_rootNode, local);

   _camera->addChild (_rootNode.get ());

   _init_layout (local);

   _update_layout (get_portal_x (), get_portal_y ());
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
