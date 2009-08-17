#ifndef DMZ_QT_MODULE_MAIN_WINDOW_BASIC_DOT_H
#define DMZ_QT_MODULE_MAIN_WINDOW_BASIC_DOT_H

#include <dmzInputObserverUtil.h>
#include <dmzQtModuleMainWindow.h>
#include <dmzRuntimeExit.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzTypesHashTableHandleTemplate.h>
#include <dmzTypesHashTableStringTemplate.h>
#include <dmzTypesStringContainer.h>
#include <QtGui/QMainWindow>
#include <QtGui/QDockWidget>
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
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level);

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

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

      protected:
         struct ChannelStruct;
         struct ToolBarStruct;
         typedef QList<ToolBarStruct *> ToolBarList;

         struct MainWindowStruct {

            QMainWindow *main;
            QStackedWidget *stack;

            MainWindowStruct () : main (0), stack (0) {;}
         };

         struct WidgetStruct {

            QWidget *widget;
            String title;

            virtual void show (MainWindowStruct &window) = 0;
            virtual void hide (MainWindowStruct &window) = 0;
            virtual void remove (MainWindowStruct &window) = 0;
 
            WidgetStruct () : widget (0) {;}

            virtual ~WidgetStruct () {;}
         };

         struct DockWidgetStruct : public WidgetStruct {

            String name;
            QDockWidget *dock;
            QDockWidget::DockWidgetFeatures features;
            Qt::DockWidgetArea area;
            Qt::DockWidgetAreas allowedAreas;
            Boolean visible;
            Boolean floating;
            
            virtual void show (MainWindowStruct &window);
            virtual void hide (MainWindowStruct &window);
            virtual void remove (MainWindowStruct &window);

            DockWidgetStruct () :
                  dock (0),
                  features (QDockWidget::AllDockWidgetFeatures),
                  area (Qt::NoDockWidgetArea),
                  allowedAreas (Qt::NoDockWidgetArea),
                  visible (True),
                  floating (False) {;}

            virtual ~DockWidgetStruct () { if (dock) { delete dock; dock = 0; } }
         };

         struct CentralWidgetStruct : public WidgetStruct {

            virtual void show (MainWindowStruct &window);
            virtual void hide (MainWindowStruct &window);
            virtual void remove (MainWindowStruct &window);

            CentralWidgetStruct () {;}
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

         struct ChannelStruct {

            const Handle Channel;
            Int32 count;
            ToolBarList toolbar;
            HashTableStringTemplate<WidgetStruct> widgetTable;

            ChannelStruct (const Handle TheChannel) : Channel (TheChannel), count (0) {;}
         };

         virtual void closeEvent (QCloseEvent *event);

         void _create_actions ();
         void _create_menus ();
         void _create_tool_bars ();
         void _create_status_bar ();

         void _update ();

         void _save_session ();
         void _load_session ();
         void _init_widget_group (ChannelStruct &cs, Config &group);
         void _init_input_channels (Config &local);
         void _init (Config &local);

         Exit _exit;
         Log _log;
         Ui::MainWindow _ui;
         MainWindowStruct _windowInfo;
         QString _windowName;
         QAction *_fileExitAction;
         QMenu *_fileMenu;
         HashTableStringTemplate<WidgetStruct> _widgetTable;
         HashTableHandleTemplate<ChannelStruct> _channelTable;
         HashTableStringTemplate<StringContainer> _tabTable;

      private:
         QtModuleMainWindowBasic ();
         QtModuleMainWindowBasic (const QtModuleMainWindowBasic &);
         QtModuleMainWindowBasic &operator= (const QtModuleMainWindowBasic &);
   };
};

#endif // DMZ_QT_MODULE_MAIN_WINDOW_BASIC_DOT_H
