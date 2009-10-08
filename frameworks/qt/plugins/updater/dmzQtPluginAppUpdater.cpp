#include <dmzQtModuleMainWindow.h>
#include "dmzQtPluginAppUpdater.h"
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <QtGui/QMainWindow>
//#include <QtGui/QMessageBox>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>

namespace {
   
   const dmz::String INTERNAL_BUILD ("INTERNAL BUILD");
   const QLatin1String MBRA_LATEST_URL ("http://dmzdev.github.com/latest/mbra.txt");
   
// http://dmzdev.github.com/latest/mbra.xml
// http://dmzdev.github.com/latest/mbra.txt
// http://cloud.github.com/downloads/dmzdev/latest/MBRA-091008182940.dmg
// http://bit.ly/4mdLYP
}


dmz::QtPluginAppUpdater::QtPluginAppUpdater (
      const PluginInfo &Info,
      Config &local,
      Config &global) :
      QDialog (0),
      Plugin (Info),
      _log (Info),
      _version (global),
      _netManager (0) {

   _ui.setupUi (this);

   _init (local);
}


dmz::QtPluginAppUpdater::~QtPluginAppUpdater () {

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
         
         QNetworkRequest request;
         request.setUrl (QUrl (MBRA_LATEST_URL));
         
         _netManager->get (request);
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

         _mainWindowModule = 0;
      }
   }
}


void
dmz::QtPluginAppUpdater::_slot_reply_finished (QNetworkReply *reply) {
   
   if (reply->error () == QNetworkReply::NoError) {
      
      QString latestVersion (reply->readAll ());
      QString currentVersion (_version.get_build ().get_buffer ());

      if (1) { //latestVersion > currentVersion) {

         if (_mainWindowModule) {
            
            setParent (_mainWindowModule->get_qt_main_window ());
         }

         open ();

//         QMessageBox dialog (_mainWindowModule ? _mainWindowModule->get_qt_main_window () : 0);

         const String Name (_version.get_name ());
         const String Major (_version.get_major ());
         const String Minor (_version.get_minor ());
         const String Bug (_version.get_bug ());
         const String Build (_version.get_build ());
         const String Release (_version.get_release ());
         const String Image (_version.get_image_name ());
         
         if (Image) {

            QPixmap pix (Image.get_buffer ());

            if (!pix.isNull ()) { _ui.iconLabel->setPixmap (pix); }
            else { _ui.iconLabel->hide (); }
         }
         
         String value;
         value << "A new version of " << Name << " (" << qPrintable (latestVersion) << ") is available.";
  
         _ui.textLabel->setText (value.get_buffer ());
//         dialog.setText (value.get_buffer ());

         value.flush () << Name;
         
         if (Major) {

            value << " " << Major;
            
            if (Minor) {

               value << "." << Minor;

               if (Bug) { value << "." << Bug; }
            }

            if (Release) { value << " " << Release; }

//            value << " (" << qPrintable (latestVersion) << ") is now available.\n";
         }

         value << " is now available.";
         value << "Would you like to download it now?";
         
         _ui.infoLabel->setText (value.get_buffer ());
         
         exec ();

//         dialog.setInformativeText (value.get_buffer ());
         
//         dialog.setStandardButtons (QMessageBox::Yes | QMessageBox::No);
//         dialog.setDefaultButton (QMessageBox::No);

         //int result =
//         dialog.open (this, SLOT (_slot_download_update ()));
         
         // switch (result) {
         //    
         //    case QMessageBox::Yes:
         //       break;
         //    
         //    case QMessageBox::No:
         //       break;
         //    
         //    default: break;
         // }
      }
   }   
}


void
dmz::QtPluginAppUpdater::_slot_download_update () {
   
}

void
dmz::QtPluginAppUpdater::_init (Config &local) {

   setObjectName (get_plugin_name ().get_buffer ());

//   if (_version.get_build () != INTERNAL_BUILD) {
      
      _netManager = new QNetworkAccessManager (this);

      connect (
         _netManager, SIGNAL (finished (QNetworkReply *)),
         this, SLOT (_slot_reply_finished (QNetworkReply *)));
//   }
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
