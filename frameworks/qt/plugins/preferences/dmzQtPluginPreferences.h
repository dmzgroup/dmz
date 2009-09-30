#ifndef DMZ_QT_PLUGIN_PREFERENCES_DOT_H
#define DMZ_QT_PLUGIN_PREFERENCES_DOT_H

#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzTypesHashTableStringTemplate.h>
#include <QtCore/QList>
#include <QtGui/QFrame>
#include "ui_dmzQtPluginPreferences.h"


namespace dmz {

   class QtModuleMainWindow;

   class QtPluginPreferences :
         public QFrame,
         public Plugin {

   Q_OBJECT
   
      public:
         QtPluginPreferences (const PluginInfo &Info, Config &local);
         ~QtPluginPreferences ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level);

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

//      protected Q_SLOTS:
         
      protected:
         void keyPressEvent (QKeyEvent *event);
         void _save_session ();
         void _load_session ();
         void _init (Config &local);

         struct WidgetStruct {

            String title;
            QWidget *widget;

            WidgetStruct () : title (), widget (0) {;}
         };

         Log _log;
         Ui::preferencesDialog _ui;
         QtModuleMainWindow *_mainWindowModule;
         String _mainWindowModuleName;
         QAction *_preferencesAction;
         String _menuName;
         HashTableStringTemplate<WidgetStruct> _widgetTable;

      private:
         QtPluginPreferences ();
         QtPluginPreferences (const QtPluginPreferences &);
         QtPluginPreferences &operator= (const QtPluginPreferences &);
   };
};

#endif // DMZ_QT_PLUGIN_PREFERENCES_DOT_H
