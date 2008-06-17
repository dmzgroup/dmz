#include "dmzRenderModuleCoreOSGBasic.h"
#include <dmzObjectModule.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigRead.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzRuntimePluginLoader.h>
#include <osg/DeleteHandler>
#include <osg/Referenced>


dmz::RenderModuleCoreOSGBasic::RenderModuleCoreOSGBasic (
      const PluginInfo &Info,
      Config &local,
      Config &global) :
      Plugin (Info),
      ObjectObserverUtil (Info, local),
      RenderModuleCoreOSG (Info),
      _log (Info),
      _extensions (),
      _objectModule (0),
      _scene (),
      _staticObjects (),
      _dynamicObjects (),
      _portalTable (),
      _numStaticVertices (0),
      _staticVertices (0),
      _numStaticTriIndices (0),
      _staticTriIndices (0) {

   _scene = new osg::Group;

   _staticObjects = new osg::Group;
   _scene->addChild (_staticObjects.get ());

   _dynamicObjects = new osg::Group;
   _scene->addChild (_dynamicObjects.get ());

   _init (local, global);
}


dmz::RenderModuleCoreOSGBasic::~RenderModuleCoreOSGBasic () {

   _extensions.remove_plugins ();
   _portalTable.empty ();

   osg::DeleteHandler *dh (osg::Referenced::getDeleteHandler ());

   if (dh) { dh->flush (); }

   if (_staticVertices) { delete [] _staticVertices; _staticVertices = 0; }
   if (_staticTriIndices) { delete [] _staticTriIndices; _staticTriIndices = 0; }
}


// Plugin Interface
void
dmz::RenderModuleCoreOSGBasic::update_plugin_state (
      const PluginStateEnum State,
      const UInt32 Level) {

   if (State == PluginStateStart) {

      _extensions.start_plugins ();
   }
   else if (State == PluginStateStop) {

      _extensions.stop_plugins ();
   }
   else if (State == PluginStateShutdown) {

      _extensions.shutdown_plugins ();
   }
}


void
dmz::RenderModuleCoreOSGBasic::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      _extensions.discover_external_plugin (PluginPtr);

      if (!_objectModule) { _objectModule = ObjectModule::cast (PluginPtr); }
   }
   else if (Mode == PluginDiscoverRemove) {

      _extensions.remove_external_plugin (PluginPtr);

      if (_objectModule && (_objectModule == ObjectModule::cast (PluginPtr))) {

         _objectModule = 0;
      }
   }
}


// Object Observer Interface
void
dmz::RenderModuleCoreOSGBasic::update_object_position (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Value,
      const Vector *PreviousValue) {

}


void
dmz::RenderModuleCoreOSGBasic::update_object_orientation (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Matrix &Value,
      const Matrix *PreviousValue) {

}


// RenderModuloeCoreOSG Interface
osg::Group *
dmz::RenderModuleCoreOSGBasic::get_scene () {

   return _scene.get ();
}


osg::Group *
dmz::RenderModuleCoreOSGBasic::get_static_objects () {

   return _staticObjects.get ();
}


osg::Group *
dmz::RenderModuleCoreOSGBasic::get_dynamic_objects () {

   return _dynamicObjects.get ();
}


dmz::Boolean
dmz::RenderModuleCoreOSGBasic::add_dynamic_object (osg::Transform *node) {

   Boolean result(False);

   result = _dynamicObjects->addChild (node);

   return result;
}


dmz::Boolean
dmz::RenderModuleCoreOSGBasic::remove_dynamic_object (osg::Transform *node) {

   Boolean result(False);

   result = _dynamicObjects->removeChild (node);

   return result;
}


dmz::Boolean
dmz::RenderModuleCoreOSGBasic::add_camera (
      const String &PortalName,
      osg::Camera *camera) {

   Boolean result (False);

   PortalStruct *ps = _get_portal_struct (PortalName);
   if (ps && camera) {

      if (!(ps->camera.valid ())) {

         ps->camera = camera;
         result = True;
      }
   }

   return result;
}


osg::Camera *
dmz::RenderModuleCoreOSGBasic::lookup_camera (const String &PortalName) {

   osg::Camera *result = 0;

   PortalStruct *ps = _portalTable.lookup (PortalName);
   if (ps) {

      result = ps->camera.get ();
   }

   return result;
}


osg::Camera *
dmz::RenderModuleCoreOSGBasic::remove_camera (const String &PortalName) {

   osg::Camera *result = 0;

   PortalStruct *ps = _portalTable.lookup (PortalName);
   if (ps) {

      result = ps->camera.get ();
      ps->camera.release ();
   }

   return result;
}


dmz::Boolean
dmz::RenderModuleCoreOSGBasic::add_camera_manipulator (
      const String &PortalName,
      dmz::RenderCameraManipulatorOSG *manipulator) {

   Boolean result(False);

   PortalStruct *ps = _get_portal_struct (PortalName);
   if (ps && manipulator) {

      if (!(ps->cameraManipulator.valid ())) {

         ps->cameraManipulator = manipulator;
         result = True;
      }
   }

   return result;
}


