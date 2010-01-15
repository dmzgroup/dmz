#ifndef DMZ_RENDER_PLUGIN_SCREEN_CAPTURE_OSG_DOT_H
#define DMZ_RENDER_PLUGIN_SCREEN_CAPTURE_OSG_DOT_H

#include <dmzRuntimeDataConverters.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimeMessaging.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimeTimeSlice.h>

#include <osgViewer/ViewerEventHandlers>

namespace dmz {

   class RenderModuleCoreOSG;

   class RenderPluginScreenCaptureOSG :
         public Plugin,
         public TimeSlice,
         public MessageObserver {

      public:
         RenderPluginScreenCaptureOSG (const PluginInfo &Info, Config &local);
         ~RenderPluginScreenCaptureOSG ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level);

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // TimeSlice Interface
         virtual void update_time_slice (const Float64 TimeDelta);

         // Message Observer Interface
         virtual void receive_message (
            const Message &Type,
            const UInt32 MessageSendHandle,
            const Handle TargetObserverHandle,
            const Data *InData,
            Data *outData);

      protected:
         void _init (Config &local);

         Log _log;
         DataConverterString _convert;

         Message _screenCaptureMsg;

         RenderModuleCoreOSG *_core;
         String _fileName;
         osg::ref_ptr<osgViewer::ScreenCaptureHandler> _handler;

      private:
         RenderPluginScreenCaptureOSG ();
         RenderPluginScreenCaptureOSG (const RenderPluginScreenCaptureOSG &);
         RenderPluginScreenCaptureOSG &operator= (const RenderPluginScreenCaptureOSG &);

   };
};

#endif // DMZ_RENDER_PLUGIN_SCREEN_CAPTURE_OSG_DOT_H
