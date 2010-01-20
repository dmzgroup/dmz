#include <dmzFoundationXMLUtil.h>
#include <dmzQtModuleMainWindow.h>
#include "dmzQtPluginAppUpdater.h"
#include <dmzQtUtil.h>
#include <dmzRuntimeConfigToNamedHandle.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeData.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimeMessaging.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzSystem.h>
#include <QtCore/QtCore>
#include <QtGui/QtGui>
#include <QtNetwork/QtNetwork>
#include <QtXml/QtXml>

namespace {

   const dmz::String INTERNAL_BUILD ("INTERNAL BUILD");
}


dmz::QtPluginAppUpdater::QtPluginAppUpdater (
      const PluginInfo &Info,
      Config &local,
      Config &global) :
      QObject (0),
      Plugin (Info),
      _log (Info),
      _exit (get_plugin_runtime_context ()),
      _version (global),
      _updateVersion (),
      _mainWindowModule (0),
      _mainWindowModuleName (),
      _netManager (0),
      _updateDialog (0),
      _releaseChannel ("stable"),
      _updateUrl (),
      _downloadUrl (),
      _downloadFile (),
      _downloadReply (0),
      _downloadToTemp (True),
      _forceUpdate (False),
      _updateFlag (True),
      _autoFinishCount (15),
      _updateMessageName ("AppUpdaterUpdateMessage"),
      _channelMessageName ("AppUpdaterChannelMessage") {

   _init (local);
}


dmz::QtPluginAppUpdater::~QtPluginAppUpdater () {

   if (_netManager) {
      
      delete _netManager;
      _netManager = 0;
   }
   
   if (_updateDialog) {

      _updateDialog->setParent (0);
      delete _updateDialog;
      _updateDialog = 0;
   }
}


// Plugin Interface
void
dmz::QtPluginAppUpdater::update_plugin_state (
      const PluginStateEnum State,
      const UInt32 Level) {

   if (State == PluginStateInit) {

   }
   else if (State == PluginStateStart) {
      
      RuntimeContext *context (get_plugin_runtime_context ());
      Definitions defs (context, &_log);

      Message message;

      if (defs.lookup_message (_updateMessageName, message)) {

         const Data *data (message.get_monostate ());
         if (data) { data->lookup_boolean (_valueAttrHandle, 0, _updateFlag); }
      }
      
      if (defs.lookup_message (_channelMessageName, message)) {

         const Data *data (message.get_monostate ());
         if (data) { data->lookup_string (_valueAttrHandle, 0, _releaseChannel); }
      }

      if (_forceUpdate || _updateFlag) {
         
         if (!_netManager) { _netManager = new QNetworkAccessManager (this); }
         
         if (_netManager) {

            if (_mainWindowModule && !_updateDialog) {

               _updateDialog = new QDialog (
                  _mainWindowModule ? _mainWindowModule->get_qt_main_window () : 0);

               _ui.setupUi (_updateDialog);

              connect (
                  _ui.downloadButton, SIGNAL (clicked ()),
                  this, SLOT (_slot_download_start ()));

               connect (
                  _ui.laterButton, SIGNAL (clicked ()),
                  this, SLOT (_slot_download_cancel ()));

               connect (
                  _ui.finishedButton, SIGNAL (clicked ()),
                  this, SLOT (_slot_handle_downloaded_file ()));
            }

            _check_for_update ();
         }
      }
   }
   else if (State == PluginStateStop) {

   }
   else if (State == PluginStateShutdown) {

   }
}


void
dmz::QtPluginAppUpdater::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      if (!_mainWindowModule) {

         _mainWindowModule = QtModuleMainWindow::cast (PluginPtr, _mainWindowModuleName);
      }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_mainWindowModule &&
            (_mainWindowModule == QtModuleMainWindow::cast (PluginPtr))) {

         if (_updateDialog) { _updateDialog->setParent (0); }
         _mainWindowModule = 0;
      }
   }
}


