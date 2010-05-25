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
dmz::RenderExtViewerQtOSG::get_qt_widget () { return this; }


// Plugin Interface
void
dmz::RenderExtViewerQtOSG::update_plugin_state (
      const PluginStateEnum State,
      const UInt32 Level) {

   if (State == PluginStateInit) {

   }
   else if (State == PluginStateStart) {

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
            "OSG Viewer Done.");
      }
      else {

         _viewer->frame (); // Render a complete new frame
      }
   }
}


void
dmz::RenderExtViewerQtOSG::_init (Config &local) {

   setObjectName (get_plugin_name ().get_buffer ());

   setAttribute (Qt::WA_PaintOnScreen);
   setAttribute (Qt::WA_OpaquePaintEvent);
   
   qframe_config_read ("frame", local, this);

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

   osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
   traits->windowName = "qt-osg-viewer";
   traits->windowDecoration = False;
   traits->doubleBuffer = True;
   traits->x = x ();
   traits->y = y ();
   traits->width = width ();
   traits->height = height ();
   traits->alpha = ds->getMinimumNumAlphaBits ();
   traits->stencil = ds->getMinimumNumStencilBits ();
   traits->sampleBuffers = ds->getMultiSamples ();
   traits->samples = ds->getNumMultiSamples ();
   
   osg::ref_ptr<osg::Camera> camera = new osg::Camera;
   GraphicsWindowQt* gw = new GraphicsWindowQt (traits.get (), 0);
   camera->setGraphicsContext (gw);
   
   QGridLayout *layout = new QGridLayout ();
   layout->setContentsMargins (0, 0, 0, 0);
   layout->addWidget (gw->getGraphWidget ());
   setLayout (layout);
   
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
   
   camera.release ();
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzRenderExtViewerQtOSG (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::RenderExtViewerQtOSG (Info, local);
}

};
