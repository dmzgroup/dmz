#ifndef DMZ_QT_PLUGIN_LIST_DOT_H
#define DMZ_QT_PLUGIN_LIST_DOT_H

#include <dmzRuntimeConfig.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimeMessaging.h>
#include <dmzRuntimeModule.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimePluginObserver.h>
#include <dmzTypesHashTableHandleTemplate.h>

#include <QtGui/QFrame>
#include <QtGui/QSortFilterProxyModel>
#include <QtGui/QStandardItemModel>
#include <ui_PluginList.h>


namespace dmz {

   class HandleContainer;

   class QtPluginList :
         public QFrame,
         public Plugin,
         public PluginObserver,
         public MessageObserver {

      Q_OBJECT

      public:
         QtPluginList (const PluginInfo &Info, Config &local, Config &global);
         ~QtPluginList ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level);

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // PluginObserver Interface
         virtual void update_runtime_plugin (
            const PluginDiscoverEnum Mode,
            const Handle RuntimeModuleHandle,
            const Handle PluginHandle);

         // Message Observer Interface
         virtual void receive_message (
            const Message &Type,
            const UInt32 MessageSendHandle,
            const Handle TargetObserverHandle,
            const Data *InData,
            Data *outData);

      protected slots:
         void on_unloadButton_clicked ();
         void on_reloadButton_clicked ();
         void on_filter_textChanged (const QString &Text);

      protected:
         typedef QList<QStandardItem *> QStandardItemList;

         // QtPluginList Interface
         void _get_selected (HandleContainer &list);
         void _init (Config &local);

         Log _log;
         Config _global;
         Ui::PluginList _ui;
         QStandardItemModel _model;
         QSortFilterProxyModel _proxyModel;
         QString _baseTitle;
         int _labelCount;

         HashTableHandleTemplate<QStandardItem> _itemTable;
         HashTableHandleTemplate<RuntimeModule> _rmTable;

         Message _showMsg;

      private:
         QtPluginList ();
         QtPluginList (const QtPluginList &);
         QtPluginList &operator= (const QtPluginList &);
   };
};

#endif // DMZ_QT_PLUGIN_LIST_DOT_H