void
dmz::QtPluginAppUpdater::_slot_get_version_finished () {

   QNetworkReply *reply (qobject_cast<QNetworkReply *>(sender ()));

   if (reply) {

      if (reply->error () == QNetworkReply::NoError) {

         if (_updateDialog) {

            QString data (reply->readAll ());

            const String XMLData (qPrintable (data));

            Config cd ("global");

            if (xml_string_to_config (XMLData, cd)) {

               _updateVersion = Version (cd);

               if (!_forceUpdate) {

                  _forceUpdate = config_to_boolean ("dmz.update.force.value", cd);
               }

               if (_forceUpdate || (_updateVersion > _version)) {

                  _get_changelog ();

                  const QString Name (_updateVersion.get_name ().get_buffer ());
                  const QString Build (_updateVersion.get_build ().get_buffer ());
                  const QString Version (_updateVersion.get_version ().get_buffer ());
                  const QString CVersion (_version.get_version ().get_buffer ());
                  const QString CBuild (_version.get_build ().get_buffer ());

                  QPixmap pix (_updateVersion.get_image_name ().get_buffer());
                  if (!pix.isNull ()) { _ui.iconLabel->setPixmap (pix); }

                  QString message = _ui.textLabel->text ();
                  message.replace ("{app_name}", Name);
                  message.replace ("{version}", Version);
                  message.replace ("{build_number}", Build);
                  message.replace ("{cversion}", CVersion);
                  message.replace ("{cbuild_number}", CBuild);
                  _ui.textLabel->setText (message);

                  _ui.stackedWidget->setCurrentWidget (_ui.startPage);
                  _updateDialog->open ();
               }
            }
         }
      }
   }
}


void
dmz::QtPluginAppUpdater::_slot_download_start () {

   if (_updateDialog) {

#if defined (Q_WS_WIN)
      const QString FileType ("exe");
#elif defined (Q_WS_MAC)
      const QString FileType ("dmg");
#else
      const QString FileType ("zip");
#endif

      QString appName (_updateVersion.get_name ().get_buffer ());
      appName.replace (" ", "-");

      const String Build (_updateVersion.get_build ());

      QString fileName;

      if (_downloadToTemp) {

         fileName = tr ("%1/%2-%3.%4").
            arg (QDir::tempPath ()).
            arg (appName).
            arg (Build.get_buffer ()).
            arg (FileType);
      }
      else {

         QString defaultFileName = tr ("%1/%2-%3.%4").
            arg (QDesktopServices::storageLocation (QDesktopServices::DesktopLocation)).
            arg (appName).
            arg (Build.get_buffer ()).
            arg (FileType);

#if defined (Q_WS_MAC)
         QString tempPath (defaultFileName);

         tempPath.replace (QRegExp (
            QDesktopServices::displayName (QDesktopServices::DesktopLocation)),
            "Downloads");

         QFileInfo fi (tempPath);

         if (QFile::exists (fi.absolutePath ())) { defaultFileName = tempPath; }
#endif

         fileName = get_save_file_name_with_extension (
            _mainWindowModule ? _mainWindowModule->get_qt_main_window () : 0,
            tr ("Save File"),
            QDir::toNativeSeparators (defaultFileName),
            tr ("*.%1").arg (FileType),
            FileType);
      }

      if (!fileName.isEmpty ()) {

         QString downloadServer (_downloadUrl.get_buffer ());
         downloadServer.replace ("{app_name}", appName);
         downloadServer.replace ("{build_number}", Build.get_buffer ());
         downloadServer.append (tr (".") + FileType);

         QUrl url (downloadServer);

         _log.info << "Downloading: " << qPrintable (url.toString ()) << endl;

         QNetworkRequest request (url);

         _downloadReply = _netManager->get (request);

          if (_downloadReply) {

            connect (
               _downloadReply, SIGNAL (downloadProgress (qint64, qint64)),
               this, SLOT (_slot_download_progress (qint64, qint64)));

            connect (
               _downloadReply, SIGNAL (readyRead ()),
               this, SLOT (_slot_download_ready_read ()));

            connect (
               _downloadReply, SIGNAL (finished ()),
               this, SLOT (_slot_download_finished ()));

            _downloadFile.setFileName (fileName);
         }

         if (!_downloadFile.open (QIODevice::WriteOnly)) {

            _log.warn << "Failed to open file for download. Download aborted!" << endl;

            if (_downloadReply) { _downloadReply->abort (); }

            QMessageBox::warning (
               _updateDialog->parentWidget (),
               "Update Failed",
               "Failed to open temp file for download.\nDownload aborted!");

            _updateDialog->reject ();
         }
         else {

            _ui.stackedWidget->setCurrentWidget (_ui.downloadPage);
         }
      }
   }
}


