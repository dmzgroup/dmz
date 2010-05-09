#include "dmzRenderModuleOverlayOSG.h"
#include <dmzRenderModuleCoreOSG.h>
#include <dmzRenderUtilOSG.h>
#include <dmzRenderConfigToOSG.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeConfigToVector.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzTypesConsts.h>

#include <osg/Texture2D>
#include <osg/PolygonMode>
#include <osg/LineWidth>
#include <osgDB/ReadFile>

namespace {

typedef dmz::RenderModuleOverlayOSG::LayoutAxis LayoutAxis;
typedef dmz::RenderModuleOverlayOSG::ScaleAxis ScaleAxis;

class LayoutAbsolute : public LayoutAxis {

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


class LayoutRelative : public LayoutAxis {

   public:
      LayoutRelative (const dmz::Float64 Relative, const dmz::Float64 Offset);
      virtual ~LayoutRelative () {;}

      virtual dmz::Float64 update (const dmz::Int32 Value);

      void set_max (const dmz::Float64 Value);
      void set_min (const dmz::Float64 Value);

   protected:
      const dmz::Float64 _Relative;
      const dmz::Float64 _Offset;
      dmz::Boolean _hasMax;
      dmz::Float64 _max;
      dmz::Boolean _hasMin;
      dmz::Float64 _min;
};


LayoutRelative::LayoutRelative (
      const dmz::Float64 Relative,
      const dmz::Float64 Offset):
      _Relative (Relative),
      _Offset (Offset),
      _hasMax (dmz::False),
      _max (0.0),
      _hasMin (dmz::False),
      _min (0.0) {;}


dmz::Float64
LayoutRelative::update (const dmz::Int32 Value) {

   dmz::Float64 result = (dmz::Float64(Value) * _Relative) - _Offset;

   if (_hasMax && (result > _max)) { result = _max; }

   if (_hasMin && (result < _min)) { result = _min; }

   return result;
}


void
LayoutRelative::set_max (const dmz::Float64 Value) {

   _hasMax = dmz::True;
   _max = Value;
}


void
LayoutRelative::set_min (const dmz::Float64 Value) {

   _hasMin = dmz::True;
   _min = Value;
}


class ScaleAxisSimple : public ScaleAxis {

   public:
      ScaleAxisSimple (
         const dmz::Float64 Size,
         const dmz::Float64 Ratio,
         const dmz::Float64 Offset);

      virtual dmz::Float64 update (const dmz::Int32 Value);

      void set_max (const dmz::Float64 Value);
      void set_min (const dmz::Float64 Value);

   protected:
      const dmz::Float64 _Size;
      const dmz::Float64 _Ratio;
      const dmz::Float64 _Offset;
      dmz::Boolean _hasMax;
      dmz::Float64 _max;
      dmz::Boolean _hasMin;
      dmz::Float64 _min;
};


ScaleAxisSimple::ScaleAxisSimple (
      const dmz::Float64 Size,
      const dmz::Float64 Ratio,
      const dmz::Float64 Offset) :
      _Size (dmz::is_zero64 (Size) ? 1.0 : Size),
      _Ratio (Ratio),
      _Offset (Offset),
      _hasMax (dmz::False),
      _max (0.0),
      _hasMin (dmz::False),
      _min (0.0) {;}


dmz::Float64
ScaleAxisSimple::update (const dmz::Int32 Value) {

   dmz::Float64 size = (dmz::Float64 (Value) * _Ratio) - _Offset;

   if (_hasMax && (_max < size)) { size = _max; }
   if (_hasMin && (_min > size)) { size = _min; }

   return size / _Size;
}


void
ScaleAxisSimple::set_max (const dmz::Float64 Value) {

   _hasMax = dmz::True;
   _max = Value;
}


void
ScaleAxisSimple::set_min (const dmz::Float64 Value) {

   _hasMin = dmz::True;
   _min = Value;
}


class ScaleAxisFixed : public ScaleAxis {

   public:
      ScaleAxisFixed (const dmz::Float64 Scale);

      virtual dmz::Float64 update (const dmz::Int32 Value);

