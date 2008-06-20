#ifndef DMZ_RENDER_MODULE_CORE_OGRE_DOT_H
#define DMZ_RENDER_MODULE_CORE_OGRE_DOT_H

#include <dmzRuntimePlugin.h>
#include <dmzRuntimeRTTI.h>
#include <dmzTypesBase.h>

#define DMZ_RENDER_MODULE_CORE_OGRE_INTERFACE_NAME "RenderModuleCoreInterface"


namespace Ogre {

   class Root;
   class SceneManager;
   class SceneNode;
   class Camera;
};


namespace dmz {

   const char DefaultPortalNameOgre[] = "default_portal";

   class RenderModuleCoreOgre {

      public:
         static RenderModuleCoreOgre *cast (
            const Plugin *PluginPtr,
            const String &PluginName = "");

         virtual Ogre::SceneManager *get_scene_manager () = 0;

         virtual Boolean add_camera (const String &PortalName, Ogre::Camera *camera) = 0;
         virtual Ogre::Camera *lookup_camera (const String &PortalName) = 0;
         virtual Ogre::Camera *remove_camera (const String &PortalName) = 0;

         virtual Boolean add_dynamic_object (
            const Handle ObjectHandle,
            Ogre::SceneNode *node) = 0;

         virtual Ogre::SceneNode *lookup_dynamic_object (const Handle ObjectHandle) = 0;
         virtual Ogre::SceneNode *remove_dynamic_object (const Handle ObjectHandle) = 0;

      protected:
         RenderModuleCoreOgre (const PluginInfo &Info);
         ~RenderModuleCoreOgre ();

      private:
         RenderModuleCoreOgre ();
         RenderModuleCoreOgre (const RenderModuleCoreOgre &);
         RenderModuleCoreOgre &operator= (const RenderModuleCoreOgre &);

         const PluginInfo &__RenderModuleCoreOgrePluginInfo;
   };
}


inline dmz::RenderModuleCoreOgre *
dmz::RenderModuleCoreOgre::cast (const Plugin *PluginPtr, const String &PluginName) {

   return (RenderModuleCoreOgre *)lookup_rtti_interface (
      DMZ_RENDER_MODULE_CORE_OGRE_INTERFACE_NAME,
      PluginName,
      PluginPtr);
}


inline
dmz::RenderModuleCoreOgre::RenderModuleCoreOgre (const PluginInfo &Info) :
      __RenderModuleCoreOgrePluginInfo (Info) {

   store_rtti_interface (
      DMZ_RENDER_MODULE_CORE_OGRE_INTERFACE_NAME,
      __RenderModuleCoreOgrePluginInfo.get_handle (),
      __RenderModuleCoreOgrePluginInfo.get_context (),
      (void *)this);
}


inline
dmz::RenderModuleCoreOgre::~RenderModuleCoreOgre () {

   remove_rtti_interface (
      DMZ_RENDER_MODULE_CORE_OGRE_INTERFACE_NAME,
      __RenderModuleCoreOgrePluginInfo.get_handle (),
      __RenderModuleCoreOgrePluginInfo.get_context ());
}

#endif //  DMZ_RENDER_MODULE_CORE_OGRE_DOT_H