void
dmz::QtPluginAppUpdater::_slot_download_cancel () {

   if (_downloadReply) { _downloadReply->abort (); }
   if (_updateDialog) { _updateDialog->reject (); }
}


void
dmz::QtPluginAppUpdater::_slot_download_ready_read () {

   if (_downloadReply) {

      if (_downloadFile.error () == QFile::NoError) {

         _downloadFile.write (_downloadReply->readAll ());
      }
   }
}


void
dmz::QtPluginAppUpdater::_slot_download_finished () {

   if (_downloadReply) {

      _downloadFile.close ();

      switch (_downloadReply->error ()) {

         case QNetworkReply::NoError: {

            _log.info << "Download saved as " << qPrintable (_downloadFile.fileName ()) << endl;
            _ui.stackedWidget->setCurrentWidget (_ui.finishPage);
            startTimer (1000);
            break;
         }

         case QNetworkReply::OperationCanceledError:

            _log.info << "Download operation canceled." << endl;
            if (_updateDialog) { _updateDialog->reject (); }
            _downloadFile.remove ();
            break;

         default:

            _log.warn << qPrintable (_downloadReply->errorString ()) << endl;

            QMessageBox::warning (
               _updateDialog->parentWidget (),
               "Update Failed",
               _downloadReply->errorString ());

            if (_updateDialog) { _updateDialog->reject (); }
            _downloadFile.remove ();
            break;
      }

      delete _downloadReply;
      _downloadReply = 0;
   }
   else if (_updateDialog) { _updateDialog->reject (); }
}


void
dmz::QtPluginAppUpdater::_slot_download_progress (qint64 received, qint64 total) {

   if (_updateDialog) {

      Float64 receivedSizeInMb = Float64 (received) / 1048576;
      Float64 totalSizeInMb = Float64 (total) / 1048576;

      QString info (tr ("Downloading %1 MB of %2 MB").
         arg (receivedSizeInMb, 0, 'f', 2).
         arg (totalSizeInMb, 0, 'f', 2));

      _ui.progressLabel->setText (info);
      _ui.progressBar->setMaximum (total);
      _ui.progressBar->setValue (received);
   }
}


void
dmz::QtPluginAppUpdater::_slot_handle_downloaded_file () {

   const QString FileName (_downloadFile.fileName ());

#if defined (Q_WS_WIN)
   QString command (tr ("\"%1\"").arg (FileName));
#elif defined (Q_WS_MAC)
   QString command (tr ("hdiutil attach \"%1\" -autoopen -quiet").arg (FileName));
#else
   QString command (tr ("unzip \"%1\"").arg (FileName));
#endif

   if (!FileName.isEmpty ()) {

      _log.info << "Running: " << qPrintable (command) << endl;
      if (QProcess::startDetached (command)) {

         _exit.request_exit (dmz::ExitStatusForced, get_plugin_name () + " Closed");
      }
      else {

         QString msg (tr ("Update has been downloaded to: %1").arg (FileName));
         QMessageBox::information (_updateDialog->parentWidget (), "Update Downloaded", msg);
      }
   }
}

void
dmz::QtPluginAppUpdater::_slot_get_changelog_finished () {

   QNetworkReply *reply (qobject_cast<QNetworkReply *>(sender ()));

   if (reply) {

      if (reply->error () == QNetworkReply::NoError) {

         if (_updateDialog) {

            QString data (reply->readAll ());
            _ui.releaseNotesTextEdit->setText (data);
         }
      }
   }
}


