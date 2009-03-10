#ifndef DMZ_QT_PLUGIN_VIEWER_OSG_DOT_H
#define DMZ_QT_PLUGIN_VIEWER_OSG_DOT_H

#include <dmzQtWidget.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimeTimeSlice.h>

#include <osg/Camera>
#include "ViewerQOSG.h"


namespace dmz {

   class InputModule;
   class RenderModuleCoreOSG;
   class RenderCameraManipulatorOSG;
   class RenderEventHandlerOSG;

   class QtPluginViewerOSG : public Plugin, public TimeSlice, public QtWidget {

      public:
         QtPluginViewerOSG (const PluginInfo &Info, Config &local);
         ~QtPluginViewerOSG ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level);

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // TimeSlice Interface
         virtual void update_time_slice (const Float64 TimeDelta);

         // QtPlugin Interface
         virtual QWidget *get_qt_widget ();

      protected:
         void _init (const Config &Local);

         Log _log;
         RenderModuleCoreOSG *_core;
         InputModule *_channels;
         String _viewerName;
         osg::ref_ptr<RenderEventHandlerOSG> _eventHandler;
         osg::ref_ptr<ViewerQOSG> _viewer;

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

#endif // DMZ_QT_PLUGIN_VIEWER_OSG_DOT_H