dmz::RenderCameraManipulatorOSG *
dmz::RenderModuleCoreOSGBasic::lookup_camera_manipulator (const String &PortalName) {

   RenderCameraManipulatorOSG *result = 0;

   PortalStruct *ps = _portalTable.lookup (PortalName);
   if (ps) {

      result = ps->cameraManipulator.get ();
   }

   return result;
}


dmz::RenderCameraManipulatorOSG *
dmz::RenderModuleCoreOSGBasic::remove_camera_manipulator (const String &PortalName) {

   RenderCameraManipulatorOSG *result = 0;

   PortalStruct *ps = _portalTable.lookup (PortalName);
   if (ps) {

      result = ps->cameraManipulator.get ();
      ps->cameraManipulator.release ();
   }

   return result;
}

void
dmz::RenderModuleCoreOSGBasic::get_static_triangles (
      Vector **vertices,
      UInt32 &numVerts,
      UInt32 **indices,
      UInt32 &numIndices) {

      if (!_staticVertices) {

         _countStaticVerticesAndIndices (_staticObjects.get());
         _extractStaticTriangles (_staticObjects.get());

      }


      (*vertices) = _staticVertices;
      numVerts = _numStaticVertices;

      (*indices) = _staticTriIndices;
      numIndices = _numStaticTriIndices;
}


void
dmz::RenderModuleCoreOSGBasic::_countStaticVerticesAndIndices (osg::Node *nd) {
   _numStaticVertices = 0;
   _numStaticTriIndices = 0;

   _countStaticVertsIndiciesRecursive (nd);

   _log.info << "Number Verts: " << _numStaticVertices
      << ", Number Triangles: " << (_numStaticTriIndices/3.0) << endl;
}


void
dmz::RenderModuleCoreOSGBasic::_countStaticVertsIndiciesRecursive (osg::Node *nd) {

   /// here you have found a group.
   osg::Geode *geode = dynamic_cast<osg::Geode *> (nd);
   if (geode) { // analyse the geode. If it isnt a geode the dynamic cast gives NULL.

      for (UInt32 i = 0; i < geode->getNumDrawables (); i++) {

         osg::Drawable *drawable=geode->getDrawable(i);
         osg::Geometry *geom=dynamic_cast<osg::Geometry *> (drawable);

         for (UInt32 ipr = 0; ipr < geom->getNumPrimitiveSets (); ipr++) {

            _numStaticVertices += geom->getVertexArray ()->getNumElements ();

            osg::PrimitiveSet* prset=geom->getPrimitiveSet (ipr);

            if (prset->getMode () == osg::PrimitiveSet::TRIANGLES) {

               _numStaticTriIndices += prset->getNumIndices ();
            }
           // osg::notify(osg::WARN) << "Primitive Set "<< ipr << std::endl;
         }
      }
   }
   else {
      osg::Group *gp = dynamic_cast<osg::Group *> (nd);
      if (gp) {

        // osg::notify(osg::WARN) << "Group "<<  gp->getName() <<std::endl;
         for (UInt32 ic = 0; ic < gp->getNumChildren (); ic++) {

            _countStaticVertsIndiciesRecursive (gp->getChild (ic));
         }
      }
   }
}


void
dmz::RenderModuleCoreOSGBasic::_extractStaticTriangles (osg::Node *nd) {

   // Clear any existing geometry
   if (_staticVertices) {

      delete [] _staticVertices; _staticVertices = 0;
   }

   _staticVertices = new Vector [_numStaticVertices];

   if (_staticTriIndices) {

      delete [] _staticTriIndices; _staticTriIndices = 0;
   }

   _staticTriIndices = new UInt32 [_numStaticTriIndices];

   UInt32 finalVertexIndex (0);
   UInt32 finalIndicesIndex (0);

   // Begin recursive extraction of triangles
   _extractStaticTrianglesRecursive (nd, 0, finalVertexIndex, 0, finalIndicesIndex);
}


