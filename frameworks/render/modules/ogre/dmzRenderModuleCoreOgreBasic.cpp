#include <dmzObjectModule.h>
#include <dmzObjectAttributeMasks.h>
#include <dmzRenderExtWindowOgre.h>
#include "dmzRenderModuleCoreOgreBasic.h"
#include <dmzRenderUtilOgre.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzRuntimeLoadPlugins.h>
#include <Ogre/Ogre.h>
#include "osmscene/OgreOSMScene.h"


dmz::RenderModuleCoreOgreBasic::RenderModuleCoreOgreBasic (
      const PluginInfo &Info,
      Config &local,
      Config &global) :
      Plugin (Info),
      TimeSlice (Info),
      ObjectObserverUtil (Info, local),
      OSMSceneCallbacks (),
      RenderModuleCoreOgre (Info),
      _log (Info),
      _extensions (),
      _objectModule (0),
      _defaultAttributeHandle (0),
      _ogreLogger (get_plugin_runtime_context ()),
      _root (0),
      _sceneManager (0),
      _osmSceneLoader (0),
      _portalTable (),
      _dynamicObjectTable () {

   _init (local, global);

   activate_default_object_attribute (ObjectPositionMask | ObjectOrientationMask);
}


dmz::RenderModuleCoreOgreBasic::~RenderModuleCoreOgreBasic () {

   _dynamicObjectTable.clear ();

   _extensions.remove_plugins ();
   _portalTable.empty ();

   if (_osmSceneLoader) {

      delete _osmSceneLoader;
      _osmSceneLoader = 0;
   }

   if (_root) {

      if (_sceneManager) {

         _root->destroySceneManager (_sceneManager);
         _sceneManager = 0;
      }

      delete _root;
      _root = 0;
   }
}


// Plugin Interface
void
dmz::RenderModuleCoreOgreBasic::update_plugin_state (
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
dmz::RenderModuleCoreOgreBasic::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      _extensions.discover_external_plugin (PluginPtr);

      if (!_objectModule) {

         _objectModule = ObjectModule::cast (PluginPtr);

         if (_objectModule) {

            PluginIterator it;

            Plugin *ptr (_extensions.get_first (it));

            while (ptr) {

               ObjectObserver *obs (ObjectObserver::cast (ptr));

               if (obs) {

                  obs->store_object_module (
                     PluginPtr->get_plugin_name (),
                     *_objectModule);
               }

               ptr = _extensions.get_next (it);
            }
         }
      }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_objectModule && (_objectModule == ObjectModule::cast (PluginPtr))) {

         PluginIterator it;

         Plugin *ptr (_extensions.get_first (it));

         while (ptr) {

            ObjectObserver *obs (ObjectObserver::cast (ptr));

            if (obs) {

               obs->remove_object_module (PluginPtr->get_plugin_name (), *_objectModule);
            }

            ptr = _extensions.get_next (it);
         }

         _objectModule = 0;
      }

      _extensions.remove_external_plugin (PluginPtr);
   }
}


void
dmz::RenderModuleCoreOgreBasic::update_time_slice (const Float64 TimeDelta) {

   if (_root) {

      Ogre::WindowEventUtilities::messagePump();
      _root->renderOneFrame ();
   }
}


// Object Observer Interface
void
dmz::RenderModuleCoreOgreBasic::update_object_position (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Value,
      const Vector *PreviousValue) {

   if (AttributeHandle == _defaultAttributeHandle) {

      Ogre::SceneNode *node = _dynamicObjectTable.lookup (ObjectHandle);

      if (node) {

         node->setPosition (to_ogre_vector (Value));
      }
   }
}


void
dmz::RenderModuleCoreOgreBasic::update_object_orientation (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Matrix &Value,
      const Matrix *PreviousValue) {

   if (AttributeHandle == _defaultAttributeHandle) {

      Ogre::SceneNode *node = _dynamicObjectTable.lookup (ObjectHandle);

      if (node) {

         node->setOrientation (to_ogre_quaternion (Value));
      }
   }
}


// RenderModuloeCoreOgre Interface
Ogre::SceneManager *
dmz::RenderModuleCoreOgreBasic::get_scene_manager () { return _sceneManager; }


dmz::Boolean
dmz::RenderModuleCoreOgreBasic::add_camera (
      const String &PortalName,
      Ogre::Camera *camera) {

   Boolean result (False);

   PortalStruct *ps = _get_portal_struct (PortalName);
   if (ps && camera) {

      ps->camera = camera;
      result = True;
   }

   return result;
}


