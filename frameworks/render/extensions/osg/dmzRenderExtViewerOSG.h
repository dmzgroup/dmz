#ifndef DMZ_RENDER_EXT_VIEWER_OSG_DOT_H
#define DMZ_RENDER_EXT_VIEWER_OSG_DOT_H

#include <dmzRuntimeExit.h>
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
         String _title;
         RenderModuleCoreOSG *_core;
         InputModule *_channels;
         String _viewerName;
         osg::ref_ptr<RenderEventHandlerOSG> _eventHandler;
         osg::ref_ptr<osgViewer::Viewer> _viewer;

      private:
         void __init_centered (
            const UInt32 Screen,
            const UInt32 WindowWidth,
            const UInt32 WindowHeight,
            Int32 &windowLeft,
            Int32 &windowTop);

         void __init_viewer_window (
               const Int32 WindowLeft,
               const Int32 WindowTop,
               const UInt32 WindowWidth,
               const UInt32 WindowHeight,
               const UInt32 Screen);

         void __init_viewer_fullscreen (UInt32 screen);
   };
};

#endif // DMZ_RENDER_EXT_VIEWER_OSG_DOT_H
