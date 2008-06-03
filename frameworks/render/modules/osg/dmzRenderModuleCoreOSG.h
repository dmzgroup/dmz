#ifndef DMZ_RENDER_MODULE_CORE_OSG_DOT_H
#define DMZ_RENDER_MODULE_CORE_OSG_DOT_H

#include <dmzRuntimePlugin.h>
#include <dmzRuntimeRTTI.h>
#include <dmzTypesBase.h>

#define DMZ_RENDER_MODULE_CORE_OSG_INTERFACE_NAME "RenderModuleCoreInterface"

namespace osg { class Group; class Transform; class Camera; }
namespace osgViewer { class Viewer; }

namespace dmz {

   const char DefaultPortalNameOSG[] = "default";

   class RenderCameraManipulatorOSG;
   

   class RenderModuleCoreOSG {

      public:
         static RenderModuleCoreOSG *cast (const Plugin *PluginPtr);

         virtual osg::Group *get_scene () = 0;
         virtual osg::Group *get_static_objects () = 0;
         virtual osg::Group *get_dynamic_objects () = 0;
         

//         virtual void update_dynamic_object_position (const ObjectHandle Handle, const Vector &Pos) = 0;

         virtual Boolean add_dynamic_object (osg::Transform *Handle) = 0;
         virtual Boolean remove_dynamic_object (osg::Transform *Handle) = 0;

         virtual Boolean add_camera (const String &PortalName, osg::Camera *camera) = 0;
         virtual osg::Camera *lookup_camera (const String &PortalName) = 0;
         virtual osg::Camera *remove_camera (const String &PortalName) = 0;

         virtual Boolean add_camera_manipulator (
            const String &PortalName,
            RenderCameraManipulatorOSG *manipulator) = 0;
            
         virtual RenderCameraManipulatorOSG *lookup_camera_manipulator (
            const String &PortalName) = 0;
            
         virtual RenderCameraManipulatorOSG *remove_camera_manipulator (
            const String &PortalName) = 0;

         virtual void get_static_triangles (
            Vector **vertices, 
            UInt32 &numVerts, 
            UInt32 **indices, 
            UInt32 &numIndices) = 0;
         
      protected:
         RenderModuleCoreOSG (const PluginInfo &Info);
         ~RenderModuleCoreOSG ();

      private:
         const PluginInfo &__RenderModuleCoreOSGPluginInfo;
   };
}


inline dmz::RenderModuleCoreOSG *
dmz::RenderModuleCoreOSG::cast (const Plugin *PluginPtr) {

   return (RenderModuleCoreOSG *)lookup_rtti_interface (
      DMZ_RENDER_MODULE_CORE_OSG_INTERFACE_NAME,
      PluginPtr ? PluginPtr->get_plugin_handle () : 0,
      PluginPtr ? PluginPtr->get_plugin_runtime_context () : 0);
}


inline
dmz::RenderModuleCoreOSG::RenderModuleCoreOSG (const PluginInfo &Info) :
      __RenderModuleCoreOSGPluginInfo (Info) {

   store_rtti_interface (
      DMZ_RENDER_MODULE_CORE_OSG_INTERFACE_NAME,
      __RenderModuleCoreOSGPluginInfo.get_handle (),
      __RenderModuleCoreOSGPluginInfo.get_context (),
      (void *)this);
}


inline
dmz::RenderModuleCoreOSG::~RenderModuleCoreOSG () {

   remove_rtti_interface (
      DMZ_RENDER_MODULE_CORE_OSG_INTERFACE_NAME,
      __RenderModuleCoreOSGPluginInfo.get_handle (),
      __RenderModuleCoreOSGPluginInfo.get_context ());
}

#endif //  DMZ_RENDER_MODULE_CORE_OSG_DOT_H
