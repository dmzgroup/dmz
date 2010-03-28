#include <dmzEventCallbackMasks.h>
#include <dmzEventConsts.h>
#include <dmzEventModule.h>
#include <dmzRenderModuleCoreOSG.h>
#include "dmzRenderPluginEventOSG.h"
#include <dmzRenderUtilOSG.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeEventType.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>

#include <osg/Billboard>
#include <osg/CullFace>
#include <osg/Material>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/BlendFunc>
#include <osg/Depth>
#include <osg/PolygonOffset>
#include <osg/Matrix>
#include <osg/RenderInfo>
#include <osg/Texture2D>
         
#include <osgDB/ReadFile>

dmz::RenderPluginEventOSG::RenderPluginEventOSG (const PluginInfo &Info, Config &local) :
      Plugin (Info),
      TimeSlice (Info),
      EventObserverUtil (Info, local),
      _log (Info),
      _defs (Info),
      _rc (Info),
      _isectMask (0),
      _core (0),
      _defaultHandle (0) {

   _init (local);
}


dmz::RenderPluginEventOSG::~RenderPluginEventOSG () {

}


// Plugin Interface
void
dmz::RenderPluginEventOSG::update_plugin_state (
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
dmz::RenderPluginEventOSG::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      if (!_core) {

         _core = RenderModuleCoreOSG::cast (PluginPtr);

         if (_core) { _isectMask = _core->get_isect_mask (); }
      }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_core &&  (_core == RenderModuleCoreOSG::cast (PluginPtr))) { _core = 0; }
   }
}


// TimeSlice Interface
void
dmz::RenderPluginEventOSG::update_time_slice (const Float64 TimeDelta) {

   HashTableHandleIterator it;
   EventStruct *event (0);
   osg::Group *group = _core ? _core->get_dynamic_objects () : 0;

   while (_eventTable.get_next (it, event)) {

      event->scalar += event->Type.Rate * TimeDelta;

//_log.warn << "Processing: " << it.get_hash_key () << " " << event->scalar << " " << event->Type.Scale << endl;

      if (event->scalar > event->Type.Scale) {

         if (group) { group->removeChild (event->root.get ()); }

         event = _eventTable.remove (it.get_hash_key ());

         if (event) { delete event; event = 0; }
      }
      else {

         const Float64 Value = event->scalar;
         osg::Matrix mat;
         mat.makeScale (osg::Vec3 (Value, Value, Value));
         event->scale->setMatrix (mat);
      }
   }
}


// Event Observer Interface
void
dmz::RenderPluginEventOSG::close_event (
      const Handle EventHandle,
      const EventType &Type,
      const EventLocalityEnum Locality) {

   const Handle TypeHandle = Type.get_handle ();

   if (!_ignore.contains (TypeHandle)) {

      TypeStruct *ts = _get_type (Type);

      if (ts) {

         EventStruct *es = new EventStruct (*ts);

         if (es) { _create_event (EventHandle, *es); }
      }
   }
}


// RenderPluginEventOSG Interface
dmz::RenderPluginEventOSG::TypeStruct *
dmz::RenderPluginEventOSG::_get_type (const EventType &Type) {

   TypeStruct *result (0);
   EventType current (Type);
   Boolean created (False);

   while (current && !result) {

      result = _typeMap.lookup (current.get_handle ());

      if (!result) { result = _create_type (current); }

      if (!result) { current.become_parent (); }
   }

   if (result && (current != Type)) {

      _typeMap.store (Type.get_handle (), result);
   }

   return result;
}


dmz::RenderPluginEventOSG::TypeStruct *
dmz::RenderPluginEventOSG::_create_type (const EventType &Type) {

   TypeStruct *result (0);

   Config info = Type.get_config ();

   const String ImageRc = config_to_string ("render.image.resource", info);
   const Float64 Offset = config_to_float64 ("render.offset.value", info, 10);
   const Float64 Scale = config_to_float64 ("render.scale.value", info, 1.1);
   const Float64 Rate = config_to_float64 ("render.scale.rate", info, 0.1);

   if (ImageRc) {

      const String ImageName = _rc.find_file (ImageRc);

      if (ImageName) {

         result = new TypeStruct (Offset, Scale, Rate);

         if (result) {

            result->image = osgDB::readImageFile (ImageName.get_buffer ());

            if (result->image.valid ()) {

               if (_typeTable.store (Type.get_handle (), result)) {

                  _typeMap.store (Type.get_handle (), result);
               }
               else { delete result; result = 0; }
            }
            else {

               _log.error << "Failed to load image resource: " << ImageRc
                  << " -> " << ImageName << endl;
               _ignore.add_handle (Type.get_handle ());
            }
         }
      }
      else {

         _log.error << "Failed to find image resource: " << ImageRc << endl;
         _ignore.add_handle (Type.get_handle ());
      }
   }

   return result;
}


