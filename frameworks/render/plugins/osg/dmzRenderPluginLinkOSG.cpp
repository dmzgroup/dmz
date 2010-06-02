#include <dmzObjectAttributeMasks.h>
#include <dmzObjectModule.h>
#include <dmzRenderConfigToOSG.h>
#include <dmzRenderConsts.h>
#include <dmzRenderModuleCoreOSG.h>
#include <dmzRenderObjectDataOSG.h>
#include "dmzRenderPluginLinkOSG.h"
#include <dmzRenderUtilOSG.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigToNamedHandle.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzTypesMatrix.h>

#include <osg/CullFace>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Material>

dmz::RenderPluginLinkOSG::RenderPluginLinkOSG (const PluginInfo &Info, Config &local) :
      Plugin (Info),
      TimeSlice (Info),
      MessageObserver (Info),
      ObjectObserverUtil (Info, local),
      _log (Info),
      _defaultAttrHandle (0),
      _render (0),
      _masterMask (0),
      _glyphMask (0),
      _entityMask (0) {

   _init (local);
}


dmz::RenderPluginLinkOSG::~RenderPluginLinkOSG () {

   _defTable.empty ();
   _linkTable.empty ();
   _objTable.empty ();
}


// Plugin Interface
void
dmz::RenderPluginLinkOSG::update_plugin_state (
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
dmz::RenderPluginLinkOSG::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      if (!_render) {

         _render = RenderModuleCoreOSG::cast (PluginPtr);

         if (_render) {

            _masterMask = _render->get_master_isect_mask ();
            _glyphMask = _render->lookup_isect_mask (RenderIsectGlyphName);
            _entityMask = _render->lookup_isect_mask (RenderIsectEntityName);

            HashTableHandleIterator it;
            LinkDefStruct *def (0);

            while (_defTable.get_next (it, def)) { _create_geometry (*def); }

            it.reset ();
            LinkStruct *link (0);

            while (_linkTable.get_next (it, link)) { _create_link (*link); }
         }
      }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_render && (_render == RenderModuleCoreOSG::cast (PluginPtr))) {

         _masterMask = 0;
         _glyphMask = 0;
         _entityMask = 0;
         _render = 0;
      }
   }
}


// TimeSlice Interface
void
dmz::RenderPluginLinkOSG::update_time_slice (const Float64 TimeDelta) {

}


// Message Observer Interface
void
dmz::RenderPluginLinkOSG::receive_message (
      const Message &Type,
      const UInt32 MessageSendHandle,
      const Handle TargetObserverHandle,
      const Data *InData,
      Data *outData) {

}


// Object Observer Interface
void
dmz::RenderPluginLinkOSG::destroy_object (
      const UUID &Identity,
      const Handle ObjectHandle) {

   ObjectStruct *os = _objTable.remove (ObjectHandle);

   if (os) { delete os; os = 0; }
}


void
dmz::RenderPluginLinkOSG::link_objects (
      const Handle LinkHandle,
      const Handle AttributeHandle,
      const UUID &SuperIdentity,
      const Handle SuperHandle,
      const UUID &SubIdentity,
      const Handle SubHandle) {

   LinkDefStruct *lds = _defTable.lookup (AttributeHandle);

   if (lds) {

      ObjectStruct *super = _lookup_object (SuperHandle);
      ObjectStruct *sub = _lookup_object (SubHandle);

      if (super && sub) {

         LinkStruct *ls = new LinkStruct (LinkHandle, *lds, *super, *sub);

         if (ls && _linkTable.store (LinkHandle, ls)) {

            _create_link (*ls);
         }
         else if (ls) { delete ls; ls = 0; }
      }
   }
}


void
dmz::RenderPluginLinkOSG::unlink_objects (
      const Handle LinkHandle,
      const Handle AttributeHandle,
      const UUID &SuperIdentity,
      const Handle SuperHandle,
      const UUID &SubIdentity,
      const Handle SubHandle) {

   LinkStruct *ls = _linkTable.remove (LinkHandle);

   if (ls) {

      if (ls->root.valid () && _render) {

         osg::ref_ptr<osg::Group> scene = _render->get_dynamic_objects ();

         if (scene.valid ()) { scene->removeChild (ls->root.get ()); }
      }

      delete ls; ls = 0;
   }
}


void
dmz::RenderPluginLinkOSG::update_link_attribute_object (
      const Handle LinkHandle,
      const Handle AttributeHandle,
      const UUID &SuperIdentity,
      const Handle SuperHandle,
      const UUID &SubIdentity,
      const Handle SubHandle,
      const UUID &AttributeIdentity,
      const Handle AttributeObjectHandle,
      const UUID &PrevAttributeIdentity,
      const Handle PrevAttributeObjectHandle) {

}


void
dmz::RenderPluginLinkOSG::update_object_state (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Mask &Value,
      const Mask *PreviousValue) {

}


void
dmz::RenderPluginLinkOSG::update_object_flag (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Boolean Value,
      const Boolean *PreviousValue) {

}


void
dmz::RenderPluginLinkOSG::update_object_position (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Value,
      const Vector *PreviousValue) {

   ObjectStruct *os = _objTable.lookup (ObjectHandle);

   if (os) {

      os->pos = Value;

      HashTableHandleIterator it;
      LinkStruct *link (0);

      while (os->superTable.get_next (it, link)) { _update_link (*link); }
      it.reset ();
      link = 0;
      while (os->subTable.get_next (it, link)) { _update_link (*link); }
   }
}


