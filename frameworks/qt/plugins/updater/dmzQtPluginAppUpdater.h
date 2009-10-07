#ifndef DMZ_QT_PLUGIN_APP_UPDATER_DOT_H
#define DMZ_QT_PLUGIN_APP_UPDATER_DOT_H

#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimeVersion.h>
#include <QtCore/QObject>

class QNetworkAccessManager;
class QNetworkReply;


namespace dmz {

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
         void _slot_reply_finished (QNetworkReply *);
      
      protected:
         void _init (Config &local);

         Log _log;
         Version _version;
         QNetworkAccessManager *_netManager;

      private:
         QtPluginAppUpdater ();
         QtPluginAppUpdater (const QtPluginAppUpdater &);
         QtPluginAppUpdater &operator= (const QtPluginAppUpdater &);

   };
};

#endif // DMZ_QT_PLUGIN_APP_UPDATER_DOT_H