   protected:
      const dmz::Float64 _Scale;
};


ScaleAxisFixed::ScaleAxisFixed (const dmz::Float64 Scale) : _Scale (Scale) {;}


dmz::Float64
ScaleAxisFixed::update (const dmz::Int32 Value) { return _Scale; }

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
         _textTable.clear ();
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
dmz::Boolean
dmz::RenderModuleOverlayOSG::lookup_named_color (
      const String &Name,
      Float64 &red,
      Float64 &green,
      Float64 &blue,
      Float64 &alpha) {

   Boolean result (False);

   osg::Vec4 *color = _colorTable.lookup (Name);

   if (color) {

      result = True;

      red = color->r ();
      green = color->g ();
      blue = color->b ();
      alpha = color->a ();
   }

   return result;
}


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
   else if ((Type == RenderOverlayText) && _textTable.lookup (Overlay)) {

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
      else if (ns->Type == RenderOverlayText) { result = _remove_text (Overlay); }
      else { result = _remove_node (Overlay); }
   }

   CloneStruct *cs = _cloneTable.remove (Overlay);

   if (cs) {

      HashTableStringIterator it;
      NodeStruct *child = 0;
      while (cs->nameTable.get_next (it, child)) { destroy_node (child->VHandle); }
      delete cs; cs = 0;
   }

   return result;
}