void
dmz::QtPluginAppUpdater::timerEvent (QTimerEvent *event) {

   if (_updateDialog) {

      _autoFinishCount--;

      if (_autoFinishCount <= 0) {

         _ui.finishedButton->animateClick ();
      }
      else if (_autoFinishCount < 10) {

         QString buttonText (_ui.finishedButton->text ());
         if (buttonText.contains ("(")) { buttonText.chop (4); }
         buttonText.append (tr (" (%2)").arg (_autoFinishCount));
         _ui.finishedButton->setText (buttonText);
      }
   }
}


void
dmz::QtPluginAppUpdater::_check_for_update () {

   if (_netManager) {

      QString appName (_version.get_name ().get_buffer ());
      appName.replace (QRegExp (" "), "-");

      QString versionServer = QString (_updateUrl.get_buffer ());
      versionServer.replace ("{release_channel}", _releaseChannel.get_buffer ());
      versionServer.replace ("{system_name}", get_system_name ().get_buffer ());
      versionServer.replace ("{app_name}", appName);
      versionServer.append (tr (".xml"));

      QUrl url (versionServer);

      _log.info << "Get: " << qPrintable (url.toString ()) << endl;

      QNetworkRequest request (url);
      QNetworkReply *reply (_netManager->get (request));

      if (reply) {

         connect (
               reply, SIGNAL (finished ()),
               this, SLOT (_slot_get_version_finished ()));
      }
   }
}


void
dmz::QtPluginAppUpdater::_get_changelog () {

   if (_netManager) {

      QString appName (_updateVersion.get_name ().get_buffer ());
      appName.replace (" ", "-");

      const QString Build (_updateVersion.get_build ().get_buffer ());

      QString changelogServer (_downloadUrl.get_buffer ());
      changelogServer.replace ("{app_name}", appName);
      changelogServer.replace ("{build_number}", Build);
      changelogServer.append (tr (".html"));

      QUrl url (changelogServer);

      _log.info << "Get: " << qPrintable (url.toString ()) << endl;

      QNetworkRequest request (url);

      QNetworkReply *reply = _netManager->get (request);
      if (reply) {

        connect (
           reply, SIGNAL (finished ()),
           this, SLOT (_slot_get_changelog_finished ()));
      }
   }
}


void
dmz::QtPluginAppUpdater::_init (Config &local) {

   setObjectName (get_plugin_name ().get_buffer ());
   RuntimeContext *context (get_plugin_runtime_context ());

   _mainWindowModuleName = config_to_string ("module.mainWindow.name", local);
   _forceUpdate = config_to_boolean ("force-update.value", local, _forceUpdate);
   _downloadToTemp = config_to_boolean ("download-to-temp.value", local, _downloadToTemp);
   _releaseChannel = config_to_string ("release.channel", local, _releaseChannel);

   // _updateUrl = "latest/{system_name}-{release_channel}/{app_name}.xml";
   // _downloadUrl = "downloads/{app_name}-{build_number}";

   String host = config_to_string ("update.host", local);
   String path = config_to_string ("update.path", local);

   if (host && path) {

      _updateUrl = host + path;

      host = config_to_string ("download.host", local, host);
      path = config_to_string ("download.path", local);

      if (host && path) { _downloadUrl = host + path; }
   }
   
   if (_updateUrl) { _log.debug << "Update URL: " << _updateUrl << endl; }
   else { _log.debug << "Update URL not specified." << endl; }

   if (_downloadUrl) { _log.debug << "Download URL: " << _downloadUrl << endl; }
   else { _log.debug << "Download URL not specified." << endl; }
   
   _valueAttrHandle = config_to_named_handle (
      "attribute.value.name",
      local,
      "value",
      context);

   _updateMessageName = config_to_string ("update.message", local, _updateMessageName);
   _channelMessageName = config_to_string ("channel.message", local, _channelMessageName);
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzQtPluginAppUpdater (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::QtPluginAppUpdater (Info, local, global);
}

};
