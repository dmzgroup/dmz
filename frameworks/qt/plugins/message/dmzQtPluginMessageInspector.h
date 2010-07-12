#ifndef DMZ_QT_PLUGIN_MESSAGE_INSPECTOR_DOT_H
#define DMZ_QT_PLUGIN_MESSAGE_INSPECTOR_DOT_H

#include <dmzRuntimeLog.h>
#include <dmzRuntimeMessaging.h>
#include <dmzRuntimePlugin.h>
#include <dmzTypesHandleContainer.h>
#include <dmzTypesHashTableHandleTemplate.h>
#include <QtGui/QTreeWidget>
#include <QtGui/QWidget>
#include <ui_MessageInspector.h>

namespace dmz {

   class QtPluginMessageInspector :
         public QWidget,
         public Plugin,
         public MessageObserver {

      Q_OBJECT

      public:
         QtPluginMessageInspector (const PluginInfo &Info, Config &local);
         ~QtPluginMessageInspector ();

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
            const Handle MessageSendHandle,
            const Handle TargetObserverHandle,
            const Data *InData,
            Data *outData);

      protected slots:
         void _update_displayed_text ();
         void _filter_for_selected_target (const QString &text);

      protected:
         // QtPluginMessageInspector Interface
         void _init (Config &local);

         Log _log;
         Ui::InspectorForm _ui;


      private:
         QtPluginMessageInspector ();
         QtPluginMessageInspector (const QtPluginMessageInspector &);
         QtPluginMessageInspector &operator= (const QtPluginMessageInspector &);

         //QStandardItem *__parentItem;
         String __runtimedata_to_string (const Data &Value);

   };
};

#endif // DMZ_QT_PLUGIN_MESSAGE_INSPECTOR_DOT_H
