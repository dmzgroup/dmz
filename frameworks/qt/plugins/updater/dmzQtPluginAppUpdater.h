#ifndef DMZ_QT_PLUGIN_APP_UPDATER_DOT_H
#define DMZ_QT_PLUGIN_APP_UPDATER_DOT_H

#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimeVersion.h>
#include <QtGui/QDialog>
#include "ui_UpdateDialog.h"

class QNetworkAccessManager;
class QNetworkReply;


namespace dmz {
   
   class QtModuleMainWindow;

   class QtPluginAppUpdater :
         public QDialog,
         public Plugin {

   Q_OBJECT
   
      public:
         QtPluginAppUpdater (const PluginInfo &Info, Config &local, Config &global);
         ~QtPluginAppUpdater ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level);

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

      protected Q_SLOTS:
         void _slot_reply_finished (QNetworkReply *);
         void _slot_download_update ();
      
      protected:
         void _init (Config &local);

         Log _log;
         Ui::UpdateDialog _ui;
         Version _version;
         QtModuleMainWindow *_mainWindowModule;
         String _mainWindowModuleName;
         QNetworkAccessManager *_netManager;

      private:
         QtPluginAppUpdater ();
         QtPluginAppUpdater (const QtPluginAppUpdater &);
         QtPluginAppUpdater &operator= (const QtPluginAppUpdater &);

   };
};

#endif // DMZ_QT_PLUGIN_APP_UPDATER_DOT_H
