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
      _title ("DMZ"),
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
   _eventHandler = new RenderEventHandlerOSG (get_plugin_runtime_context (), local);

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

         osgViewer::ViewerBase::Windows w;
         _viewer->getWindows (w);

         osgViewer::GraphicsWindow *gw = w.front ();

         if (gw) {

#if defined(__APPLE__) || defined(MACOSX)
            // WARNING This call does not seem to work under Win32. Fortunately
            // The OSG window comes up in focus under Win32. For the Mac, this
            // gives the window focus on startup
            gw->grabFocus ();
#endif
            gw->setWindowName (_title.get_buffer ());
         }
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

   _title = config_to_string ("window-title.value", Local, _title);

   const Boolean Fullscreen = config_to_boolean ("window.fullscreen", Local, False);
   const Boolean Centered = config_to_boolean ("window.center", Local, True);
   Int32 windowLeft = config_to_uint32 ("window.left", Local, 100);
   Int32 windowTop = config_to_uint32 ("window.top", Local, 100);
   const UInt32 WindowWidth = config_to_uint32 ("window.width", Local, 800);
   const UInt32 WindowHeight = config_to_uint32 ("window.height", Local, 600);
   const UInt32 Screen = config_to_uint32 ("window.screen", Local, 0);

   if (Fullscreen) { __init_viewer_fullscreen (Screen); }
   else {
         
      if (Centered) {

         __init_centered (Screen, WindowWidth, WindowHeight, windowLeft, windowTop);
      }

      __init_viewer_window (windowLeft, windowTop, WindowWidth, WindowHeight, Screen);
   }

   _log.info << "Viewer Info: ";

   if (Fullscreen) { _log.info << "Full Screen: "; }
   else {

      _log.info << WindowWidth << "x" << WindowHeight;

      if (Centered) { _log.info << " [Centered]"; }

      _log.info << " Corner: " << windowLeft << ", " << windowTop << " Screen: ";
   }

   _log.info << Screen << endl;
}


void
dmz::RenderExtViewerOSG::__init_centered (
      const UInt32 Screen,
      const UInt32 WindowWidth,
      const UInt32 WindowHeight,
      Int32 &windowLeft,
      Int32 &windowTop) {

   osg::GraphicsContext::WindowingSystemInterface* wsi =
      osg::GraphicsContext::getWindowingSystemInterface();

   if (wsi) {

      osg::GraphicsContext::ScreenIdentifier si;

#if 0
      si.readDISPLAY();
      // displayNum has not been set so reset it to 0.
      if (si.displayNum < 0) { si.displayNum = 0; {
#endif

      si.screenNum = Screen;

      unsigned int width = 0, height = 0;

      wsi->getScreenResolution (si, width, height);

      const UInt32 HalfScreenWidth (width / 2);
      const UInt32 HalfScreenHeight (height / 2);
      const UInt32 HalfWindowWidth (WindowWidth / 2);
      const UInt32 HalfWindowHeight (WindowHeight / 2);

      windowLeft = (Int32)HalfScreenWidth - (Int32)HalfWindowWidth;
      windowTop = (Int32)HalfScreenHeight - (Int32)HalfWindowHeight;
   }
}


void
dmz::RenderExtViewerOSG::__init_viewer_window (
      const Int32 WindowLeft,
      const Int32 WindowTop,
      const UInt32 WindowWidth,
      const UInt32 WindowHeight,
      const UInt32 Screen) {

   if (_viewer.valid ()) {

      _viewer->setUpViewInWindow (
         WindowLeft,
         WindowTop,
         WindowWidth,
         WindowHeight,
         Screen);
   }
}


void
dmz::RenderExtViewerOSG::__init_viewer_fullscreen (const UInt32 Screen) {

   if (_viewer.valid ()) {

      _viewer->setUpViewOnSingleScreen (Screen);
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
