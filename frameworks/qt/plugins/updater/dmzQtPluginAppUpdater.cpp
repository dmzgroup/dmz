#include <dmzQtModuleMainWindow.h>
#include "dmzQtPluginAppUpdater.h"
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzSystem.h>
#include <dmzXMLUtil.h>
#include <QtCore/QtCore>
#include <QtGui/QtGui>
#include <QtNetwork/QtNetwork>
#include <QtXml/QtXml>

#include <QtCore/QDebug>


namespace {
   
   const dmz::String INTERNAL_BUILD ("INTERNAL BUILD");
   
   // http://dmzdev.github.com/latest/{system_name}/{app_name}.xml
   const dmz::String VERSION_SERVER ("http://dmzdev.github.com/latest/");
   const QString VERSION_URL ("http://dmzdev.github.com/latest/%1/%2.xml");
   
   // http://cloud.github.com/downloads/dmzdev/latest/{app_name}-{build_number}.dmg
   const QString DOWNLOAD_SERVER ("http://cloud.github.com/downloads/dmzdev/latest/");
   const QString DOWNLOAD_URL ("http://cloud.github.com/downloads/dmzdev/latest/%1-%2.%3");
}


dmz::QtPluginAppUpdater::QtPluginAppUpdater (
      const PluginInfo &Info,
      Config &local,
      Config &global) :
      QObject (0),
      Plugin (Info),
      _log (Info),
      _version (global),
      _updateVersion (),
      _mainWindowModule (0),
      _mainWindowModuleName (),
      _netManager (0),
      _updateDialog (0),
      _downloadReply (0) {

   _init (local);
}


