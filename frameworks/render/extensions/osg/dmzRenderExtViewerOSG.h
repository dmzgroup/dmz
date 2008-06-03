#ifndef DMZ_RENDER_EXT_VIEWER_OSG_DOT_H
#define DMZ_RENDER_EXT_VIEWER_OSG_DOT_H

#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimeSync.h>

#include <osg/Camera>
#include <osgViewer/Viewer>


namespace dmz {
   
   class InputModule;
   class RenderModuleCoreOSG;
   class RenderCameraManipulatorOSG;
   class RenderEventHandlerOSG;


   class RenderExtViewerOSG : public Plugin, public Sync {

      public:
         RenderExtViewerOSG (const PluginInfo &Info, Config &local);
         ~RenderExtViewerOSG ();

         // Plugin Interface
         virtual void discover_plugin (const Plugin *PluginPtr);
         virtual void start_plugin ();
         virtual void stop_plugin ();
         virtual void shutdown_plugin ();
         virtual void remove_plugin (const Plugin *PluginPtr);

         // Sync Interface
         virtual void update_sync (const Float64 TimeDelta);

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
