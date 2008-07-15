#ifndef DMZ_RENDER_MODULE_CORE_OSG_DOT_H
#define DMZ_RENDER_MODULE_CORE_OSG_DOT_H

#include <dmzRuntimePlugin.h>
#include <dmzRuntimeRTTI.h>
#include <dmzTypesBase.h>

#define DMZ_RENDER_MODULE_CORE_OSG_INTERFACE_NAME "RenderModuleCoreInterface"

namespace osg { class Group; class Transform; class Camera; }
namespace osgViewer { class Viewer; }

namespace dmz {

   const char RenderModuleCoreOSGInterfaceName[] = "RenderModuleCoreOSGInterface";
   const char DefaultPortalNameOSG[] = "DMZ_Default_Portal_OSG";

   class RenderCameraManipulatorOSG;

   class RenderModuleCoreOSG {

      public:
         static RenderModuleCoreOSG *cast (const Plugin *PluginPtr);

         virtual osg::Group *get_scene () = 0;
         virtual osg::Group *get_static_objects () = 0;
         virtual osg::Group *get_dynamic_objects () = 0;

         virtual osg::Group *create_dynamic_object (const Handle ObjectHandle) = 0;
         virtual osg::Group *lookup_dynamic_object (const Handle ObjectHandle) = 0;

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

      protected:
         RenderModuleCoreOSG (const PluginInfo &Info);
         ~RenderModuleCoreOSG ();

      private:
         const PluginInfo &__Info;
   };
}


inline dmz::RenderModuleCoreOSG *
dmz::RenderModuleCoreOSG::cast (const Plugin *PluginPtr) {

   return (RenderModuleCoreOSG *)lookup_rtti_interface (
      RenderModuleCoreOSGInterfaceName,
      "",
      PluginPtr);
}


inline
dmz::RenderModuleCoreOSG::RenderModuleCoreOSG (const PluginInfo &Info) :
      __Info (Info) {

   store_rtti_interface (RenderModuleCoreOSGInterfaceName, __Info, (void *)this);
}


inline
dmz::RenderModuleCoreOSG::~RenderModuleCoreOSG () {

   remove_rtti_interface (RenderModuleCoreOSGInterfaceName, __Info);
}

#endif //  DMZ_RENDER_MODULE_CORE_OSG_DOT_H
