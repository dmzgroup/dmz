#ifndef DMZ_QT_PLUGIN_APP_UPDATER_DOT_H
#define DMZ_QT_PLUGIN_APP_UPDATER_DOT_H

#include <dmzRuntimeExit.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimeVersion.h>
#include <QtCore/QTemporaryFile>
#include <QtCore/QObject>
#include "ui_UpdateDialog.h"

class QDialog;
class QNetworkAccessManager;
class QNetworkReply;


namespace dmz {

   class QtModuleMainWindow;

   class QtPluginAppUpdater :
         public QObject,
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
         void _slot_get_version_error ();
         void _slot_get_version_finished ();
         void _slot_get_changelog_finished ();
         void _slot_download_start ();
         void _slot_download_cancel ();
         void _slot_download_ready_read ();
         void _slot_download_finished ();
         void _slot_download_progress (qint64 received, qint64 total);
         void _slot_handle_downloaded_file ();

      protected:
         void _get_changelog ();
         void _init (Config &local);

         Log _log;
         Exit _exit;
         Version _version;
         Version _updateVersion;
         Ui::UpdateDialog _ui;
         QtModuleMainWindow *_mainWindowModule;
         String _mainWindowModuleName;
         QNetworkAccessManager *_netManager;
         QNetworkReply *_downloadReply;
         QDialog *_updateDialog;
         String _releaseChannel;
         String _versionUrl;
         String _downloadUrl;
         QFile _downloadFile;
         Boolean _downloadToTemp;
         Boolean _forceUpdate;

      private:
         QtPluginAppUpdater ();
         QtPluginAppUpdater (const QtPluginAppUpdater &);
         QtPluginAppUpdater &operator= (const QtPluginAppUpdater &);

   };
};

#endif // DMZ_QT_PLUGIN_APP_UPDATER_DOT_H
