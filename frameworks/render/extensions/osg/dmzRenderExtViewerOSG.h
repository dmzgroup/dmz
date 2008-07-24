#ifndef DMZ_RENDER_EXT_VIEWER_OSG_DOT_H
#define DMZ_RENDER_EXT_VIEWER_OSG_DOT_H

#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimeTimeSlice.h>

#include <osg/Camera>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>


namespace dmz {

   class InputModule;
   class RenderModuleCoreOSG;
   class RenderCameraManipulatorOSG;
   class RenderEventHandlerOSG;


   class RenderExtViewerOSG : public Plugin, public TimeSlice {

      public:
         RenderExtViewerOSG (const PluginInfo &Info, Config &local);
         ~RenderExtViewerOSG ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level);

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // TimeSlice Interface
         virtual void update_time_slice (const Float64 TimeDelta);

      protected:
         void _init (const Config &Local);

         Log _log;
         RenderModuleCoreOSG *_core;
         InputModule *_channels;
         String _portalName;
         osg::ref_ptr<osg::Camera> _camera;
         osg::ref_ptr<RenderCameraManipulatorOSG> _cameraManipulator;
         osg::ref_ptr<RenderEventHandlerOSG> _eventHandler;
         osg::ref_ptr<osgViewer::Viewer> _viewer;

      private:
         void __init_viewer_window (
               UInt32 windowLeft,
               UInt32 windowTop,
               UInt32 windowWidth,
               UInt32 windowHeight,
               UInt32 screen);

         void __init_viewer_fullscreen (UInt32 screen);
   };
};

#endif // DMZ_RENDER_EXT_VIEWER_OSG_DOT_H
