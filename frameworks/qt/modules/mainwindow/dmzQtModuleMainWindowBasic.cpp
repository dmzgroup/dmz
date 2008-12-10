#include <dmzInputEventMasks.h>
#include <dmzInputModule.h>
#include <dmzInputConsts.h>
#include "dmzQtModuleMainWindowBasic.h"
#include <dmzQtUtil.h>
#include <dmzQtWidget.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzRuntimeSession.h>
#include <QtGui/QtGui>

namespace {

static const int LocalSessionVersion = 4;

};

void
dmz::QtModuleMainWindowBasic::DockWidgetStruct::show (MainWindowStruct &window) {

   if (widget && window.main && !dock) {

      dock = new QDockWidget (title ? title.get_buffer () : "");
      QLayout *layout (dock->layout ());
      dock->setObjectName (name.get_buffer ());
      dock->setAllowedAreas (startArea);
      dock->setFeatures (features);
      dock->setWidget (widget);
      window.main->addDockWidget (startArea, dock);
      dock->show ();
   }

   if (window.main && dock && !dock->parentWidget ()) {

      window.main->addDockWidget (startArea, dock);
      
      dock->show ();
   }
}


void
dmz::QtModuleMainWindowBasic::DockWidgetStruct::hide (MainWindowStruct &window) {

   if (dock && dock->parentWidget ()) {

      window.main->removeDockWidget (dock);
      dock->setParent (0);
   }
}


void
dmz::QtModuleMainWindowBasic::DockWidgetStruct::remove (MainWindowStruct &window) {

   hide (window);

   if (dock && widget) {

      dock->setWidget (0);
      widget->setParent (0);
   }
}


void
dmz::QtModuleMainWindowBasic::CentralWidgetStruct::show (MainWindowStruct &window) {

   if (widget && window.stack) {

      if (!widget->parentWidget ()) {

         window.stack->addWidget (widget);
         widget->show ();
      }

      window.stack->setCurrentWidget (widget);
   }
}


void
dmz::QtModuleMainWindowBasic::CentralWidgetStruct::hide (MainWindowStruct &window) {

   // Nothing to do?
}


void
dmz::QtModuleMainWindowBasic::CentralWidgetStruct::remove (MainWindowStruct &window) {

   if (widget && window.stack) {

      if (widget->parentWidget ()) {

         window.stack->removeWidget (widget);
         widget->setParent (0);
      }
   }
}


dmz::QtModuleMainWindowBasic::QtModuleMainWindowBasic (
      const PluginInfo &Info,
      Config &local) :
      QMainWindow (0),
      QtModuleMainWindow (Info),
      Plugin (Info),
      InputObserverUtil (Info, local),
      _exit (get_plugin_runtime_context ()),
      _log (Info),
      _fileExitAction (0),
      _fileMenu (0),
      _showUnifiedTitleAndToolBar (False) {

   setObjectName (get_plugin_name ().get_buffer ());
//   setAttribute (Qt::WA_AlwaysShowToolTips);

   _ui.setupUi (this);

   _windowInfo.main = this;
   _windowInfo.stack = _ui.stackedWidget;

   _create_actions ();
   _create_menus ();
   _create_tool_bars ();
   _create_status_bar ();

   QWidget *widget (new QWidget (this));
   _ui.stackedWidget->addWidget (widget);

   _init (local);

   setCorner (Qt::BottomRightCorner, Qt::RightDockWidgetArea);
}


dmz::QtModuleMainWindowBasic::~QtModuleMainWindowBasic () {

   _widgetTable.empty ();
   _channelTable.empty ();
}




// Plugin Interface
void
dmz::QtModuleMainWindowBasic::update_plugin_state (
      const PluginStateEnum State,
      const UInt32 Level) {

   if (State == PluginStateStart) {

      _load_session ();
      setUnifiedTitleAndToolBarOnMac (_showUnifiedTitleAndToolBar);
      show ();
      raise ();
   }
   else if (State == PluginStateStop) {

      _save_session ();
      hide ();
   }
}


void
dmz::QtModuleMainWindowBasic::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      QtWidget *w = QtWidget::cast (PluginPtr);

      if (w) {

         const String Name = w->get_qt_widget_name ();
         QWidget *widget = w->get_qt_widget ();
         WidgetStruct *ws = _widgetTable.lookup (Name);

         if (ws) {

            ws->widget = widget;
         }
      }
   }
   else if (Mode == PluginDiscoverRemove) {

      QtWidget *w = QtWidget::cast (PluginPtr);

      if (w) {

         const String Name = w->get_qt_widget_name ();
         WidgetStruct *ws = _widgetTable.lookup (Name);
         if (ws && (ws->widget == w->get_qt_widget ())) {

            ws->remove (_windowInfo);
            ws->widget = 0;
         }
      }
   }
}


