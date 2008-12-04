#ifndef DMZ_QT_PLUGIN_CANVAS_BACKGROUND_DOT_H
#define DMZ_QT_PLUGIN_CANVAS_BACKGROUND_DOT_H

#include <dmzRuntimeLog.h>
#include <dmzRuntimeMessaging.h>
#include <dmzRuntimePlugin.h>

class QGraphicsPixmapItem;


namespace dmz {

   class QtCanvasModule;
   class QtModuleMainWindow;

   class QtPluginCanvasBackground :
         public Plugin,
         public MessageObserver {

      public:
         QtPluginCanvasBackground (const PluginInfo &Info, Config &local);
         ~QtPluginCanvasBackground ();

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
         void _load_background ();
         void _clear_background ();
         void _init (Config &local);

         Log _log;
         QtModuleMainWindow *_mainWindowModule;
         String _mainWindowModuleName;
         QtModuleCanvas *_canvasModule;
         String _canvasModuleName;
         Message _backgroundEditMessage;
         QGraphicsPixmapItem *_bgItem;

      private:
         QtPluginCanvasBackground ();
         QtPluginCanvasBackground (const QtPluginCanvasBackground &);
         QtPluginCanvasBackground &operator= (const QtPluginCanvasBackground &);

   };
};

#endif // DMZ_QT_PLUGIN_CANVAS_BACKGROUND_DOT_H
