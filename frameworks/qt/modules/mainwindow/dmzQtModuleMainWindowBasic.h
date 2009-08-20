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
         
         // QtWidget Interface
         virtual QWidget *get_qt_widget ();

      protected:
         struct ToolBarStruct;
         typedef QList<ToolBarStruct *> ToolBarList;

         struct DockWidgetStruct {

            const String Name;
            QDockWidget *dock;
            QWidget *widget;
            Qt::DockWidgetArea area;
            QDockWidget::DockWidgetFeatures features;
            Qt::DockWidgetAreas allowedAreas;
            Boolean visible;
            Boolean floating;

            DockWidgetStruct (
               const String &WidgetName,
               const Config &Data,
               const Qt::DockWidgetArea Area);

            ~DockWidgetStruct ();
            
            void set_widget (QWidget *widget);
            
            void add_to (QMainWindow *window);
            void remove_from (QMainWindow *window);
         };

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

         void _create_actions ();
         void _create_menus ();
         void _create_tool_bars ();
         void _create_status_bar ();

         void _save_session ();
         void _load_session ();
         void _init_dock_windows (Config &local);
         void _init (Config &local);

         Exit _exit;
         Log _log;
         Ui::MainWindow _ui;
         QString _windowName;
         QAction *_fileExitAction;
         QMenu *_fileMenu;
         String _mainWidgetName;
         HashTableStringTemplate<DockWidgetStruct> _dockWidgetTable;

      private:
         QtModuleMainWindowBasic ();
         QtModuleMainWindowBasic (const QtModuleMainWindowBasic &);
         QtModuleMainWindowBasic &operator= (const QtModuleMainWindowBasic &);
   };
};

#endif // DMZ_QT_MODULE_MAIN_WINDOW_BASIC_DOT_H
