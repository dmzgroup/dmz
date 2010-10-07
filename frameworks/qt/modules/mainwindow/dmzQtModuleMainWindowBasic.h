#ifndef DMZ_QT_MODULE_MAIN_WINDOW_BASIC_DOT_H
#define DMZ_QT_MODULE_MAIN_WINDOW_BASIC_DOT_H

#include <dmzQtModuleMainWindow.h>
#include <dmzQtWidget.h>
#include <dmzRuntimeExit.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzTypesHashTableHandleTemplate.h>
#include <dmzTypesHashTableStringTemplate.h>
#include <QtGui/QMainWindow>
#include <QtGui/QDockWidget>
#include "ui_dmzQtMainWindow.h"


namespace dmz {

   class QtModuleMainWindowBasic :
         public QMainWindow,
         public Plugin,
         public QtModuleMainWindow,
         public QtWidget {

      Q_OBJECT

      public:
         QtModuleMainWindowBasic (const PluginInfo &Info, Config &local);
         ~QtModuleMainWindowBasic ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level);

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // QtModuleMainWindow Interface
         virtual QString get_window_name ();
         virtual QMainWindow *get_qt_main_window ();
         virtual QMenu *lookup_menu (const String &Text);
         virtual void add_menu_action (const String &MenuName, QAction *action);
         virtual void remove_menu_action (const String &MenuName, QAction *action);

         virtual QDockWidget *create_dock_widget (const String &DockName, QWidget *widget);
         virtual QDockWidget *lookup_dock_widget (const String &DockName);

         virtual Boolean add_dock_widget (const String &DockName, const Qt::DockWidgetArea Area);
         virtual Boolean add_dock_widget (QDockWidget *dock, const Qt::DockWidgetArea Area);

         virtual Boolean remove_dock_widget (const String &DockName);
         virtual Boolean remove_dock_widget (QDockWidget *dock);

         // QtWidget Interface
         virtual QWidget *get_qt_widget ();

      protected:
         struct ToolBarStruct;
         typedef QList<ToolBarStruct *> ToolBarList;

         struct ToolBarStruct {

            QToolBar *toolBar;
            Qt::ToolBarArea area;
            Boolean visible;

            ToolBarStruct () :
               toolBar (0),
               area (Qt::NoToolBarArea),
               visible (False) {;}
         };

         virtual void closeEvent (QCloseEvent *event);
         void _save_session ();
         void _load_session ();
         void _restore_state ();
         void _init_dock_windows (Config &local);
         void _init (Config &local);

         Exit _exit;
         Log _log;
         Ui::MainWindow _ui;
         QString _windowName;
         QMenuBar *_menuBar;
         QAction *_exitAction;
         String _mainWidgetName;
         String _windowMenuName;
         String _fileMenuName;
         HashTableStringTemplate<QMenu> _menuTable;
         HashTableStringTemplate<QDockWidget> _dockTable;
         QList<QDockWidget *> _docksToShow;
         QList<QDockWidget *> _docksToFloat;
         Boolean _fixedSize;

      private:
         QtModuleMainWindowBasic ();
         QtModuleMainWindowBasic (const QtModuleMainWindowBasic &);
         QtModuleMainWindowBasic &operator= (const QtModuleMainWindowBasic &);
   };
};


#endif // DMZ_QT_MODULE_MAIN_WINDOW_BASIC_DOT_H