void
dmz::QtModuleMainWindowBasic::update_channel_state (
      const UInt32 Channel,
      const Boolean State) {

   ChannelStruct *cs (_channelTable.lookup (Channel));

   if (cs) {

      if (State) { cs->count++; }
      else { cs->count--; }

      _update ();
   }
}


// QtModuleMainWindow Interface
QString
dmz::QtModuleMainWindowBasic::get_window_name () { return _windowName; }


QWidget *
dmz::QtModuleMainWindowBasic::get_widget () { return this; }


QMenuBar *
dmz::QtModuleMainWindowBasic::get_menu_bar () { return menuBar (); }


QStatusBar *
dmz::QtModuleMainWindowBasic::get_status_bar () { return statusBar (); }


dmz::Boolean
dmz::QtModuleMainWindowBasic::add_tool_bar (QToolBar *toolBar) {

   Boolean retVal (False);

   if (toolBar) {

      addToolBar (toolBar);
      retVal = True;
   }

   return retVal;
}


dmz::Boolean
dmz::QtModuleMainWindowBasic::add_tool_bar (const Handle Channel, QToolBar *toolBar) {

   Boolean retVal (False);

   ChannelStruct *cs (_channelTable.lookup (Channel));

   if (cs && toolBar) {

      ToolBarStruct *tbs (new ToolBarStruct ());
      tbs->visible = True;
      tbs->toolBar = toolBar;
      tbs->area = Qt::TopToolBarArea;

      cs->toolbar.append (tbs);

      if (cs->count) {

         addToolBar (toolBar);
      }

      retVal = True;
   }

   return retVal;
}


dmz::Boolean
dmz::QtModuleMainWindowBasic::remove_tool_bar (QToolBar *toolBar) {

   Boolean retVal (False);

   if (toolBar) {

      removeToolBar (toolBar);
      toolBar->setParent (0);
      retVal = True;
   }

   return retVal;
}


dmz::Boolean
dmz::QtModuleMainWindowBasic::remove_tool_bar (const Handle Channel, QToolBar *toolBar) {

   Boolean retVal (False);
   ChannelStruct *cs (_channelTable.lookup (Channel));

   if (cs && toolBar) {

      ToolBarStruct *tbs (0);

      foreach (ToolBarStruct *ctbs, cs->toolbar) {

         if (ctbs->toolBar == toolBar) {

            tbs = ctbs;
            break;
         }
      }

      Int32 index (cs->toolbar.indexOf (tbs));

      if (index != -1) {

         cs->toolbar.takeAt (index);
         removeToolBar (tbs->toolBar);
         delete tbs; tbs = 0;
         retVal = True;
      }

      toolBar->setParent (0);
   }

   return retVal;
}


void
dmz::QtModuleMainWindowBasic::add_tool_bar_break (const Qt::ToolBarArea Area) {

   addToolBarBreak (Area);
}


void
dmz::QtModuleMainWindowBasic::closeEvent (QCloseEvent *event) {

   _exit.request_exit (dmz::ExitStatusNormal, get_plugin_name () + " Closed");
}


void
dmz::QtModuleMainWindowBasic::_create_actions () {

   _fileExitAction = new QAction (tr ("E&xit"), this);
   _fileExitAction->setShortcut (tr ("Ctrl+Q"));
   _fileExitAction->setStatusTip (tr ("Exit the application"));
   connect (_fileExitAction, SIGNAL (triggered ()), this, SLOT (close ()));
}


void
dmz::QtModuleMainWindowBasic::_create_menus () {

   _fileMenu = menuBar ()->addMenu (tr ("&File"));
   _fileMenu->addAction (_fileExitAction);
}


void
dmz::QtModuleMainWindowBasic::_create_tool_bars () {

}


void
dmz::QtModuleMainWindowBasic::_create_status_bar () {

   statusBar()->showMessage (tr ("Ready"), 2000);
}


void
dmz::QtModuleMainWindowBasic::_update () {

   _ui.stackedWidget->setCurrentIndex (0);

   HashTableHandleIterator it;

   ChannelStruct *cs (_channelTable.get_first (it));

   while (cs) {

      foreach (ToolBarStruct *tbs, cs->toolbar) {

         if (cs->count > 0) {

            addToolBar (tbs->area, tbs->toolBar);
            //if (tbs->visible) { tbs->toolBar->setVisible (True); }
            tbs->toolBar->setVisible (True);
         }
         else {

            //tbs->visible = tbs->toolBar->isVisible ();
            //tbs->area = toolBarArea (tbs->toolBar);
            removeToolBar (tbs->toolBar);
         }
      }

      HashTableStringIterator wit;
      WidgetStruct *ws (cs->widgetTable.get_first (wit));

      while (ws) {
         
         if (cs->count > 0) { ws->show (_windowInfo); } 
         else { ws->hide (_windowInfo); }

         ws = cs->widgetTable.get_next (wit);
      }

      cs = _channelTable.get_next (it);
   }
}


