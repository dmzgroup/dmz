#include "dmzRenderModuleCoreOSGBasic.h"
#include <dmzObjectModule.h>
#include <dmzObjectAttributeMasks.h>
#include <dmzRenderObjectDataOSG.h>
#include <dmzRenderUtilOSG.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeConfigToPathContainer.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzRuntimeLoadPlugins.h>
#include <dmzSystem.h>
#include <dmzSystemFile.h>
#include <osg/DeleteHandler>
#include <osg/LightSource>
#include <osg/Referenced>
#include <osgDB/Registry>


dmz::RenderModuleCoreOSGBasic::RenderModuleCoreOSGBasic (
      const PluginInfo &Info,
      Config &local,
      Config &global) :
      Plugin (Info),
      TimeSlice (Info),
      ObjectObserverUtil (Info, local),
      RenderModuleCoreOSG (Info),
      _log (Info),
      _cullMask (0x01),
      _isectMask (0x10),
      _defaultHandle (0),
      _dirtyObjects (0) {

   _scene = new osg::Group;

   _overlay = new osg::Group;
   _scene->addChild (_overlay.get ());

   _isect = new osg::Group;
   _scene->addChild (_isect.get ());

   _staticObjects = new osg::Group;
   _isect->addChild (_staticObjects.get ());

   _dynamicObjects = new osg::Group;
   _dynamicObjects->setDataVariance (osg::Object::DYNAMIC);
   _isect->addChild (_dynamicObjects.get ());

   _init (local, global);
}


dmz::RenderModuleCoreOSGBasic::~RenderModuleCoreOSGBasic () {

   _extensions.remove_plugins ();
   _objectTable.empty ();
   _viewerTable.empty ();

   osg::DeleteHandler *dh (osg::Referenced::getDeleteHandler ());

   if (dh) { dh->flush (); }
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
   }
   else if (Mode == PluginDiscoverRemove) {

      _extensions.remove_external_plugin (PluginPtr);
   }
}


// Time Slice Interface
void
dmz::RenderModuleCoreOSGBasic::update_time_slice (const Float64 DeltaTime) {

   while (_dirtyObjects) {

      ObjectStruct *os (_dirtyObjects);
      _dirtyObjects = os->next;

      os->transform->setMatrix (to_osg_matrix (os->ori, os->pos));

      if (os->destroyed) { 

         if (_dynamicObjects.valid ()) {

            _dynamicObjects->removeChild (os->transform.get ());
         }

         delete os; os = 0;
      }
      else { os->next = 0; os->dirty = False; }
   }
}


// Object Observer Interface
void
dmz::RenderModuleCoreOSGBasic::destroy_object (
      const UUID &Identity,
      const Handle ObjectHandle) {

   ObjectStruct *os (_objectTable.remove (ObjectHandle));

   if (os) {

      if (os->dirty) { os->destroyed = True; }
      else {

         if (_dynamicObjects.valid ()) {

            _dynamicObjects->removeChild (os->transform.get ());
         }

         delete os; os = 0;
      }
   }
}


void
dmz::RenderModuleCoreOSGBasic::update_object_position (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Value,
      const Vector *PreviousValue) {

   ObjectStruct *os (_objectTable.lookup (ObjectHandle));

   if (os) {

      os->pos = Value;

      if (!os->dirty) {

         os->dirty = True;
         os->next = _dirtyObjects;
         _dirtyObjects = os;
      }
   }
}


void
dmz::RenderModuleCoreOSGBasic::update_object_orientation (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Matrix &Value,
      const Matrix *PreviousValue) {

   ObjectStruct *os (_objectTable.lookup (ObjectHandle));

   if (os) {

      os->ori = Value;

      if (!os->dirty) {

         os->dirty = True;
         os->next = _dirtyObjects;
         _dirtyObjects = os;
      }
   }
}

// RenderModuleCoreOSG Interface
dmz::UInt32
dmz::RenderModuleCoreOSGBasic::get_cull_mask () { return _cullMask; }


dmz::UInt32
dmz::RenderModuleCoreOSGBasic::get_isect_mask () { return _isectMask; }


osg::Group *
dmz::RenderModuleCoreOSGBasic::get_scene () { return _scene.get (); }


