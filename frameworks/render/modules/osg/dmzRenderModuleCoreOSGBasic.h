#ifndef DMZ_RENDER_MODULE_CORE_OSG_BASIC_DOT_H
#define DMZ_RENDER_MODULE_CORE_OSG_BASIC_DOT_H

#include <dmzObjectObserverUtil.h>
#include <dmzRenderModuleCoreOSG.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimePluginContainer.h>
#include <dmzRuntimeTimeSlice.h>
#include <dmzSystemFile.h>
#include <dmzTypesBase.h>
#include <dmzTypesHashTableStringTemplate.h>
#include <dmzTypesHashTableHandleTemplate.h>
#include <dmzTypesMatrix.h>
#include <dmzTypesVector.h>

#include <osg/Camera>
#include <osg/Drawable>
#include <osg/Group>
#include <osg/Geometry>
#include <osg/MatrixTransform>
#include <osg/Node>
#include <osg/Transform>
#include <osgViewer/View>


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
         virtual UInt32 get_cull_mask ();
         virtual UInt32 get_isect_mask ();
         virtual UInt32 get_overlay_mask ();

         virtual osg::Group *get_scene ();
         virtual osg::Group *get_overlay ();
         virtual osg::Group *get_isect ();
         virtual osg::Group *get_static_objects ();
         virtual osg::Group *get_dynamic_objects ();

         virtual osg::Group *create_dynamic_object (const Handle ObjectHandle);
         virtual osg::Group *lookup_dynamic_object (const Handle ObjectHandle);

         virtual Boolean add_view (const String &ViewName, osgViewer::View *view);
         virtual osgViewer::View *lookup_view (const String &ViewName);
         virtual osgViewer::View *remove_view (const String &ViewName);

      protected:
         struct ViewStruct {

            const String Name;
            osg::ref_ptr<osgViewer::View> view;

            ViewStruct (const String &TheName, osgViewer::View *theView) :
                  Name (TheName),
                  view (theView) {;} 

            ~ViewStruct () { view = 0; }
         };

         struct ObjectStruct {

            const Handle Object;
            ObjectStruct *next;
            osg::ref_ptr<osg::MatrixTransform> transform;
            Matrix ori;
            Vector pos;
            Boolean dirty;
            Boolean destroyed;

            ObjectStruct (const Handle TheObject) :
                  Object (TheObject),
                  next (0),
                  dirty (False),
                  destroyed (False) {

               transform =  new osg::MatrixTransform;
            }

            ~ObjectStruct () { transform = 0; }
         };

         void _init (Config &local, Config &global);

         Log _log;
         PluginContainer _extensions;
         UInt32 _cullMask;
         UInt32 _isectMask;
         UInt32 _overlayMask;
         Handle _defaultHandle;
         Handle _bvrHandle;
         osg::ref_ptr<osg::Group> _scene;
         osg::ref_ptr<osg::Group> _overlay;
         osg::ref_ptr<osg::Group> _isect;
         osg::ref_ptr<osg::Group> _staticObjects;
         osg::ref_ptr<osg::Group> _dynamicObjects;
         HashTableStringTemplate<ViewStruct> _viewTable;
         HashTableHandleTemplate<ObjectStruct> _objectTable;
         ObjectStruct *_dirtyObjects;
   };
}

#endif //  DMZ_RENDER_MODULE_CORE_OSG_BASIC_DOT_H
