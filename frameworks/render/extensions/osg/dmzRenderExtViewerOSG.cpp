#include "dmzRenderExtViewerOSG.h"
#include <dmzInputModule.h>
#include <dmzRenderModuleCoreOSG.h>
#include <dmzRenderCameraManipulatorOSG.h>
#include <dmzRenderEventHandlerOSG.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzTypesVector.h>
#include <dmzTypesMatrix.h>

dmz::RenderExtViewerOSG::RenderExtViewerOSG (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      TimeSlice (Info),
      _log (Info),
      _core (0),
      _channels (0),
      _portalName (DefaultPortalNameOSG),
      _camera (0),
      _cameraManipulator (0),
      _eventHandler (0),
      _viewer (0) {

   _viewer = new osgViewer::Viewer;
   _viewer->setThreadingModel (osgViewer::Viewer::SingleThreaded);
   _cameraManipulator = new RenderCameraManipulatorOSG;
   _eventHandler = new RenderEventHandlerOSG (get_plugin_runtime_context ());

   osgViewer::StatsHandler *stats = new osgViewer::StatsHandler;
   stats->setKeyEventTogglesOnScreenStats (osgGA::GUIEventAdapter::KEY_F1);
   stats->setKeyEventPrintsOutStats (osgGA::GUIEventAdapter::KEY_F2);
   _viewer->addEventHandler (stats);
   _viewer->setCameraManipulator (_cameraManipulator.get ());
   _viewer->addEventHandler (_eventHandler.get ());
   _viewer->setKeyEventSetsDone (0);
   _viewer->setQuitEventSetsDone (true);

   _init (local);

   _camera = _viewer->getCamera ();
}


dmz::RenderExtViewerOSG::~RenderExtViewerOSG () {

    _viewer = 0;
    _eventHandler = 0;
}


// Plugin Interface
void
dmz::RenderExtViewerOSG::update_plugin_state (
      const PluginStateEnum State,
      const UInt32 Level) {

   if (State == PluginStateStart) {

      if (_viewer.valid ()) {

         _viewer->realize ();
      }
   }
}


void
dmz::RenderExtViewerOSG::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      if (!_core) {

         _core = RenderModuleCoreOSG::cast (PluginPtr);
         if (_core) {

            osg::ref_ptr<osg::Group> scene = _core->get_scene ();
            if (scene.valid ()) { _viewer->setSceneData (scene.get ()); }

            if (_cameraManipulator.valid ()) {

               _core->add_camera_manipulator (_portalName, _cameraManipulator.get ());
            }

            if (_camera.valid ()) {

               _core->add_camera (_portalName, _camera.get ());
            }
         }
      }

      if (!_channels) {

         _channels = InputModule::cast (PluginPtr);
         if (_channels) {

            Definitions defs (get_plugin_runtime_context (), &_log);
            const Handle SourceHandle = defs.create_named_handle (_portalName);
            _eventHandler->set_input_module_channels (_channels, SourceHandle);
         }
      }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_core && (_core == RenderModuleCoreOSG::cast (PluginPtr))) {

         _viewer->setCameraManipulator (0);
         _core->remove_camera (_portalName);
         _core->remove_camera_manipulator (_portalName);
         osg::ref_ptr<osg::Group> scene = new osg::Group;
         if (scene.valid ()) { _viewer->setSceneData (scene.get ()); }
         _core = 0;
      }

      if (_channels && (_channels == InputModule::cast (PluginPtr))) {

         _eventHandler->set_input_module_channels (0, 0);
         _channels = 0;
      }
   }
}


void
dmz::RenderExtViewerOSG::update_time_slice (const Float64 TimeDelta) {

   if (_viewer.valid ()) {

      if (_viewer->done ()) {

         Exit (get_plugin_runtime_context ()).request_exit (
            ExitStatusNormal,
            "Application Quit.");
      }
      else { _viewer->frame (); } // Render a complete new frame
   }
}




void
dmz::RenderExtViewerOSG::_init (const Config &Local) {

   _portalName = config_to_string ("portal.name", Local, DefaultPortalNameOSG);

   Config windowData;

   if (Local.lookup_all_config ("window", windowData)) {

      ConfigIterator it;
      Config cd;

      Boolean found (windowData.get_first_config (it, cd));
      if (found)  {

         UInt32 windowLeft = config_to_uint32 ("left", cd);
         UInt32 windowTop = config_to_uint32 ("top", cd);
         UInt32 windowWidth = config_to_uint32 ("width", cd);
         UInt32 windowHeight = config_to_uint32 ("height", cd);
         UInt32 screen = config_to_uint32 ("screen", cd);

         __init_viewer_window (windowLeft, windowTop, windowWidth, windowHeight, screen);
      }

      _log.info << "Loading viewer windowed" << endl;
   }
   else if (Local.lookup_all_config ("fullscreen", windowData)) {

      ConfigIterator it;
      Config cd;

      Boolean found (windowData.get_first_config (it, cd));
      if (found)  {

         UInt32 screen = config_to_uint32 ("screen", cd);
         __init_viewer_fullscreen (screen);
      }

      _log.info << "Loading viewer full-screen" << endl;
   }
   else {

      __init_viewer_window (100, 100, 800, 600, 0);
      _log.info << "Loading viewer windowed with defaults" << endl;
   }
}


void
dmz::RenderExtViewerOSG::__init_viewer_window (
      UInt32 windowLeft,
      UInt32 windowTop,
      UInt32 windowWidth,
      UInt32 windowHeight,
      UInt32 screen) {

   if (_viewer.valid ()) {

      _viewer->setUpViewInWindow (
         windowLeft,
         windowTop,
         windowWidth,
         windowHeight,
         screen);
   }
}


void
dmz::RenderExtViewerOSG::__init_viewer_fullscreen (UInt32 screen) {

   if (_viewer.valid ()) {

      _viewer->setUpViewOnSingleScreen (screen);
      if (_viewer->done ()) { _log.error << "The viewer thinks it is done?" << endl; }
   }
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzRenderExtViewerOSG (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::RenderExtViewerOSG (Info, local);
}

};