void
dmz::QtModuleMainWindowBasic::_save_session () {

   String data;

   Config session (get_plugin_name ());

   session.add_config (
      qbytearray_to_config ("geometry", saveGeometry ()));

   session.add_config (qbytearray_to_config ("state", saveState (LocalSessionVersion)));

   set_session_config (get_plugin_runtime_context (), session);
}


void
dmz::QtModuleMainWindowBasic::_load_session () {

   Config session (
      get_session_config (get_plugin_name (), get_plugin_runtime_context ()));

   QByteArray geometry (config_to_qbytearray ("geometry", session, saveGeometry ()));
   restoreGeometry (geometry);

   QByteArray stateData (
      config_to_qbytearray ("state", session, saveState (LocalSessionVersion)));

   restoreState (stateData, LocalSessionVersion);
}


void
dmz::QtModuleMainWindowBasic::_init_widget_group (ChannelStruct &cs, Config &group) {

   Config widgetList;

   Qt::DockWidgetArea area = Qt::NoDockWidgetArea;

   Boolean isMain (False);

   String AreaName (config_to_string ("area", group).get_lower ());

   if (AreaName == "main") { isMain = True; }
   else if (AreaName == "left") { area = Qt::LeftDockWidgetArea; }
   else if (AreaName == "right") { area = Qt::RightDockWidgetArea; }
   else if (AreaName == "top") { area = Qt::TopDockWidgetArea; }
   else if (AreaName == "bottom") { area = Qt::BottomDockWidgetArea; }
   else {

      _log.error << "Unknown widget area: " << AreaName << endl;
   }

   if (group.lookup_all_config ("widget", widgetList)) {

      ConfigIterator it;
      Config widget;

      while (widgetList.get_next_config (it, widget)) {

         const String WidgetName (config_to_string ("name", widget));
         const String WidgetTitle (config_to_string ("title", widget));

         if (!_widgetTable.lookup (WidgetName)) {

            WidgetStruct *ws (0);

            if (isMain) {

               CentralWidgetStruct *cws = new CentralWidgetStruct;
               ws = cws;
            }
            else {

               DockWidgetStruct *dws = new DockWidgetStruct;
               ws = dws;

               if (dws) { dws->startArea = area; dws->name = WidgetName; }
            }

            if (ws && _widgetTable.store (WidgetName, ws)) {

               ws->title = WidgetTitle;
               cs.widgetTable.store (WidgetName, ws);
            }
            else { delete ws; ws = 0; }
         }
         else {

            _log.error << "Widget: " << WidgetName
               << " has already been mapped to the main window." << endl;
         }
      }
   }
}


void
dmz::QtModuleMainWindowBasic::_init_input_channels (Config &local) {

   Config channelList;

   if (local.lookup_all_config ("channel", channelList)) {

      ConfigIterator it;
      Config channel;

      while (channelList.get_next_config (it, channel)) {

         const String ChannelName (
            config_to_string ("name", channel, InputChannelDefaultName));

         const Handle ChannelHandle (
            activate_input_channel (ChannelName, InputEventChannelStateMask));

         ChannelStruct *cs (_channelTable.lookup (ChannelHandle));

         if (!cs) {

            cs = new ChannelStruct (ChannelHandle);

            if (cs && !_channelTable.store (cs->Channel, cs)) { delete cs; cs = 0; }
         }

         if (cs) {

            Config groupList;

            if (channel.lookup_all_config ("group", groupList)) {

               ConfigIterator wit;
               Config group;

               while (groupList.get_next_config (wit, group)) {

                  _init_widget_group (*cs, group);
               }
            }
         }
      }
   }
}

void
dmz::QtModuleMainWindowBasic::_init (Config &local) {

   _windowName =
      config_to_string ("window.title", local, "DMZ Application").get_buffer ();

   if (!_windowName.isEmpty ()) { setWindowTitle (_windowName); }

   String iconFile (config_to_string ("window.icon", local, ":/images/windowIcon.png"));

   if (iconFile) {

      QFileInfo fi (iconFile.get_buffer ());

      if (fi.exists ()) {

         setWindowIcon (QIcon (fi.absoluteFilePath ()));
         _log.info << "Window Icon: " << iconFile << endl;
      }
   }

   _init_input_channels (local);

   _showUnifiedTitleAndToolBar = config_to_boolean (
      "showUnifiedTitleAndToolBar.value",
      local, _showUnifiedTitleAndToolBar);

   set_qwidget_stylesheet ("stylesheet", local, this);

   if (config_to_boolean ("hide.value", local, False)) { hide (); }
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzQtModuleMainWindowBasic (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::QtModuleMainWindowBasic (Info, local);
}

};
