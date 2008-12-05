#ifndef DMZ_QT_PLUGIN_CANVAS_BACKGROUND_DOT_H
#define DMZ_QT_PLUGIN_CANVAS_BACKGROUND_DOT_H

#include <dmzApplicationState.h>
#include <dmzArchiveObserverUtil.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimeMessaging.h>
#include <dmzRuntimePlugin.h>

class QGraphicsPixmapItem;
class QPixmap;


namespace dmz {

   class QtCanvasModule;
   class QtModuleMainWindow;

   class QtPluginCanvasBackground :
         public Plugin,
         public ArchiveObserverUtil,
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

         // ArchiveObserver Interface.
         virtual void create_archive (
            const Handle ArchiveHandle,
            Config &local,
            Config &global);

         virtual void process_archive (
            const Handle ArchiveHandle,
            Config &local,
            Config &global);

         // Message Observer Interface
         virtual void receive_message (
            const Message &Type,
            const UInt32 MessageSendHandle,
            const Handle TargetObserverHandle,
            const Data *InData,
            Data *outData);

      protected:
         QString _get_last_path ();
         void _load_background ();
         void _load_pixmap (const QPixmap &Pixmap);
         void _clear_background ();
         void _init (Config &local);

         Log _log;
         ApplicationStateWrapper _appState;
         QtModuleMainWindow *_mainWindowModule;
         String _mainWindowModuleName;
         QtModuleCanvas *_canvasModule;
         String _canvasModuleName;
         Message _backgroundEditMessage;
         QGraphicsPixmapItem *_bgItem;
         Config _bgConfig;

      private:
         QtPluginCanvasBackground ();
         QtPluginCanvasBackground (const QtPluginCanvasBackground &);
         QtPluginCanvasBackground &operator= (const QtPluginCanvasBackground &);
   };
};

#endif // DMZ_QT_PLUGIN_CANVAS_BACKGROUND_DOT_H
