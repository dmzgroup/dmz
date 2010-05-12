#include "dmzRenderExtViewerQtOSG.h"
#include <dmzInputModule.h>
#include <dmzQtConfigRead.h>
#include <dmzRenderConsts.h>
#include <dmzRenderEventHandlerOSG.h>
#include <dmzRenderModuleCoreOSG.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRenderConfigToOSG.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzTypesVector.h>
#include <dmzTypesMatrix.h>
#include "GraphicsWindowQt.h"
#include <osg/Camera>
#include <osgViewer/ViewerEventHandlers>
#include <QtGui/QLayout>

dmz::RenderExtViewerQtOSG::RenderExtViewerQtOSG (
      const PluginInfo &Info,
      Config &local) :
      QFrame (0),
      Plugin (Info),
      QtWidget (Info),
      TimeSlice (Info),
      _log (Info),
      _title ("DMZ Application"),
      _core (0),
      _channels (0),
      _layout (0),
      _viewerName (RenderMainPortalName),
      _eventHandler (0),
      _viewer (0),
      _view (0) {

   _init (local);
}


dmz::RenderExtViewerQtOSG::~RenderExtViewerQtOSG () {

   _view = 0;
   _viewer = 0;
   _eventHandler = 0;
}


QWidget *
dmz::RenderExtViewerQtOSG::get_qt_widget () {
   
   return this;
}


// Plugin Interface
void
dmz::RenderExtViewerQtOSG::update_plugin_state (
      const PluginStateEnum State,
      const UInt32 Level) {

   if (State == PluginStateInit) {

_log.warn << "!" << endl;
      if (_view.valid ()) {

_log.warn << "!!" << endl;
         osg::Camera *camera = _view->getCamera ();
         if (camera) {
            
_log.warn << "!!!" << endl;
            GraphicsWindowQt* gw = dynamic_cast<GraphicsWindowQt*> (camera->getGraphicsContext ());
            if (gw) {

_log.warn << "!!!!" << endl;
               QWidget *widget = gw->getGraphWidget ();
               _layout->addWidget (widget);

               // widget->show ();
            }
         }
      }
   }
   else if (State == PluginStateStart) {

_log.error << "!" << endl;
      if (_viewer.valid ()) {

_log.error << "!!" << endl;
         _viewer->realize ();

         if (!_viewer->isRealized ()) {

            _log.error << "Failed to realize OSG viewer." << endl;
         }
         else {
            
_log.error << "!!!" << endl;
            show ();
// 
//             osgViewer::ViewerBase::Windows w;
//             _viewer->getWindows (w);
// 
//             osgViewer::GraphicsWindow *gw = w.front ();
// 
//             if (gw) {
// 
// #if defined(__APPLE__) || defined(MACOSX)
//                // WARNING This call does not seem to work under Win32. Fortunately
//                // The OSG window comes up in focus under Win32. For the Mac, this
//                // gives the window focus on startup
//                // gw->grabFocus ();
// #endif
//                gw->setWindowName (_title.get_buffer ());
// 
//                int width (0), height (0), extra (0);
//                gw->getWindowRectangle (extra, extra, width, height);
//                
// _log.warn << "width: " << width << endl;
// _log.warn << "height: " << height << endl;
//                _eventHandler->set_portal_size (width, height);
//             }
         }
      }
   }
}


void
dmz::RenderExtViewerQtOSG::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      if (!_core) {

         _core = RenderModuleCoreOSG::cast (PluginPtr);
         if (_core && _view.valid ()) {

            osg::ref_ptr<osg::Group> scene = _core->get_scene ();

            if (scene.valid ()) { _view->setSceneData (scene.get ()); }

            _core->add_view (_viewerName, _view.get ());
         }
      }

      if (!_channels) {

         _channels = InputModule::cast (PluginPtr);
         if (_channels) {

            Definitions defs (get_plugin_runtime_context (), &_log);
            const Handle SourceHandle = defs.create_named_handle (_viewerName);
            _eventHandler->set_input_module_channels (_channels, SourceHandle);
         }
      }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_core && (_core == RenderModuleCoreOSG::cast (PluginPtr))) {

         _core->remove_view (_viewerName);
         osg::ref_ptr<osg::Group> scene = new osg::Group;
         if (scene.valid () && _view.valid ()) { _view->setSceneData (scene.get ()); }
         _core = 0;
      }

      if (_channels && (_channels == InputModule::cast (PluginPtr))) {

         _eventHandler->set_input_module_channels (0, 0);
         _channels = 0;
      }
   }
}


void
dmz::RenderExtViewerQtOSG::update_time_slice (const Float64 TimeDelta) {

   if (_viewer.valid ()) {

      if (_viewer->done ()) {

         Exit (get_plugin_runtime_context ()).request_exit (
            ExitStatusNormal,
            "OSG View Done.");
      }
      else {
       
         _viewer->frame (); // Render a complete new frame
      }
   }
}


void
dmz::RenderExtViewerQtOSG::paintEvent (QPaintEvent *event) {
   
   // if (_viewer.valid ()) {
   // 
   //    if (!_viewer->done ()) { _viewer->frame (); }
   // }
}