dmz::QtPluginAppUpdater::~QtPluginAppUpdater () {

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

      if (_netManager) {
         
         String versionServer (VERSION_SERVER);
         versionServer << get_system_name ();
         versionServer << "/" << _version.get_name ().to_lower () << ".xml";
         _log.info << "get: " << versionServer << endl;
         
         QNetworkRequest request (QUrl::fromEncoded (versionServer.get_buffer ()));
         QNetworkReply *reply (_netManager->get (request));
         
          if (reply) {
             
            connect (
               reply, SIGNAL (finished ()),
               this, SLOT (_slot_get_version_finished ()));
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
         
         if (_mainWindowModule && !_updateDialog) {
            
            _updateDialog = new QDialog (
               _mainWindowModule ? _mainWindowModule->get_qt_main_window () : 0);
               
            _ui.setupUi (_updateDialog);
            
            connect (
               _ui.buttonBox, SIGNAL (accepted ()),
               this, SLOT (_slot_download_start ()));

            connect (
               _ui.buttonBox, SIGNAL (rejected ()),
               this, SLOT (_slot_download_cancel ()));
         }
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
dmz::QtPluginAppUpdater::_slot_get_version_read_ready () {
   
   
}


void
dmz::QtPluginAppUpdater::_slot_get_version_finished () {
   
   QNetworkReply *reply (qobject_cast<QNetworkReply *>(sender ()));
   
   if (reply) {
      
      if (_updateDialog && (reply->error () == QNetworkReply::NoError)) {

         QString data (reply->readAll ());

         const String XMLData (qPrintable (data));

         Config cd ("global");

         if (xml_string_to_config (XMLData, cd)) {

            _updateVersion = Version (cd);

            QString latestBuild (_updateVersion.get_build ().get_buffer ());
            QString currentBuild (_version.get_build ().get_buffer ());

            if (_forceUpdate || (latestBuild > currentBuild)) {

               _ui.progressBar->hide ();

               const String Name (_updateVersion.get_name ());
               const String Build (_updateVersion.get_build ());
               const String Image (_updateVersion.get_image_name ());
               const String Version (_updateVersion.get_version ());

               QPixmap pix (Image.get_buffer ());

               if (!pix.isNull ()) {

                  _ui.iconLabel->setPixmap (pix);
                  _ui.iconLabel->show ();
               }
               else { _ui.iconLabel->hide (); }

               String value;
               value << "<b>A new version of " << Name << " is available.</b>";

               _ui.textLabel->setText (value.get_buffer ());

               value.flush () << Name << " " << Version << " is now available";
               value << " (you have " << _version.get_version () << ").\n";
               value << "Would you like to download it now?";

               _ui.infoLabel->setText (value.get_buffer ());

               _updateDialog->open ();
            }
         }
      }
      // else if (reply->error () == QNetworkReply::HostNotFoundError ||
      //          reply->error () == QNetworkReply::ConnectionRefusedError ||
      //          reply->error () == QNetworkReply::ContentNotFoundError) {
      // 
      // }
   }
}


void
dmz::QtPluginAppUpdater::_slot_download_start () {

   if (_updateDialog) {
      
#if defined (Q_WS_WIN)
      const String FileType ("exe");
      const QString DownlaodDr ("Desktop");
#elif defined (Q_WS_MAC)
      const String FileType ("dmg");
      const QString DownloadDir ("Downloads");
#else
      const String FileType ("zip");
      const QString DownloadDir ("Desktop");
#endif

      const String Name (_updateVersion.get_name ());
      const String Build (_updateVersion.get_build ());
      
      const String Target (Name + "-" + Build + "." + FileType);
      
      QString defaultFileName (tr ("%1/%2/%3").
         arg (QDir::homePath ()).
         arg (DownloadDir).
         arg (Target.get_buffer ()));
         
      defaultFileName.replace (QRegExp (" "), "_");

      QString fileName = QFileDialog::getSaveFileName (
         _mainWindowModule ? _mainWindowModule->get_qt_main_window () : 0,
         "Save File",
         defaultFileName);
   
      if (!fileName.isEmpty ()) {
         
         QString downloadServer (DOWNLOAD_SERVER + QLatin1String (Target.get_buffer ()));
            
         downloadServer.replace (QRegExp (" "), "_");
         
         QUrl url (downloadServer);
         
         _log.info << "downloading: " << qPrintable (url.toString ()) << endl;

         QNetworkRequest request (url);
         request.setAttribute (QNetworkRequest::User, fileName);
         
         _downloadReply = _netManager->get (request);
         
          if (_downloadReply) {
             
            connect (
               _downloadReply, SIGNAL (downloadProgress (qint64, qint64)),
               this, SLOT (_slot_download_progress (qint64, qint64)));

            connect (
               _downloadReply, SIGNAL (finished ()),
               this, SLOT (_slot_download_finished ()));
          }
         
         _ui.textLabel->setText ("<b>Downloading update...</b>");
         _ui.infoLabel->setText (QString::null);
         _ui.progressBar->show ();
         _ui.buttonBox->setStandardButtons (QDialogButtonBox::Cancel);
      }
   }
}


void
dmz::QtPluginAppUpdater::_slot_download_cancel () {

   if (_downloadReply) {
      
      _downloadReply->abort ();
      delete _downloadReply;
      _downloadReply = 0;
   }
   
   if (_updateDialog) {
      
      _updateDialog->reject ();
   }
}


void
dmz::QtPluginAppUpdater::_slot_download_finished () {
   
   if (_downloadReply && _updateDialog) {
      
      if (_downloadReply->error () == QNetworkReply::NoError) {
   
         QNetworkRequest request (_downloadReply->request ());
         QVariant v (request.attribute (QNetworkRequest::User));
         QString fileName = v.toString ();
         QFile file (fileName);
      
         if (file.open (QFile::ReadWrite)) {
         
            file.write (_downloadReply->readAll ());
         }
      
         _updateDialog->accept ();
      }
      else {
      
         QMessageBox::warning (
            _updateDialog->parentWidget (),
            "Update Failed",
            _downloadReply->errorString ());
      
         _slot_download_cancel ();
      }
   }
}


void
dmz::QtPluginAppUpdater::_slot_download_progress (qint64 received, qint64 total) {

   if (_updateDialog) {
      
      Float64 receivedSizeInMb = Float64 (received) / 1048576;
      Float64 totalSizeInMb = Float64 (total) / 1048576;
      
      QString info (tr ("%1 MB of %2 MB").
         arg (receivedSizeInMb, 0, 'f', 2).
         arg (totalSizeInMb, 0, 'f', 2));
         
      _ui.infoLabel->setText (info);
      _ui.progressBar->setMaximum (total);
      _ui.progressBar->setValue (received);
   }
}


void
dmz::QtPluginAppUpdater::_init (Config &local) {

   setObjectName (get_plugin_name ().get_buffer ());
   
   _mainWindowModuleName = config_to_string ("module.mainWindow.name", local);

   _forceUpdate = config_to_boolean ("force-update.value", local, False);
   
   if (_forceUpdate || (_version.get_build () != INTERNAL_BUILD)) {
      
      _netManager = new QNetworkAccessManager (this);
   }
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
