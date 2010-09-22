#ifndef DMZ_QT_PLUGIN_TYPE_PALLET_TOOL_DOT_H
#define DMZ_QT_PLUGIN_TYPE_PALLET_TOOL_DOT_H

#include "dmzMimeModel.h"
#include <dmzQtWidget.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimeMessaging.h>
#include <dmzRuntimePlugin.h>

#include <QtGui/QFrame>
#include <QtGui/QSortFilterProxyModel>
#include <ui_TypeFrame.h>

namespace dmz {

   class QtPluginTypePalletTool :
         public QFrame,
         public Plugin,
         public MessageObserver,
         public QtWidget {
            
      Q_OBJECT

      public:
         QtPluginTypePalletTool (const PluginInfo &Info, Config &local);
         ~QtPluginTypePalletTool ();

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

         // QtWidget Interface
         virtual QWidget *get_qt_widget ();

      protected slots:
         void on_filter_textChanged (const QString &Text);


      protected:
         // QtPluginTypePalletTool Interface
         void _init (Config &local);

         Log _log;
         Ui::TypeFrame _ui;
         MimeModel _model;
         MimeModel _listModel;
         QSortFilterProxyModel _proxyModel;
         int _labelCount;


      private:
         QtPluginTypePalletTool ();
         QtPluginTypePalletTool (const QtPluginTypePalletTool &);
         QtPluginTypePalletTool &operator= (const QtPluginTypePalletTool &);

   };
};

#endif // DMZ_QT_PLUGIN_TYPE_PALLET_TOOL_DOT_H
