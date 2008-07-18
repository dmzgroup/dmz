#ifndef DMZ_RENDER_MODULE_CORE_OSG_BASIC_DOT_H
#define DMZ_RENDER_MODULE_CORE_OSG_BASIC_DOT_H

#include <dmzObjectObserverUtil.h>
#include <dmzRenderModuleCoreOSG.h>
#include <dmzRenderCameraManipulatorOSG.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimePluginContainer.h>
#include <dmzRuntimeTimeSlice.h>
#include <dmzSystemFile.h>
#include <dmzTypesBase.h>
#include <dmzTypesHashTableStringTemplate.h>
#include <dmzTypesHashTableHandleTemplate.h>

#include <osg/Camera>
#include <osg/Drawable>
#include <osg/Group>
#include <osg/Geometry>
#include <osg/MatrixTransform>
#include <osg/Node>
#include <osg/Transform>
#include <osgViewer/Viewer>


namespace dmz {

   class ObjectModule;

   class RenderModuleCoreOSGBasic :
         public Plugin,
         public TimeSlice,
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

         // Time Slice Interface
         virtual void update_time_slice (const Float64 DeltaTime);

         // Object Observer Interface
         virtual void destroy_object (const UUID &Identity, const Handle ObjectHandle);

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
         virtual String find_file (const String &FileName);
         virtual osg::Group *get_scene ();
         virtual osg::Group *get_static_objects ();
         virtual osg::Group *get_dynamic_objects ();

         virtual osg::Group *create_dynamic_object (const Handle ObjectHandle);
         virtual osg::Group *lookup_dynamic_object (const Handle ObjectHandle);

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

         struct ObjectStruct {

            ObjectStruct *next;
            osg::ref_ptr<osg::MatrixTransform> transform;
            Matrix ori;
            Vector pos;
            Boolean dirty;
            Boolean destroyed;

            ObjectStruct () : next (0), dirty (False), destroyed (False) {

               transform =  new osg::MatrixTransform;
            }

            ~ObjectStruct () { transform.release (); }
         };

         void _init (Config &local, Config &global);
         PortalStruct *_get_portal_struct (const String &Name);

         Log _log;
         Handle _defaultHandle;
         PluginContainer _extensions;
         osg::ref_ptr<osg::Group> _scene;
         osg::ref_ptr<osg::Group> _staticObjects;
         osg::ref_ptr<osg::Group> _dynamicObjects;
         HashTableStringTemplate<PortalStruct> _portalTable;
         HashTableHandleTemplate<ObjectStruct> _objectTable;
         PathContainer _searchPath;
         ObjectStruct *_dirtyObjects;
   };
}

#endif //  DMZ_RENDER_MODULE_CORE_OSG_BASIC_DOT_H
