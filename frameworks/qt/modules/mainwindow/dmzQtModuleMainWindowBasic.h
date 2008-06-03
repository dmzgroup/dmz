#ifndef DMZ_QT_MODULE_MAIN_WINDOW_BASIC_DOT_H
#define DMZ_QT_MODULE_MAIN_WINDOW_BASIC_DOT_H

#include <dmzInputObserverUtil.h>
#include <dmzQtModuleMainWindow.h>
#include <dmzRuntimeExit.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzTypesHashTableHandleTemplate.h>
#include <QtGui/QMainWindow>
#include "ui_dmzQtMainWindow.h"


namespace dmz {

   class QtModuleMainWindowBasic :
         public QMainWindow,
         public Plugin,
         public InputObserverUtil,
         public QtModuleMainWindow {

      Q_OBJECT
      
      public:
         QtModuleMainWindowBasic (const PluginInfo &Info, Config &local);
         ~QtModuleMainWindowBasic ();

         // Plugin Interface
         virtual void discover_plugin (const Plugin *PluginPtr);
         virtual void start_plugin ();
         virtual void stop_plugin ();
         virtual void shutdown_plugin ();
         virtual void remove_plugin (const Plugin *PluginPtr);

         // Input Observer Interface
         virtual void update_channel_state (const UInt32 Channel, const Boolean State);

         virtual void receive_axis_event (
            const UInt32 Channel,
            const InputEventAxis &Value) {;}

         virtual void receive_button_event (
            const UInt32 Channel,
            const InputEventButton &Value) {;}

         virtual void receive_key_event (
            const UInt32 Channel,
            const InputEventKey &Value) {;}

         virtual void receive_mouse_event (
            const UInt32 Channel,
            const InputEventMouse &Value) {;}

         virtual void receive_switch_event (
            const UInt32 Channel,
            const InputEventSwitch &Value) {;}

         // QtModuleMainWindow Interface
         virtual QString get_window_name ();
         virtual QWidget *get_widget ();
         virtual QMenuBar *get_menu_bar ();
         virtual QStatusBar *get_status_bar ();
         
         virtual Boolean add_tool_bar (QToolBar *toolBar);
         virtual Boolean add_tool_bar (const Handle Channel, QToolBar *toolBar);
         
         virtual Boolean remove_tool_bar (QToolBar *toolBar);
         virtual Boolean remove_tool_bar (const Handle Channel, QToolBar *toolBar);
            
         virtual void add_tool_bar_break (const Qt::ToolBarArea Area);
            
         virtual Boolean add_dock_widget (
            const Qt::DockWidgetArea Area,
            QDockWidget *dock);
            
         virtual Boolean add_dock_widget (
            const Handle Channel,
            const Qt::DockWidgetArea Area,
            QDockWidget *dock);
         
         virtual Boolean remove_dock_widget (QDockWidget *dock);
         virtual Boolean remove_dock_widget (const Handle Channel, QDockWidget *dock);
         
         virtual Boolean add_central_widget (const Handle Channel, QWidget *widget);
         virtual Boolean remove_central_widget (const Handle Channel);
         
      protected:
         struct ToolBarStruct {
         
            QToolBar *toolBar;
            Qt::ToolBarArea area;
            Boolean visible;
         };
         
         struct DockWidgetStruct {
           
            QDockWidget *dockWidget;
            Qt::DockWidgetArea area; 
            Boolean visible;
         };
         
         typedef QList<ToolBarStruct *> ToolBarList;
         typedef QList<DockWidgetStruct *> DockWidgetList;
         
         virtual void closeEvent (QCloseEvent *event);

         void _create_actions ();
         void _create_menus ();
         void _create_tool_bars ();
         void _create_status_bar ();

         void _update ();
         
         void _save_session ();
         void _load_session ();
         void _init (Config &local);

         Exit _exit;
         Log _log;
         Ui::MainWindow _ui;
         QString _windowName;
         QAction *_fileExitAction;
         QMenu *_fileMenu;
         Boolean _showUnifiedTitleAndToolBar;
         HashTableHandleTemplate<QWidget> _centralWidgetTable;
         HashTableHandleTemplate<DockWidgetList> _dockWidgetTable;
         HashTableHandleTemplate<ToolBarList> _toolBarTable;
         HashTableHandleTemplate<Int32> _channelTable;
         Handle _channel;

      private:
         QtModuleMainWindowBasic ();
         QtModuleMainWindowBasic (const QtModuleMainWindowBasic &);
         QtModuleMainWindowBasic &operator= (const QtModuleMainWindowBasic &);
   };
};

#endif // DMZ_QT_MODULE_MAIN_WINDOW_BASIC_DOT_H