Ogre::Camera *
dmz::RenderModuleCoreOgreBasic::lookup_camera (const String &PortalName) {

   Ogre::Camera *result = 0;

   PortalStruct *ps = _portalTable.lookup (PortalName);
   if (ps) {

      result = ps->camera;
   }

   return result;
}


Ogre::Camera *
dmz::RenderModuleCoreOgreBasic::remove_camera (const String &PortalName) {

   Ogre::Camera *result = 0;

   PortalStruct *ps = _portalTable.lookup (PortalName);
   if (ps) {

      result = ps->camera;
      ps->camera = 0;
   }

   return result;
}


dmz::Boolean
dmz::RenderModuleCoreOgreBasic::add_dynamic_object (
      const Handle ObjectHandle,
      Ogre::SceneNode *node) {

   Boolean retVal (False);

   if (node) {

      if (_dynamicObjectTable.store (ObjectHandle, node)) {

         try {

            _sceneManager->getRootSceneNode ()->addChild (node);
            retVal = True;
         }
         catch (Ogre::Exception e) {

            _log.error << e.getFullDescription ().c_str () << endl;
         }
      }
   }

   return retVal;
}


Ogre::SceneNode *
dmz::RenderModuleCoreOgreBasic::lookup_dynamic_object (const Handle ObjectHandle) {

   return _dynamicObjectTable.lookup (ObjectHandle);
}


Ogre::SceneNode *
dmz::RenderModuleCoreOgreBasic::remove_dynamic_object (const Handle ObjectHandle) {

   Ogre::SceneNode *node (_dynamicObjectTable.remove (ObjectHandle));

   if (node && _sceneManager) {

      _sceneManager->getRootSceneNode ()->removeChild (node);
   }

   return node;
}


void
dmz::RenderModuleCoreOgreBasic::OnSceneManagerCreate (
      Ogre::SceneManager *pManager,
      TiXmlElement* pNodeDesc) {

   if (pManager) {

      _sceneManager = pManager;

      PluginIterator it;

      Plugin *ptr (_extensions.get_first (it));

      while (ptr) {

         ptr->discover_plugin (PluginDiscoverAdd, this);

         ptr = _extensions.get_next (it);
      }
   }
}


void
dmz::RenderModuleCoreOgreBasic::_init (Config &local, Config &global) {

   Definitions defs (get_plugin_runtime_context (), &_log);

   _defaultAttributeHandle = defs.create_named_handle (ObjectAttributeDefaultName);

   Config pluginList;

   if (local.lookup_all_config ("plugin-list.plugin", pluginList)) {

      RuntimeContext *context (get_plugin_runtime_context ());
      dmz::load_plugins (context, pluginList, local, global, _extensions, &_log);
   }
   else {

   }

   Config ogreLocal;

   if (local.lookup_config ("ogre", ogreLocal)) {

      try {

         _init_root (ogreLocal);
         _init_resources (ogreLocal);
         _init_render_system (ogreLocal);
         _init_render_windows ();

         Ogre::ResourceGroupManager::getSingleton ().initialiseAllResourceGroups ();

         _init_scene_manager (ogreLocal);
      }
      catch (Ogre::Exception e) {

         if (_root) {

            if (_sceneManager) {

               _root->destroySceneManager (_sceneManager);
               _sceneManager = 0;
            }

            delete _root;
            _root = 0;
         }

         _log.error << e.getFullDescription ().c_str () << endl;
      }
   }
   else {

   }

   _extensions.discover_plugins ();
}


void
dmz::RenderModuleCoreOgreBasic::_init_root (Config &local) {

   String logName ("ogre.log");
   Boolean suppressLogFileOutput (True);

   Config logData;
   if (local.lookup_config ("log", logData)) {

      suppressLogFileOutput = False;
      logName = config_to_string ("name", logData, logName);
   }

   Ogre::LogManager *logManager = new Ogre::LogManager;

   Ogre::Log *log =
      Ogre::LogManager::getSingleton ().createLog (
         logName.get_buffer (), True, False, suppressLogFileOutput);

   if (suppressLogFileOutput) {

      //Ogre::LogManager::getSingleton ().setLogDetail(Ogre::LL_LOW);
      log->addListener (&_ogreLogger);
   }

   _root = new Ogre::Root("", "");

   ConfigIterator it;
   Config cd;

   Config pluginList;
   if (local.lookup_all_config ("plugin-list.plugin", pluginList)) {

      Boolean done (!pluginList.get_first_config (it, cd));
      while (!done)  {

         String name;
         if (cd.lookup_attribute ("name", name)) {

            try {

               _root->loadPlugin (name.get_buffer ());
            }
            catch (Ogre::Exception e) {

               String msg (e.getFullDescription ().c_str ());
               _log.error << msg << endl;
            }
         }

         done = !pluginList.get_next_config (it, cd);
      }
   }
}


