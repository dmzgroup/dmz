#include "dmzQtPluginAppUpdater.h"
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <QtGui/QMessageBox>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>

namespace {
   
   const dmz::String INTERNAL_BUILD ("INTERNAL BUILD");
   const QLatin1String MBRA_LATEST_URL ("http://shillcock.github.com/mbra/latest.html");
}


dmz::QtPluginAppUpdater::QtPluginAppUpdater (
      const PluginInfo &Info,
      Config &local,
      Config &global) :
      QObject (0),
      Plugin (Info),
      _log (Info),
      _version (global),
      _netManager (0) {

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
         
_log.warn << "_netManager->get (request);" << endl;
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

   }
   else if (Mode == PluginDiscoverRemove) {

   }
}


void
dmz::QtPluginAppUpdater::_slot_reply_finished (QNetworkReply *reply) {
   
   if (reply->error () == QNetworkReply::NoError) {
      
      QString latestVersion (reply->readAll ());
      QString currentVersion (_version.get_build ().get_buffer ());

      if (1 || latestVersion > currentVersion) {

         QMessageBox dialog;

         const String Name (_version.get_name ());
         const String Major (_version.get_major ());
         const String Minor (_version.get_minor ());
         const String Bug (_version.get_bug ());
         const String Build (_version.get_build ());
         const String Release (_version.get_release ());
         const String Image (_version.get_image_name ());
         
         String value;
         value << "A new version of " << Name << "is available.";
         
         dialog.setText (value.get_buffer ());

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

         value << " is now available.\n";
         value << "Would you like to download it now?";
         
         dialog.setInformativeText (value.get_buffer ());

         int result = dialog.exec ();
         
         switch (result) {
            
            case QMessageBox::Yes:
               break;
            
            case QMessageBox::No:
               break;
            
            default: break;
         }
      }
   }   
}


void
dmz::QtPluginAppUpdater::_init (Config &local) {

   if (1 || _version.get_build () != INTERNAL_BUILD) {
      
      _netManager = new QNetworkAccessManager (this);

      connect (
         _netManager, SIGNAL (finished (QNetworkReply *)),
         this, SLOT (_slot_reply_finished (QNetworkReply *)));
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
