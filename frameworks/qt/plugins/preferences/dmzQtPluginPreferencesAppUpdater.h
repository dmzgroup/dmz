#ifndef DMZ_QT_PLUGIN_PREFERENCES_APP_UPDATER_DOT_H
#define DMZ_QT_PLUGIN_PREFERENCES_APP_UPDATER_DOT_H

#include <dmzQtWidget.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimeMessaging.h>
#include <dmzRuntimePlugin.h>
#include <QtCore/QMap>
#include <QtGui/QFrame>
#include "ui_dmzQtPluginPreferencesAppUpdater.h"


namespace dmz {

   class QtPluginPreferencesAppUpdater :
         public QFrame,
         public QtWidget,
         public Plugin,
         public MessageObserver {
         
      Q_OBJECT

      public:
         QtPluginPreferencesAppUpdater (
            const PluginInfo &Info,
            Config &local,
            Config &global);
            
         ~QtPluginPreferencesAppUpdater ();

         // QtWidget Interface
         virtual QWidget *get_qt_widget ();

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

      protected Q_SLOTS:
         void on_updateCheckBox_stateChanged (int);
         void on_channelComboBox_currentIndexChanged (const QString &);

      protected:
         void _init (Config &local, Config &global);

         Log _log;
         Ui::preferencesForm _ui;
         Handle _valueAttrHandle;
         Message _updateMessage;
         Message _channelMessage;

      private:
         QtPluginPreferencesAppUpdater ();
         QtPluginPreferencesAppUpdater (const QtPluginPreferencesAppUpdater &);
         QtPluginPreferencesAppUpdater &operator= (const QtPluginPreferencesAppUpdater &);
   };
};

#endif // DMZ_QT_PLUGIN_PREFERENCES_APP_UPDATER_DOT_H