         // Overlay Node API
dmz::Boolean
dmz::RenderModuleOverlayOSG::store_color (
      const Handle Overlay,
      const Float64 Red,
      const Float64 Green,
      const Float64 Blue,
      const Float64 Alpha) {

   Boolean result (False);

   NodeStruct *ns = _nodeTable.lookup (Overlay);

   if (ns) {

      result = True;
      const osg::Vec4 Color (Red, Green, Blue, Alpha);
      _set_color (Color, ns->node.get ());
   }

   return result;
}


dmz::Boolean
dmz::RenderModuleOverlayOSG::lookup_color (
      const Handle Overlay,
      Float64 &red,
      Float64 &green,
      Float64 &blue,
      Float64 &alpha) {

   Boolean result (False);

   return result;
}


// Overlay Text API
dmz::Boolean
dmz::RenderModuleOverlayOSG::store_text (const Handle Overlay, const String &Value) {

   Boolean result (False);

   TextStruct *ts (_textTable.lookup (Overlay));

   if (ts && ts->text.valid ()) {

      ts->text->setText (Value.get_buffer ());
      ts->value = Value;
      result = True;
   }

   return result;
}


dmz::Boolean
dmz::RenderModuleOverlayOSG::lookup_text (const Handle Overlay, String &value) {

   Boolean result (False);

   TextStruct *ts (_textTable.lookup (Overlay));

   if (ts) {

      value = ts->value;
      result = True;
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
dmz::RenderModuleOverlayOSG::store_switch_state_all (
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
dmz::RenderModuleOverlayOSG::enable_switch_state_single (
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
   _update_auto_scale (TheX, TheY);
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
dmz::RenderModuleOverlayOSG::_update_auto_scale (const Int32 TheX, const Int32 TheY) {

   HashTableHandleIterator it;
   ScaleStruct *ss (0);

   while (_scaleTable.get_next (it, ss)) {

      Float64 ix = 1.0;
      Float64 jy = 1.0;

      if (ss->xaxis) {

         ix = ss->xaxis->update (TheX);

         if (!ss->yaxis) { jy = ix; }
      }

      if (ss->yaxis) {

         jy = ss->yaxis->update (TheY);

         if (!ss->xaxis) { ix = jy; }
      }

      ss->ts.scale.set_xyz (ix, jy, 0.0);

      _apply_transform (ss->ts);
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
dmz::RenderModuleOverlayOSG::_create_layout_axis (const String &Prefix, Config &layout) {

   LayoutAxis *result (0);

   Config axis;

   if (layout.lookup_config (Prefix, axis)) {

      const String TypeName = config_to_string ("type", axis);
      const Float64 Value = config_to_float64 ("value", axis);

      if (TypeName == "relative") {

         const Float64 Offset = config_to_float64 ("offset", axis);

         LayoutRelative *lr = new LayoutRelative (Value, Offset);

         if (lr) {

            String value;

            if (axis.lookup_attribute ("min", value)) {

               lr->set_min (string_to_float64 (value));
            }

            if (axis.lookup_attribute ("max", value)) {

               lr->set_max (string_to_float64 (value));
            }

            result = lr;
         }
      }
      else if (TypeName == "absolute") {

         const Boolean Flipped = config_to_boolean ("flip", axis);

         result = new LayoutAbsolute (Value, Flipped);
      }
      else {

         _log.error << "Unknown layout axis type: " << TypeName << " for axis: "
            << Prefix << endl;
      }
   }

   return result;
}


dmz::RenderModuleOverlayOSG::ScaleAxis *
dmz::RenderModuleOverlayOSG::_create_scale_axis (const String &Prefix, Config &data) {

   ScaleAxis *result (0);

   Config axis;

   if (data.lookup_config (Prefix, axis)) {

      const String TypeName = config_to_string ("type", axis, "simple");

      if (TypeName == "simple") {

         ScaleAxisSimple *sas = new ScaleAxisSimple (
            config_to_float64 ("size", axis, 1.0),
            config_to_float64 ("ratio", axis, 1.0),
            config_to_float64 ("offset", axis, 1.0));

         if (sas) {

            String value;

            if (axis.lookup_attribute ("min", value)) {

               sas->set_min (string_to_float64 (value));
            }

            if (axis.lookup_attribute ("max", value)) {

               sas->set_max (string_to_float64 (value));
            }

            result = sas;
         }
      }
      else if (TypeName == "fixed") {

         result = new ScaleAxisFixed (config_to_float64 ("value", axis, 1.0));
      }
      else {

         _log.error << "Unknown auto scale axis type: " << TypeName << endl;
      }
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


osg::Vec4
dmz::RenderModuleOverlayOSG::_config_to_color (Config &data) {

   osg::Vec4 result (1.0, 1.0, 1.0, 1.0);

   const String ColorName = config_to_string ("color.name", data);

   if (ColorName) {

      osg::Vec4 *ptr = _colorTable.lookup (ColorName);

      if (ptr) { result = *ptr; }
      else { _log.error << "Unknown color: " << ColorName << endl; }
   }
   else { result = config_to_osg_vec4_color ("color", data, result); }

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
dmz::RenderModuleOverlayOSG::_register_text (TextStruct *ptr) {

   Boolean result (_register_node (ptr));

   if (result && ptr) {

      _textTable.store (ptr->VHandle, ptr);
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
   else if (TypeName == "text") { _add_text (parent, node); }
   else if (TypeName == "switch") { _add_switch (parent, node); }
   else if (TypeName == "transform") { _add_transform (parent, node); }
   else if (TypeName == "box") { _add_box (parent, node); }
   else if (TypeName == "circle") { _add_circle (parent, node); }
   else if (TypeName == "clone") { _add_clone (parent, node); }
   else {

      _log.error << "Unknown overlay node type: " << TypeName << endl;
   }
}


void
dmz::RenderModuleOverlayOSG::_add_text (
      osg::ref_ptr<osg::Group> &parent,
      Config &node) {

   osg::ref_ptr<osg::Geode> child = new osg::Geode;
   osg::ref_ptr<osgText::Text> textNode = new osgText::Text;

   if (child.valid () && textNode.valid ()) {

      const String Name = config_to_string ("name", node);
      const Int32 Depth = config_to_int32 ("depth.value", node);
      const String FontResource = config_to_string ("font.resource", node);
      const Vector Pos = config_to_vector ("position", node);

      String text;
      Config data;

      if (node.lookup_config ("text", data)) {

         if (!data.lookup_attribute ("value", text)) {

            data.get_value (text);
         }
      }

      osg::StateSet* stateset = child->getOrCreateStateSet ();
      stateset->setMode (GL_LIGHTING, osg::StateAttribute::OFF);
      stateset->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);
      stateset->setRenderBinDetails (Depth, "RenderBin");

      if (FontResource) {

         const String FontName = _rc.find_file (FontResource);

         if (FontName) { textNode->setFont (FontName.get_buffer ()); }
      }

      textNode->setAxisAlignment (osgText::TextBase::XY_PLANE);
      textNode->setBackdropType (osgText::Text::NONE);
      if (text) { textNode->setText (text.get_buffer ()); }
      textNode->setPosition (osg::Vec3 (Pos.get_x (), Pos.get_y (), 0.0));
      textNode->setColor (_config_to_color (node));

      textNode->setCharacterSize (
         config_to_float32 ("height.value", node, textNode->getCharacterHeight ()));

      parent->addChild (child.get ());
      child->addDrawable (textNode.get ());

      if (Name) {

         TextStruct *ts = new TextStruct (
            Name,
            text,
            get_plugin_runtime_context (),
            child.get (),
            textNode.get ());

         if (ts && !_register_text (ts)) { delete ts; ts = 0; }
      }
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
   const Int32 Which = config_to_int32 ("which", node);
   SwitchStruct *ss = 0;

   if (Name) {

      ss = new SwitchStruct (Name, get_plugin_runtime_context (), ptr.get ());

      if (!_register_switch (ss)) { delete ss; ss = 0; }
   }  

   osg::ref_ptr<osg::Group> child = ptr.get ();
   _add_children (child, node);

   if (ss && ss->switchNode.valid ()) {

      if (Which < 0) {

         ss->switchNode->setAllChildrenOff ();
      }
      else if (UInt32 (Which) < ss->switchNode->getNumChildren ()) {

         ss->switchNode->setSingleChildOn (Which);
      }
   }
}

namespace {

static const dmz::Float64 SixPi64 = dmz::Pi64 * 6.0;
static const dmz::Float64 SevenPi64 = dmz::Pi64 * 7.0;

}

void
dmz::RenderModuleOverlayOSG::_add_transform (
      osg::ref_ptr<osg::Group> &parent,
      Config &node) {

   osg::ref_ptr<osg::MatrixTransform> ptr = new osg::MatrixTransform;
   parent->addChild (ptr.get ());

   const Vector Pos = config_to_vector ("position", node);
   Float64 rot  = config_to_float64 ("rotation.value", node, SevenPi64);

   if (rot > SixPi64) {

      rot = to_radians (config_to_float64 ("rotation.degrees", node, 0.0));
   }

   const Vector Scale = config_to_vector ("scale", node, Vector (1.0, 1.0, 1.0));

   osg::Matrix posMat;
   posMat.makeTranslate (Pos.get_x (), Pos.get_y (), 0.0);

   osg::Matrix rotMat;
   rotMat.makeRotate (rot, osg::Vec3d (0.0, 0.0, 1.0));

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
         ts->rot = rot;
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

   const String Name = config_to_string ("name", node);
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

   colors->push_back (_config_to_color (node));

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

   if (Name) {

      NodeStruct *ns = new NodeStruct (Name, get_plugin_runtime_context (), geode);

      if (!_register_node (ns)) { delete ns; ns = 0; }
   }
}


void
dmz::RenderModuleOverlayOSG::_add_circle (
      osg::ref_ptr<osg::Group> &parent,
      Config &node) {

   const Vector Center = config_to_vector ("center", node);
   const Float64 Radius = config_to_float64 ("radius.value", node);
   const Float64 Width = config_to_float64 ("width.value", node, 1.0);
   const Int32 Depth = config_to_int32 ("depth.value", node);

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

   const Int32 Segments = 64;

   geom->addPrimitiveSet (
      new osg::DrawArrays (osg::PrimitiveSet::LINE_LOOP, 0, Segments));

   stateset = geom->getOrCreateStateSet ();
   stateset->setRenderingHint (osg::StateSet::TRANSPARENT_BIN);
   stateset->setAttributeAndModes (new osg::LineWidth (Width));

   const Float64 X = Center.get_x ();
   const Float64 Y = Center.get_y ();
   const Float64 Angle = TwoPi64 / Segments;

   osg::Vec3Array* vertices = new osg::Vec3Array;

   Float64 current (0.0);

   for (Int32 ix = 0; ix < Segments; ix++) {

      vertices->push_back (
         osg::Vec3 (X + (sin (current) * Radius), Y + (cos (current) * Radius), 0.0));

      current = current + Angle;
   }

   geom->setVertexArray (vertices);

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

      const int ParentCount = (int)ns->node->getNumParents ();

      if (ParentCount) {

         for (int ix = (ParentCount - 1); ix >= 0; ix--) {

            osg::Group *parent = ns->node->getParent ((unsigned int)ix);

            if (parent) { parent->removeChild (ns->node.get ()); }
         }
      }

      result = True;
      delete ns; ns = 0;
   }

   return result;
}


dmz::Boolean
dmz::RenderModuleOverlayOSG::_remove_text (const Handle Overlay) {

   const Boolean Result = (_textTable.remove (Overlay) != 0);

   return Result && _remove_node (Overlay);
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

   TransformStruct *ts = _transformTable.remove (Overlay);

   const Boolean Result = (ts ? True : False);

   if (ts) {

      LayoutStruct *ls = _layoutTable.remove (ts->VHandle);
      if (ls) { delete ls; ls = 0; }

      ScaleStruct *ss = _scaleTable.remove (ts->VHandle);
      if (ss) { delete ss; ss = 0; }
   }

   return Result && _remove_group (Overlay);
}


void
dmz::RenderModuleOverlayOSG::_set_color (const osg::Vec4 &Color, osg::Node *node) {

   if (node) {

      osg::Group *group = dynamic_cast<osg::Group *> (node);

      if (group) {

         const unsigned int Count = group->getNumChildren ();

         for (unsigned int ix = 0; ix < Count; ix++) {

            osg::Node *child = group->getChild (ix);

            _set_color (Color, child);
         }
      }
      else {

         osg::Geode *geode = dynamic_cast<osg::Geode *> (node);

         if (geode) {

            const unsigned int Count = geode->getNumDrawables ();

            for (unsigned int ix = 0; ix < Count; ix++) {

               osg::Drawable *child = geode->getDrawable (ix);

               osg::Geometry *geom = dynamic_cast<osg::Geometry *> (child);

               if (geom) {

                  osg::Vec4Array* colors = new osg::Vec4Array;
                  colors->push_back (Color);
                  geom->setColorArray (colors);
               }
               else {

                  osgText::Text *text = dynamic_cast<osgText::Text *> (child);

                  if (text) { text->setColor (Color); }
               }
            }
         }
      }
   }
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

            LayoutAxis *xaxis = _create_layout_axis ("x", layout);
            LayoutAxis *yaxis = _create_layout_axis ("y", layout);

            if (xaxis && yaxis) {

               LayoutStruct *ls = new LayoutStruct (*xaxis, *yaxis, *ts);

               if (ls && !_layoutTable.store (ts->VHandle, ls)) {

                  delete ls; ls = 0;
               }
            }

            if (!xaxis) {

               _log.error << "Layout X axis not defined for node: " << Name << endl;
            }

            if (!yaxis) {

               _log.error << "Layout Y axis not defined for node: " << Name << endl;
            }
         }
         else {

            _log.error << "Unable to find transform: " << Name << " for layout" << endl;
         }
      }
   }
}


void
dmz::RenderModuleOverlayOSG::_init_auto_scale (Config &local) {

   Config scaleList;

   if (local.lookup_all_config ("auto-scale", scaleList)) {

      ConfigIterator it;
      Config scaleData;

      while (scaleList.get_next_config (it, scaleData)) {

         const String Name = config_to_string ("node", scaleData);

         NodeStruct *ns = _nodeNameTable.lookup (Name);

         TransformStruct *ts (ns ? _transformTable.lookup (ns->VHandle) : 0);

         if (ts) {

            ScaleStruct *ss = new ScaleStruct (
               _create_scale_axis ("x", scaleData),
               _create_scale_axis ("y", scaleData),
               *ts);

            if (ss && !_scaleTable.store (ts->VHandle, ss)) {

               delete ss; ss = 0;
            }
         }
         else {

            _log.error << "Unable to find transform: " << Name << " for auto scale"
               << endl;
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

   _init_auto_scale (local);

   _update_auto_scale (get_portal_x (), get_portal_y ());
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