void
dmz::RenderModuleCoreOgreBasic::_init_resources (Config &local) {

   Config resourceList;
   if (local.lookup_all_config ("resources.resource", resourceList)) {

      ConfigIterator it;
      Config cd;

      Boolean done (!resourceList.get_first_config (it, cd));
      while (!done)  {

         String name;
         if (cd.lookup_attribute ("name", name)) {

            String type = config_to_string ("type", cd, "FileSystem");
            String group = config_to_string ("group", cd, "General");
            Boolean recursive = config_to_boolean ("recursive", cd, False);

            try {

               Ogre::ResourceGroupManager::getSingleton ().addResourceLocation (
                  name.get_buffer (),
                  type.get_buffer (),
                  group.get_buffer (),
                  recursive);
            }
            catch (Ogre::Exception e) {

               _log.error << e.getFullDescription ().c_str () << endl;
            }
         }

         done = !resourceList.get_next_config (it, cd);
      }
   }
}


void
dmz::RenderModuleCoreOgreBasic::_init_render_system (Config &local) {

   if (_root) {

      String renderSystemName (
         config_to_string ("renderSystem.name", local, "OpenGL"));

      Ogre::RenderSystem *renderSystem (0);

      Ogre::RenderSystemList *renderSystemList = _root->getAvailableRenderers ();
      if (renderSystemList) {

         Ogre::RenderSystemList::iterator it = renderSystemList->begin ();
         while (it != renderSystemList->end () && !renderSystem) {

            if (renderSystemList->size () == 1) {

               renderSystem = *it;
            }
            else {

               Ogre::String curName ((*it)->getName ());
               if (curName.find (renderSystemName.get_buffer ()) >= 0) {

                  renderSystem = *it;
               }
            }

            it++;
         }
      }

      if (renderSystem) {

         _root->setRenderSystem (renderSystem);
         _root->initialise (false); // don't autocreate a window
      }
      else {

         _log.error
            << "Specified render system (" << renderSystemName << ") not found" << endl;
      }
   }
}


void
dmz::RenderModuleCoreOgreBasic::_init_render_windows () {

   PluginIterator it;

   Plugin *ptr (_extensions.get_first (it));

   while (ptr) {

      RenderExtWindowOgre *renderWindowExt = RenderExtWindowOgre::cast (ptr);

      if (renderWindowExt) {

         if (renderWindowExt->init_render_window ()) {

            PortalStruct *ps =
               _get_portal_struct (renderWindowExt->get_render_window_name ());
         }
      }

      ptr = _extensions.get_next (it);
   }
}


void
dmz::RenderModuleCoreOgreBasic::_init_scene_manager (Config &local) {

   if (_root) {

      Ogre::RenderSystem *renderSystem = _root->getRenderSystem ();

      Config sceneData;
      if (local.lookup_config ("scene", sceneData)) {

         const String SceneFileName = config_to_string ("file", sceneData);

         if (SceneFileName && renderSystem) {

            Ogre::RenderWindow *renderWindow =
               dynamic_cast<Ogre::RenderWindow *> (
                  renderSystem->getRenderTarget (DefaultPortalNameOgre));

            if (renderWindow) {

               _log.info << "Loading scene: " << SceneFileName << endl;

               _osmSceneLoader = new OSMScene (0, renderWindow);

               _osmSceneLoader->initialise (SceneFileName.get_buffer (), this);

               _osmSceneLoader->createScene ();

               _sceneManager = _osmSceneLoader->getSceneManager ();
            }
         }
      }
      else {

         _log.error << "No scene file listed for loading" << endl;
      }
   }
}


dmz::RenderModuleCoreOgreBasic::PortalStruct *
dmz::RenderModuleCoreOgreBasic::_get_portal_struct (const String &PortalName) {

   PortalStruct *ps = _portalTable.lookup (PortalName);
   if (!ps) {

      ps = new PortalStruct (PortalName);
      if (!_portalTable.store (PortalName, ps)) { delete ps; ps = 0; }
   }

   return ps;
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzRenderModuleCoreOgreBasic (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::RenderModuleCoreOgreBasic (Info, local, global);
}

};