void
dmz::RenderPluginLinkOSG::update_object_vector (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Value,
      const Vector *PreviousValue) {

}


void
dmz::RenderPluginLinkOSG::update_object_scalar (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Float64 Value,
      const Float64 *PreviousValue) {

}


// RenderPluginLinkOSG Interface
dmz::RenderPluginLinkOSG::ObjectStruct *
dmz::RenderPluginLinkOSG::_lookup_object (const Handle Object) {

   ObjectStruct *result = _objTable.lookup (Object);

   if (!result) {

      result = new ObjectStruct (Object);

      if (result && _objTable.store (Object, result)) { 

         ObjectModule *module = get_object_module ();

         if (module) {

            module->lookup_position (Object, _defaultAttrHandle, result->pos);
         }
      }
      else if (result) { delete result; result = 0; }
   }

   return result;
}


void
dmz::RenderPluginLinkOSG::_create_link (LinkStruct &ls) {

   if (_render) {

      ls.root = new osg::MatrixTransform;

      ls.root->setUserData (new RenderObjectDataOSG (ls.Link));
      ls.root->setDataVariance (osg::Object::DYNAMIC);
      ls.root->addChild (ls.Def.model.get ());

      ls.root->setNodeMask (
         (ls.root->getNodeMask () & ~_masterMask) |
            (ls.Def.Glyph ? _glyphMask : _entityMask));

      osg::ref_ptr<osg::Group> scene = _render->get_dynamic_objects ();

      if (scene.valid ()) { scene->addChild (ls.root.get ()); }

      _update_link (ls);
   }
}


void
dmz::RenderPluginLinkOSG::_update_link (LinkStruct &ls) {

   if (ls.root.valid ()) {

      Vector dir = ls.super.pos - ls.sub.pos;
      const Vector Scale (1.0, 1.0, dir.magnitude ());
      dir.normalize_in_place ();
      Matrix rot (dir);

      ls.root->setMatrix (to_osg_matrix (rot, ls.sub.pos, Scale));
   }
}


void
dmz::RenderPluginLinkOSG::_create_geometry (LinkDefStruct &def) {

   def.model = new osg::Geode ();

   osg::Geometry* geom = new osg::Geometry;

   osg::Vec4Array* colors = new osg::Vec4Array;
   colors->push_back (def.Color);
   geom->setColorArray (colors);
   geom->setColorBinding (osg::Geometry::BIND_OVERALL);

   osg::StateSet *stateset = geom->getOrCreateStateSet ();

   stateset->setAttributeAndModes (new osg::CullFace (osg::CullFace::BACK));

#if 0
   osg::ref_ptr<osg::Material> material = new osg::Material;
   material->setEmission (osg::Material::FRONT_AND_BACK, def.Color);
   stateset->setAttributeAndModes (material.get (), osg::StateAttribute::ON);
#endif

   osg::Vec3Array *vertices = new osg::Vec3Array;
   osg::Vec3Array* normals = new osg::Vec3Array;

   const Vector Offset (0.0, 0.0, -1.0);
   const Vector Forward (0.0, 0.0, -1.0);

   Float64 Angle = TwoPi64 / (Float64 (def.Sides));

   int count (0);

   for (Int32 ix = 0; ix <= def.Sides; ix++) {

      Matrix xform;
      Vector vec (0.0, 1.0, 0.0);

      if ((ix > 0) && (ix < def.Sides)) {

         Matrix xform (Forward, Angle * (Float64)ix);
         xform.transform_vector (vec);
      }

      Vector radius (vec * def.Radius);

      vertices->push_back (to_osg_vector (radius + Offset)); count++;
      vertices->push_back (to_osg_vector (radius)); count++;
      normals->push_back (to_osg_vector (vec));
      normals->push_back (to_osg_vector (vec));
   }

   geom->setNormalArray (normals);
   geom->setNormalBinding (osg::Geometry::BIND_PER_VERTEX);
   geom->addPrimitiveSet (new osg::DrawArrays (GL_TRIANGLE_STRIP, 0, count));
   geom->setVertexArray (vertices);
   def.model->addDrawable (geom);
}


void
dmz::RenderPluginLinkOSG::_init (Config &local) {

   RuntimeContext *context = get_plugin_runtime_context ();

   Config list;

   if (local.lookup_all_config ("link", list)) {

      ConfigIterator it;
      Config link;

      while (list.get_next_config (it, link)) {

         const Handle Attr = config_to_named_handle (
            "attribute",
            link,
            context);

         const osg::Vec4 Color = config_to_osg_vec4_color (
            link,
            osg::Vec4 (1.0, 1.0, 1.0, 1.0));

         const Float64 Radius = config_to_float64 ("radius", link, 0.15);

         const Int32 Sides = config_to_int32 ("sides", link, 4);

         const Boolean Glyph = config_to_boolean ("glyph", link, True);

         LinkDefStruct *lds = new LinkDefStruct (Attr, Color, Radius, Sides, Glyph);

         if (lds && _defTable.store (Attr, lds)) {

            activate_object_attribute (Attr, ObjectLinkMask | ObjectUnlinkMask);
         }
         else if (lds) { delete lds; lds = 0; }
      }
   }

   _defaultAttrHandle = activate_default_object_attribute (
      ObjectDestroyMask | ObjectPositionMask);
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzRenderPluginLinkOSG (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::RenderPluginLinkOSG (Info, local);
}

};