void
dmz::RenderModuleCoreOSGBasic::_extractStaticTrianglesRecursive (
      osg::Node *nd,
      const UInt32 StartVertexIndex,
      UInt32 &endVertexIndex,
      const UInt32 StartIndicesIndex,
      UInt32 &endIndiciesIndex) {


   UInt32 outputVertIndex = StartVertexIndex;
   UInt32 outputIndiciesIndex = StartIndicesIndex;

   osg::Geode *geode = dynamic_cast<osg::Geode *> (nd);
   if (geode) { // analyse the geode. If it isnt a geode the dynamic cast gives NULL.

      for (UInt32 i=0; i<geode->getNumDrawables (); i++) {

         osg::Drawable *drawable=geode->getDrawable(i);
         osg::Geometry *geom=dynamic_cast<osg::Geometry *> (drawable);

         for (UInt32 ipr = 0; ipr < geom->getNumPrimitiveSets (); ipr++) {


            // Copy vertices

            osg::Vec3Array *verts = dynamic_cast<osg::Vec3Array*>(geom->getVertexArray());

            UInt32 vertCount (0);
            for (; vertCount < verts->size (); vertCount ++) {

               _staticVertices [outputVertIndex + vertCount].set_xyz (
                  (*verts)[vertCount]. x(),
                  (*verts)[vertCount]. y(),
                  (*verts)[vertCount]. z());

            }
            // Update output index for vertices
            outputVertIndex += vertCount;

            // Copy triangle indices
            osg::PrimitiveSet* prset=geom->getPrimitiveSet (ipr);
            if (prset->getMode () == osg::PrimitiveSet::TRIANGLES) {

               UInt32 indiciesCount (0);
               for (; indiciesCount < prset->getNumIndices () - 2; indiciesCount += 3) {

                  _staticTriIndices [outputIndiciesIndex + indiciesCount] =
                     prset->index (indiciesCount);

                  _staticTriIndices [outputIndiciesIndex + indiciesCount + 1] =
                     prset->index (indiciesCount + 1);

                  _staticTriIndices [outputIndiciesIndex + indiciesCount + 2] =
                     prset->index (indiciesCount + 2);
               }
               // Update output index for indices
               outputIndiciesIndex += indiciesCount;
            }
           // osg::notify(osg::WARN) << "Primitive Set "<< ipr << std::endl;
         }
      }
   }
   else {
      osg::Group *gp = dynamic_cast<osg::Group *> (nd);
      if (gp) {

        // osg::notify(osg::WARN) << "Group "<<  gp->getName() <<std::endl;
         for (UInt32 ic=0; ic < gp->getNumChildren (); ic++) {

            _extractStaticTrianglesRecursive (
               gp->getChild (ic),
               outputVertIndex, // Input index
               outputVertIndex, // Result index
               outputIndiciesIndex, // Input index
               outputIndiciesIndex); // Result index
         }
      }
   }

   // Output values will be the index AFTER the last written value location in both
   // arrays
   endVertexIndex = outputVertIndex;
   endIndiciesIndex = outputIndiciesIndex;
}


void
dmz::RenderModuleCoreOSGBasic::_analyzePrimitiveSet (
      osg::PrimitiveSet *prset,
      const osg::Vec3Array *verts) {

   unsigned int ic;
   //unsigned int i2;
   unsigned int nprim=0;
  // osg::notify(osg::WARN) << "Prim set type "<< prset->getMode() << std::endl;

   for (ic=0; ic<prset->getNumIndices (); ic++) {
      // NB the vertices are held in the drawable -

      //osg::notify(osg::WARN) << "vertex "<< ic <<
       //  " is index "<<prset->index(ic) << " at " <<
        // (* verts)[prset->index(ic)].x() << "," <<
        // (* verts)[prset->index(ic)].y() << "," <<
         //(* verts)[prset->index(ic)].z() << std::endl;
   }
   // you might want to handle each type of primset differently: such as:
   switch (prset->getMode ()) {


      case osg::PrimitiveSet::TRIANGLES:
         // get vertices of triangle
         //osg::notify(osg::WARN) << "Triangles "<< nprim << " is index "<<prset->index(ic) << std::endl;
         //for (unsigned int i2=0; i2<prset->getNumIndices()-2; i2+=3) {
         //}
         _log.out << "Triangles found" << endl;
         break;
      case osg::PrimitiveSet::TRIANGLE_STRIP: // look up how tristrips are coded
         _log.out << "Triangle strips found" << endl;
         break;
      case osg::PrimitiveSet::TRIANGLE_FAN:
         _log.out << "Triangle fans found" << endl;
      default:
         _log.out << "Other primitives found: " << String(prset->getName ().c_str ()) << endl;

         break;
   // etc for all the primitive types you expect. EG quads, quadstrips lines line loops....
   }

}


dmz::RenderModuleCoreOSGBasic::PortalStruct *
dmz::RenderModuleCoreOSGBasic::_get_portal_struct (const String &PortalName) {

   PortalStruct *ps = _portalTable.lookup (PortalName);
   if (!ps) {

      ps = new PortalStruct (PortalName);
      if (!_portalTable.store (PortalName, ps)) { delete ps; ps = 0; }
   }

   return ps;
}


void
dmz::RenderModuleCoreOSGBasic::_init (Config &local, Config &global) {

   Config pluginList;

   if (local.lookup_all_config ("plugins.plugin", pluginList)) {

      PluginLoader loader (get_plugin_runtime_context ());

      if (loader.load_plugins (pluginList, local, global, _extensions, &_log)) {

         _extensions.discover_plugins ();
      }
   }
   else {

   }
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzRenderModuleCoreOSGBasic (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::RenderModuleCoreOSGBasic (Info, local, global);
}

};