void
dmz::RenderExtViewerQtOSG::resizeEvent (QResizeEvent *event) {
   
   if (event && _eventHandler) {

      QSize size = event->size ();
      _eventHandler->set_portal_size (size.width (), size.height ());
   }
   
   QWidget::resizeEvent (event);
}


void
dmz::RenderExtViewerQtOSG::_init (Config &local) {

   setObjectName (get_plugin_name ().get_buffer ());

   _eventHandler = new RenderEventHandlerOSG (get_plugin_runtime_context (), local);
   
   _viewer = new osgViewer::CompositeViewer;
   _viewer->setThreadingModel (osgViewer::CompositeViewer::SingleThreaded);

   _viewer->setKeyEventSetsDone (0);
   _viewer->setQuitEventSetsDone (true);

   osg::DisplaySettings *ds = osg::DisplaySettings::instance ();
   if (ds) {

      const Int32 Samples = config_to_int32 ("aa.samples", local, 0);
      _log.info << "Using: " << Samples << " AA samples." << endl;
      ds->setNumMultiSamples (Samples);
   }

   _viewerName = config_to_string ("portal.name", local, _viewerName);

   _title = config_to_string ("window-title.value", local, _title);

   setWindowTitle (_title.get_buffer ());

   // const Boolean Fullscreen = config_to_boolean ("window.fullscreen", local, False);
   // const Boolean Centered = config_to_boolean ("window.center", local, True);
   // Int32 windowLeft = config_to_uint32 ("window.left", local, 100);
   // Int32 windowTop = config_to_uint32 ("window.top", local, 100);
   const UInt32 WindowWidth = config_to_uint32 ("window.width", local, 800);
   const UInt32 WindowHeight = config_to_uint32 ("window.height", local, 600);
   // const UInt32 Screen = config_to_uint32 ("window.screen", local, 0);

   // if (Fullscreen) { __init_viewer_fullscreen (Screen); }
   // else {
   //       
   //    if (Centered) {
   // 
   //       __init_centered (Screen, WindowWidth, WindowHeight, windowLeft, windowTop);
   //    }
   // 
   //    __init_viewer_window (windowLeft, windowTop, WindowWidth, WindowHeight, Screen);
   // }

   // _log.info << "Viewer Info: ";
   // 
   // if (Fullscreen) { _log.info << "Full Screen: "; }
   // else {
   // 
   //    _log.info << WindowWidth << "x" << WindowHeight;
   // 
   //    if (Centered) { _log.info << " [Centered]"; }
   // 
   //    _log.info << " Corner: " << windowLeft << ", " << windowTop << " Screen: ";
   // }
   // 
   // _log.info << Screen << endl;

   resize (WindowWidth, WindowHeight);
   
   osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
   traits->windowName = _title.get_buffer ();
   traits->windowDecoration = False;
   traits->x = 100;
   traits->y = 100;
   traits->width = WindowWidth;
   traits->height = WindowHeight;
   traits->doubleBuffer = True;
   traits->alpha = ds->getMinimumNumAlphaBits ();
   traits->stencil = ds->getMinimumNumStencilBits ();
   traits->sampleBuffers = ds->getMultiSamples ();
   traits->samples = ds->getNumMultiSamples ();
   
   osg::ref_ptr<osg::Camera> camera = new osg::Camera;
   camera->setGraphicsContext (new GraphicsWindowQt (traits.get ()));
   
   const osg::Vec4 DefaultColor = camera->getClearColor ();
   const osg::Vec4 Color = config_to_osg_vec4_color ("clear-color", local, DefaultColor);
   camera->setClearColor (Color);
   
   camera->setViewport (new osg::Viewport (0, 0, traits->width, traits->height));
   
   camera->setProjectionMatrixAsPerspective(
      30.0f, double(traits->width) / double(traits->height), 1.0f, 10000.0f);
   
   _view = new osgViewer::View;
   _view->setCamera (camera);

   _viewer->addView (_view);
   
   osg::ref_ptr<osgViewer::StatsHandler> stats = new osgViewer::StatsHandler;
   stats->setKeyEventTogglesOnScreenStats (osgGA::GUIEventAdapter::KEY_F1);
   stats->setKeyEventPrintsOutStats (osgGA::GUIEventAdapter::KEY_F2);
   _view->addEventHandler (stats.get ());
   _view->addEventHandler (_eventHandler.get ());
   
   _layout = new QGridLayout ();
   _layout->setContentsMargins (0, 0, 0, 0);

   qframe_config_read ("frame", local, this);
   
   camera.release ();
   
   setLayout (_layout);
   
   // show ();
}

/*
void
dmz::RenderExtViewerQtOSG::__init_centered (
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
dmz::RenderExtViewerQtOSG::__init_viewer_window (
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
dmz::RenderExtViewerQtOSG::__init_viewer_fullscreen (const UInt32 Screen) {

   if (_viewer.valid ()) {

      _viewer->setUpViewOnSingleScreen (Screen);
      if (_viewer->done ()) { _log.error << "The viewer thinks it is done?" << endl; }
   }
}
*/


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzRenderExtViewerQtOSG (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::RenderExtViewerQtOSG (Info, local);
}

};
