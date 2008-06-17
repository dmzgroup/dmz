#ifndef DMZ_RENDER_MODULE_CORE_OGRE_BASIC_DOT_H
#define DMZ_RENDER_MODULE_CORE_OGRE_BASIC_DOT_H

#include <dmzObjectObserverUtil.h>
#include <dmzRenderModuleCoreOgre.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimePluginContainer.h>
#include <dmzRuntimeTimeSlice.h>
#include <dmzTypesBase.h>
#include <dmzTypesHashTableStringTemplate.h>
#include <dmzTypesHashTableHandleTemplate.h>
#include "osmscene/IOSMSceneCallbacks.h"
#include <Ogre/OgreLog.h>

class OSMScene;
namespace Ogre { class Root; class SceneManager; class SceneNode; }



namespace dmz {
   
   class OgreLogger : public Ogre::LogListener {
      
      protected:
         Log _log;
   
      public:
         OgreLogger (RuntimeContext *context) : _log ("ogre", context) {;}
         ~OgreLogger () {;}
      
         virtual void messageLogged (
               const Ogre::String &Message,
               Ogre::LogMessageLevel lml,
               bool maskDebug,
               const Ogre::String &LogName) {
               
            if (Ogre::LML_TRIVIAL == lml) { _log.debug << Message.c_str () << endl; }
            else if (Ogre::LML_NORMAL == lml) { _log.info << Message.c_str () << endl; }
            else if (Ogre::LML_CRITICAL == lml) {

               _log.error << Message.c_str () << endl;
            }
         }
   };

   class ObjectModule;

   
   class RenderModuleCoreOgreBasic : 
         public Plugin,
         public TimeSlice,
         public ObjectObserverUtil,
         private OSMSceneCallbacks,
         private RenderModuleCoreOgre {

      public:
         RenderModuleCoreOgreBasic (
            const PluginInfo &Info,
            Config &local,
            Config &global);
            
         ~RenderModuleCoreOgreBasic ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level);

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);
         
         // TimeSlice Interface
         virtual void update_time_slice (const Float64 TimeDelta);

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
            
         // RenderModuloeCoreOgre Interface
         virtual Ogre::SceneManager *get_scene_manager ();

         virtual Boolean add_camera (const String &PortalName, Ogre::Camera *camera);
         virtual Ogre::Camera *lookup_camera (const String &PortalName);
         virtual Ogre::Camera *remove_camera (const String &PortalName);
         
         virtual Boolean add_dynamic_object (
            const Handle ObjectHandle,
            Ogre::SceneNode *node);
         
         virtual Ogre::SceneNode *lookup_dynamic_object (const Handle ObjectHandle);
         virtual Ogre::SceneNode *remove_dynamic_object (const Handle ObjectHandle);
         
      protected:
         // OSMSceneCallback Interface
         virtual void OnSceneManagerCreate (
            Ogre::SceneManager *pManager,
            TiXmlElement* pNodeDesc);

         struct PortalStruct {

            const String Name;
            Ogre::Camera *camera;

            PortalStruct (const String &TheName);
            ~PortalStruct ();
         };
         
         // OgreOSMSceneCallback Interface
         // void OnCreatedSceneManager (
         //    const OgreMax::OgreMaxScene *scene,
         //    Ogre::SceneManager *sceneManager);
            
         void _init (Config &local, Config &global);
         void _init_root (Config &local);
         void _init_resources (Config &local);
         void _init_render_system (Config &local);
         void _init_render_windows ();
         void _init_scene_manager (Config &local);

         PortalStruct *_get_portal_struct (const String &Name);
         
         Log _log;

         PluginContainer _extensions;

         ObjectModule *_objectModule;

         Handle _defaultAttributeHandle;

         OgreLogger _ogreLogger;
         
         Ogre::Root *_root;
         Ogre::SceneManager *_sceneManager;
         OSMScene *_osmSceneLoader;

         HashTableStringTemplate<PortalStruct> _portalTable;
         HashTableHandleTemplate<Ogre::SceneNode> _dynamicObjectTable;
         
      private:
         RenderModuleCoreOgreBasic ();
         RenderModuleCoreOgreBasic (const RenderModuleCoreOgreBasic &);
         RenderModuleCoreOgreBasic &operator= (const RenderModuleCoreOgreBasic &);
   };
}


inline
dmz::RenderModuleCoreOgreBasic::PortalStruct::PortalStruct (const String &TheName) :
      Name (TheName), camera (0) {;}


inline
dmz::RenderModuleCoreOgreBasic::PortalStruct::~PortalStruct () { camera = 0; }


#endif //  DMZ_RENDER_MODULE_CORE_OGRE_BASIC_DOT_H
