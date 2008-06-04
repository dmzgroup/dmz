#ifndef DMZ_RENDER_MODULE_CORE_OSG_BASIC_DOT_H
#define DMZ_RENDER_MODULE_CORE_OSG_BASIC_DOT_H

#include <dmzObjectObserverUtil.h>
#include <dmzRenderModuleCoreOSG.h>
#include <dmzRenderCameraManipulatorOSG.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimePluginContainer.h>
#include <dmzTypesBase.h>
#include <dmzTypesHashTableStringTemplate.h>

#include <osg/Camera>
#include <osg/Drawable>
#include <osg/Group>
#include <osg/Geometry>
#include <osg/Node>
#include <osg/Transform>
#include <osgViewer/Viewer>


namespace dmz {

   class ObjectModule;

   class RenderModuleCoreOSGBasic : 
         public Plugin,
         public ObjectObserverUtil,
         private RenderModuleCoreOSG {

      public:
         RenderModuleCoreOSGBasic (
            const PluginInfo &Info,
            Config &local,
            Config &global);
            
         ~RenderModuleCoreOSGBasic ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level);

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);
         
         // Object Observer Interface
         virtual void update_object_position (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Vector &Value,
            const Vector *PreviousValue);

         virtual void update_object_orientation (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Matrix &Value,
            const Matrix *PreviousValue);
            
         // RenderModuloeCoreOSG Interface
         virtual osg::Group *get_scene ();
         virtual osg::Group *get_static_objects ();
         virtual osg::Group *get_dynamic_objects ();

         virtual Boolean add_dynamic_object (osg::Transform *Handle);
         virtual Boolean remove_dynamic_object (osg::Transform *Handle);

         virtual Boolean add_camera (const String &PortalName, osg::Camera *camera);
         virtual osg::Camera *lookup_camera (const String &PortalName);
         virtual osg::Camera *remove_camera (const String &PortalName);

         virtual Boolean add_camera_manipulator (
            const String &PortalName,
            RenderCameraManipulatorOSG *manipulator);
            
         virtual RenderCameraManipulatorOSG *lookup_camera_manipulator (
            const String &PortalName);
            
         virtual RenderCameraManipulatorOSG *remove_camera_manipulator (
            const String &PortalName);

         virtual void get_static_triangles (
            Vector **vertices, 
            UInt32 &numVerts, 
            UInt32 **indices, 
            UInt32 &numIndices);

      protected:
         struct PortalStruct {

            const String Name;
            osg::ref_ptr<osg::Camera> camera;
            osg::ref_ptr<RenderCameraManipulatorOSG> cameraManipulator;

            PortalStruct (const String &TheName) :
               Name (TheName), camera (0), cameraManipulator (0) {;}

            ~PortalStruct () { 
               
               camera.release ();
               cameraManipulator.release ();
            }
         };
         
         void _init (Config &local, Config &global);
         PortalStruct *_get_portal_struct (const String &Name);

         void _countStaticVerticesAndIndices (osg::Node *nd);
         void _countStaticVertsIndiciesRecursive (osg::Node *nd);

         void _extractStaticTriangles (osg::Node *nd);
         void _extractStaticTrianglesRecursive (
            osg::Node *nd,
            const UInt32 StartVertexIndex, 
            UInt32 &endVertexIndex,
            const UInt32 StartIndicesIndex,
            UInt32 &endIndiciesIndex);

         void _analyzePrimitiveSet (
            osg::PrimitiveSet *prset, 
            const osg::Vec3Array *verts);

         UInt32 _numStaticVertices;
         Vector *_staticVertices;
         UInt32 _numStaticTriIndices;
         UInt32 *_staticTriIndices;

         

         Log _log;
         PluginContainer _extensions;
         ObjectModule *_objectModule;
         osg::ref_ptr<osg::Group> _scene;
         osg::ref_ptr<osg::Group> _staticObjects;
         osg::ref_ptr<osg::Group> _dynamicObjects;
         HashTableStringTemplate<PortalStruct> _portalTable;
   };
}

#endif //  DMZ_RENDER_MODULE_CORE_OSG_BASIC_DOT_H
