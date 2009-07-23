#ifndef DMZ_QT_PLUGIN_MAP_PROPERTIES_DOT_H
#define DMZ_QT_PLUGIN_MAP_PROPERTIES_DOT_H

#include <dmzApplicationState.h>
#include <dmzArchiveObserverUtil.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeDataConverters.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimeMessaging.h>
#include <dmzRuntimePlugin.h>
#include <QtGui/QDialog>
#include "ui_dmzQtPluginMapProperties.h"

namespace qmapcontrol {
   
   class MapControl;
};


namespace dmz {

   class QtModuleMap;
   class QtModuleCanvas;
   class QtModuleMainWindow;
   

   class QtPluginMapProperties :
         public QDialog,
         public Plugin,
         public ArchiveObserverUtil,
         public MessageObserver {
            
   Q_OBJECT

      public:
         QtPluginMapProperties (const PluginInfo &Info, Config &local);
         ~QtPluginMapProperties ();

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

      protected slots:
         void on_mapCheckBox_stateChanged (int state);
         void on_mapAdapterAddButton_clicked ();
         void on_mapAdapterEditButton_clicked ();
         void on_mapAdapterDeleteButton_clicked ();
         void on_emptyCacheButton_clicked ();
         
      protected:
         void _save_session ();
         void _load_session ();
         void _init (Config &local);

         Log _log;
         Ui::mapPropertiesDialog _ui;
         QtModuleMainWindow *_mainWindowModule;
         String _mainWindowModuleName;
         QtModuleCanvas *_canvasModule;
         String _canvasModuleName;
         QtModuleMap *_mapModule;
         String _mapModuleName;
         Message _propertiesEditMessage;
         
      private:
         QtPluginMapProperties ();
         QtPluginMapProperties (const QtPluginMapProperties &);
         QtPluginMapProperties &operator= (const QtPluginMapProperties &);
   };
};

#endif // DMZ_QT_PLUGIN_MAP_PROPERTIES_DOT_H
