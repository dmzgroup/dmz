#ifndef DMZ_QT_PLUGIN_ICON_PALLET_TOOL_DOT_H
#define DMZ_QT_PLUGIN_ICON_PALLET_TOOL_DOT_H

#include "dmzMimeModel.h"
#include <dmzQtWidget.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimeMessaging.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimeResources.h>

#include <QtGui/QFrame>
#include <ui_IconFrame.h>

namespace dmz {

   class ObjectType;

   class QtPluginIconPalletTool :
         public QFrame,
         public Plugin,
         public MessageObserver,
         public QtWidget {
            
      Q_OBJECT

      public:
         QtPluginIconPalletTool (const PluginInfo &Info, Config &local);
         ~QtPluginIconPalletTool ();

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

      protected:
         // QtPluginIconPalletTool Interface
         void _add_type (const ObjectType &Type);
         void _init (Config &local);

         Log _log;
         Resources _rc;
         Ui::IconFrame _ui;
         MimeModel _model;


      private:
         QtPluginIconPalletTool ();
         QtPluginIconPalletTool (const QtPluginIconPalletTool &);
         QtPluginIconPalletTool &operator= (const QtPluginIconPalletTool &);

   };
};

#endif // DMZ_QT_PLUGIN_ICON_PALLET_TOOL_DOT_H
