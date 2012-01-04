#ifndef DMZ_QT_PLUGIN_CANVAS_ZOOM_MESSAGE_DOT_H
#define DMZ_QT_PLUGIN_CANVAS_ZOOM_MESSAGE_DOT_H

#include <dmzRuntimeDataConverterTypesBase.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimeMessaging.h>
#include <dmzRuntimePlugin.h>

#include <QtCore/QList>
#include <QtCore/QPointF>

namespace dmz {

   class QtModuleCanvas;

   class QtPluginCanvasZoomMessage :
         public Plugin,
         public MessageObserver {

      public:
         QtPluginCanvasZoomMessage (const PluginInfo &Info, Config &local);
         ~QtPluginCanvasZoomMessage ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level);

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // Message Observer Interface
         virtual void receive_message (
            const Message &Type,
            const UInt32 MessageSendHandle,
            const Handle TargetObserverHandle,
            const Data *InData,
            Data *outData);

      protected:
         // QtPluginCanvasZoomMessage Interface
         void _init (Config &local);
         void _set_zoom ();

         Log _log;
         QtModuleCanvas *_canvasModule;
         String _canvasModuleName;
         QPointF _location;
         Message _zoomMessage;
         Message _zoomInMessage;
         Message _zoomOutMessage;
         Handle _positionAttr;

         DataConverterFloat64 _convertNum;
         QList<Float32> _zoomList;
         UInt32 _zoomIndex;

      private:
         QtPluginCanvasZoomMessage ();
         QtPluginCanvasZoomMessage (const QtPluginCanvasZoomMessage &);
         QtPluginCanvasZoomMessage &operator= (const QtPluginCanvasZoomMessage &);

   };
};

#endif // DMZ_QT_PLUGIN_CANVAS_ZOOM_MESSAGE_DOT_H