void
dmz::RenderPluginEventOSG::_create_event (const Handle EventHandle, EventStruct &event) {

   event.root = new osg::MatrixTransform;
   event.scale = new osg::MatrixTransform;
   event.root->addChild (event.scale);

   UInt32 mask = event.root->getNodeMask ();
   mask &= ~(_isectMask);
   event.root->setNodeMask (mask);

   EventModule *module = get_event_module ();

   if (module) {

      Vector pos;
      module->lookup_position (EventHandle, _defaultHandle, pos);
      osg::Matrix mat;
      mat.makeTranslate (to_osg_vector (pos));
      event.root->setMatrix (mat);
   }

   osg::Billboard* geode = new osg::Billboard ();
   geode->setMode (osg::Billboard::POINT_ROT_EYE);
//   geode->setMode (osg::Billboard::POINT_ROT_WORLD);

   osg::Geometry* geom = new osg::Geometry;

   osg::Vec4Array* colors = new osg::Vec4Array;
   colors->push_back (osg::Vec4 (1.0f, 1.0f, 1.0f, 1.0f));
   geom->setColorArray (colors);
   geom->setColorBinding (osg::Geometry::BIND_OVERALL);

   osg::StateSet *stateset = geom->getOrCreateStateSet ();
   stateset->setMode (GL_BLEND, osg::StateAttribute::ON);

   osg::ref_ptr<osg::Material> material = new osg::Material;
   material->setEmission (osg::Material::FRONT_AND_BACK, osg::Vec4 (1.0, 1.0, 1.0, 1.0));
   stateset->setAttributeAndModes (material.get (), osg::StateAttribute::ON);

   osg::Texture2D *tex = new osg::Texture2D (event.Type.image.get ());
   tex->setWrap (osg::Texture2D::WRAP_S, osg::Texture2D::REPEAT);
   tex->setWrap (osg::Texture2D::WRAP_T, osg::Texture2D::REPEAT);

   stateset->setTextureAttributeAndModes (0, tex, osg::StateAttribute::ON);

   stateset->setAttributeAndModes (new osg::CullFace (osg::CullFace::BACK));

   const float Off (event.Type.Offset);

   osg::Vec3 v1 (-Off, 0.0, -Off);
   osg::Vec3 v2 ( Off, 0.0, -Off);
   osg::Vec3 v3 ( Off, 0.0,  Off);
   osg::Vec3 v4 (-Off, 0.0,  Off);

   osg::Vec3 n1 (0.0, -1.0, 0.0);

   const float F1 (0.0f);
   const float F2 (1.0f);

   osg::Vec3Array *vertices = new osg::Vec3Array;
   osg::Vec2Array *tcoords = new osg::Vec2Array;
   osg::Vec3Array* normals = new osg::Vec3Array;

   vertices->push_back (v1);
   vertices->push_back (v2);
   vertices->push_back (v3);
   vertices->push_back (v4);
   tcoords->push_back (osg::Vec2 (F1, F1));
   tcoords->push_back (osg::Vec2 (F1, F2));
   tcoords->push_back (osg::Vec2 (F2, F2));
   tcoords->push_back (osg::Vec2 (F2, F1));
   normals->push_back (n1);

   geom->setNormalArray (normals);
   geom->setNormalBinding (osg::Geometry::BIND_OVERALL);
   geom->addPrimitiveSet (new osg::DrawArrays (GL_QUADS, 0, 4));
   geom->setVertexArray (vertices);
   geom->setTexCoordArray (0, tcoords);
   geode->addDrawable (geom);

   event.scale->addChild (geode);

   if (_eventTable.store (EventHandle, &event)) {

      osg::Group *group = _core ? _core->get_dynamic_objects () : 0;

      if (group) { group->addChild (event.root.get ()); }
      else { _log.error << "Failed to add geode!" << endl; }
   }
   else { delete &event; }
}


void
dmz::RenderPluginEventOSG::_init (Config &local) {

   RuntimeContext *context = get_plugin_runtime_context ();

   EventTypeSet set = config_to_event_type_set ("events", local, context);

   if (set.get_count () == 0) {

      EventType root = _defs.get_root_event_type ();
      activate_event_callback (root, EventCloseMask);
   }
   else {

      EventTypeIterator it;
      EventType type;

      while (set.get_next (it, type)) {

         activate_event_callback (type, EventCloseMask);
      }
   }

   _defaultHandle = _defs.create_named_handle (EventAttributeDefaultName);
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzRenderPluginEventOSG (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::RenderPluginEventOSG (Info, local);
}

};
