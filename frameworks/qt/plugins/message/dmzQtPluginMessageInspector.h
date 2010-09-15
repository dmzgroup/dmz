#ifndef DMZ_QT_PLUGIN_MESSAGE_INSPECTOR_DOT_H
#define DMZ_QT_PLUGIN_MESSAGE_INSPECTOR_DOT_H

#include <dmzRuntimeDefinitionsObserver.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimeMessaging.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimeTimeSlice.h>
#include <dmzTypesHandleContainer.h>
#include <dmzTypesHashTableHandleTemplate.h>
#include <QtGui/QTreeWidget>
#include <QtGui/QWidget>
#include <QtGui/QListWidget>
#include <ui_MessageInspector.h>

namespace dmz {

   class QtPluginMessageInspector :
         public QWidget,
         public Plugin,
         public MessageObserver,
         public TimeSlice,
         public DefinitionsObserver {

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

         virtual void update_time_slice (const Float64 TimeDelta);

         virtual void define_message (const Message &Type);

      protected slots:
         void _check_all_targets ();
         void _uncheck_all_targets ();
         void _check_all_types ();
         void _uncheck_all_types ();
         void _update_displayed_text ();
         void _apply_new_filter (QListWidgetItem *);


      protected:
         // QtPluginMessageInspector Interface
         void _init (Config &local);

         Log _log;
         Ui::InspectorForm _ui;
         QStringList _targetFilterList;
         QStringList _typeFilterList;


      private:
         QtPluginMessageInspector ();
         QtPluginMessageInspector (const QtPluginMessageInspector &);
         QtPluginMessageInspector &operator= (const QtPluginMessageInspector &);

         UInt32 __messageCount;
         String __runtimedata_to_string (const Data &Value);

   };
};

#endif // DMZ_QT_PLUGIN_MESSAGE_INSPECTOR_DOT_H