osg::Group *
dmz::RenderModuleCoreOSGBasic::get_overlay () { return _overlay.get (); }


osg::Group *
dmz::RenderModuleCoreOSGBasic::get_isect () { return _isect.get (); }

osg::Group *
dmz::RenderModuleCoreOSGBasic::get_static_objects () { return _staticObjects.get (); }


osg::Group *
dmz::RenderModuleCoreOSGBasic::get_dynamic_objects () { return _dynamicObjects.get (); }


osg::Group *
dmz::RenderModuleCoreOSGBasic::create_dynamic_object (const Handle ObjectHandle) {

   osg::Group *result (0);

   ObjectStruct *os (_objectTable.lookup (ObjectHandle));

   if (!os) {

      os = new ObjectStruct;

      if (os && !_objectTable.store (ObjectHandle, os)) { delete os; os = 0; }

      if (os) {

         os->transform->setUserData (new RenderObjectDataOSG (ObjectHandle));
         os->transform->setDataVariance (osg::Object::DYNAMIC);

         ObjectModule *objMod (get_object_module ());

         if (objMod) {

            objMod->lookup_position (ObjectHandle, _defaultHandle, os->pos);
            objMod->lookup_orientation (ObjectHandle, _defaultHandle, os->ori);
            os->transform->setMatrix (to_osg_matrix (os->ori, os->pos));
         }
         else {

            os->dirty = True;
            os->next = _dirtyObjects;
            _dirtyObjects = os;
         }

         if (_dynamicObjects.valid ()) {

            _dynamicObjects->addChild (os->transform.get ());
         }
      }
   }

   if (os) { result = os->transform.get (); }

   return result;
}


osg::Group *
dmz::RenderModuleCoreOSGBasic::lookup_dynamic_object (const Handle ObjectHandle) {

   osg::Group *result (0);

   ObjectStruct *os (_objectTable.lookup (ObjectHandle));

   if (os) { result = os->transform.get (); }

   return result;
}


dmz::Boolean
dmz::RenderModuleCoreOSGBasic::add_viewer (
      const String &ViewerName,
      osgViewer::Viewer *viewer) {

   Boolean result (False);

   ViewerStruct *vs = new ViewerStruct (ViewerName, viewer);

   if (vs && _viewerTable.store (ViewerName, vs)) {

      result = True;

      if (viewer) {

         osg::Camera *camera = viewer->getCamera ();

         if (camera) { camera->setCullMask (_cullMask); }
      }
   }
   else if (vs) { delete vs; vs = 0; }

   return result;
}


osgViewer::Viewer *
dmz::RenderModuleCoreOSGBasic::lookup_viewer (const String &ViewerName) {

   ViewerStruct *vs (_viewerTable.lookup (ViewerName));

   return vs ? vs->viewer.get () : 0;
}


osgViewer::Viewer *
dmz::RenderModuleCoreOSGBasic::remove_viewer (const String &ViewerName) {

   osgViewer::Viewer *result (0);
   ViewerStruct *vs (_viewerTable.remove (ViewerName));

   if (vs) {

      result = vs->viewer.get ();
      delete vs; vs = 0;
   }

   return result;
}


void
dmz::RenderModuleCoreOSGBasic::_init (Config &local, Config &global) {

   Config pluginList;

   if (local.lookup_all_config ("plugin-list.plugin", pluginList)) {

      RuntimeContext *context (get_plugin_runtime_context ());

      if (dmz::load_plugins (context, pluginList, local, global, _extensions, &_log)) {

         _extensions.discover_plugins ();
         _extensions.discover_external_plugin (this);
      }
   }

   osgDB::Registry *reg = osgDB::Registry::instance ();
   Config pathList;

   if (reg && local.lookup_all_config ("loader.path", pathList)) {

      osgDB::FilePathList &fpl = reg->getLibraryFilePathList ();

      ConfigIterator it;
      Config path;

      while (pathList.get_next_config (it, path)) {

         String pathStr = config_to_string ("value", path);

         if (get_absolute_path (pathStr, pathStr)) {

            fpl.push_back (pathStr.get_buffer ());
         }
      }
   }

   _defaultHandle = activate_default_object_attribute (
      ObjectDestroyMask | ObjectPositionMask | ObjectOrientationMask);
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
