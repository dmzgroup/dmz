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

static const int LocalSessionVersion = 6;

};


dmz::QtModuleMainWindowBasic::DockWidgetStruct::DockWidgetStruct (
      const String &WidgetName,
      const Config &Data,
      const Qt::DockWidgetArea Area) :
      Name (WidgetName),
      dock (0),
      widget (0),
      area (Area) {
            
   const String Title (config_to_string ("title", Data, ""));
   
   dock = new QDockWidget (Title.get_buffer ());
   dock->setObjectName (WidgetName.get_buffer ());
   dock->setAllowedAreas (config_to_dock_widget_areas ("allowed-areas", Data, area));
   dock->setFeatures (QDockWidget::AllDockWidgetFeatures);
   
   visible = config_to_boolean ("visible", Data, True);
   floating = config_to_boolean ("floating", Data, False);
}


dmz::QtModuleMainWindowBasic::DockWidgetStruct::~DockWidgetStruct () {
   
   if (dock) { delete dock; dock = 0; }
}


void
dmz::QtModuleMainWindowBasic::DockWidgetStruct::set_widget (QWidget *theWidget) {

   if (dock) {
      
      if (theWidget) {

         if (widget) {
            
            dock->setWidget (0);
            widget->setParent (0);
         }
         
         widget = theWidget;
         widget->adjustSize ();
         dock->setWidget (widget);
         dock->hide ();
      }
      else {

         dock->setWidget (0);
         
         if (widget) {
            
            widget->setParent (0);
            widget = 0;
         }
      }
   }
}


void
dmz::QtModuleMainWindowBasic::DockWidgetStruct::add_to (QMainWindow *window) {

   if (dock && widget && window) {
      
      dock->adjustSize ();
      window->addDockWidget (area, dock);
      dock->setFloating (floating);
      if (visible) { dock->show (); }
      else { dock->hide (); }
   }
}


void
dmz::QtModuleMainWindowBasic::DockWidgetStruct::remove_from (QMainWindow *window) {
   
   if (dock && window) {
      
      window->removeDockWidget (dock);
      dock->setParent (0);
   }
}


dmz::QtModuleMainWindowBasic::QtModuleMainWindowBasic (
      const PluginInfo &Info,
      Config &local) :
      QMainWindow (0),
      Plugin (Info),
      QtModuleMainWindow (Info),
      QtWidget (Info),
      _exit (get_plugin_runtime_context ()),
      _log (Info),
      _exitAction (0),
      _mainWidgetName (),
      _windowMenuName ("&Window"),
      _dockWidgetTable () {

   setObjectName (get_plugin_name ().get_buffer ());

   _ui.setupUi (this);

   _init (local);

   statusBar()->showMessage (tr ("Ready"), 2000);
}


dmz::QtModuleMainWindowBasic::~QtModuleMainWindowBasic () {

   _dockWidgetTable.empty ();
}


// Plugin Interface
void
dmz::QtModuleMainWindowBasic::update_plugin_state (
      const PluginStateEnum State,
      const UInt32 Level) {

   if (State == PluginStateInit) {

      HashTableStringIterator it;
      DockWidgetStruct *dws (_dockWidgetTable.get_first (it));
      
      while (dws) {

         dws->add_to (this);
         add_menu_action (_windowMenuName, dws->dock->toggleViewAction ());
         dws = _dockWidgetTable.get_next (it);
      }
      
      _load_session ();
      
      show ();
      raise ();
      activateWindow ();
      
      _restore_state ();
   }
   else if (State == PluginStateStart) {

   }
   else if (State == PluginStateStop) {

   }
   else if (State == PluginStateShutdown) {

      _save_session ();
      
      HashTableStringIterator it;
      DockWidgetStruct *dws (_dockWidgetTable.get_first (it));

      while (dws) {

         dws->dock->hide ();
         dws = _dockWidgetTable.get_next (it);
      }
      
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
         
         if (Name == _mainWidgetName) {
         
            setCentralWidget (widget);
         }
         else {
            
            DockWidgetStruct *dws = _dockWidgetTable.lookup (Name);
            if (dws) {
            
               dws->set_widget (widget);
            }
         }
      }
   }
   else if (Mode == PluginDiscoverRemove) {

      QtWidget *w = QtWidget::cast (PluginPtr);

      if (w) {

         const String Name = w->get_qt_widget_name ();
         
         if (Name == _mainWidgetName) {
            
            setCentralWidget (0);
            
            QWidget *widget = w->get_qt_widget ();
            if (widget) { widget->setParent (0); }
         }
         else {
            
            DockWidgetStruct *dws = _dockWidgetTable.remove (Name);
            if (dws && (dws->widget == w->get_qt_widget ())) {

               remove_menu_action (_windowMenuName, dws->dock->toggleViewAction ());
               dws->remove_from (this);
               dws->set_widget (0);
            }
         }
      }
   }
}


// QtModuleMainWindow Interface
QString
dmz::QtModuleMainWindowBasic::get_window_name () { return _windowName; }


QMainWindow *
dmz::QtModuleMainWindowBasic::get_qt_main_window () { return this; }


QMenu *
dmz::QtModuleMainWindowBasic::lookup_menu (const String &Text) {

   QMenu *menu (_menuTable.lookup (Text));
   
   if (!menu) {
      
      menu = menuBar ()->addMenu (Text.get_buffer ());
      if (!_menuTable.store (Text, menu)) { delete menu; menu = 0; }
   }
   
   return menu;
}


void
dmz::QtModuleMainWindowBasic::add_menu_action (const String &MenuName, QAction *action) {
   
   if (MenuName && action) {
      
      QMenu *menu (lookup_menu (MenuName));
      if (menu) { menu->addAction (action); }
   }
}


void
dmz::QtModuleMainWindowBasic::remove_menu_action (const String &MenuName, QAction *action) {

   QMenu *menu (_menuTable.lookup (MenuName));
   if (menu) { menu->removeAction (action); }
}


// QtWidget Interface
QWidget *
dmz::QtModuleMainWindowBasic::get_qt_widget () { return this; }


void
dmz::QtModuleMainWindowBasic::closeEvent (QCloseEvent *event) {

   _exit.request_exit (dmz::ExitStatusNormal, get_plugin_name () + " Closed");
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

   Config geometry;

   if (session.lookup_config ("geometry", geometry)) {

      restoreGeometry (config_to_qbytearray (geometry));
   }
   else {

      QRect grect = QApplication::desktop ()->availableGeometry (this);
      move(grect.center () - rect ().center ());
   }
}


void
dmz::QtModuleMainWindowBasic::_restore_state () {

   Config session (
      get_session_config (get_plugin_name (), get_plugin_runtime_context ()));

   QByteArray stateData (
      config_to_qbytearray ("state", session, saveState (LocalSessionVersion)));

   restoreState (stateData, LocalSessionVersion);
}


void
dmz::QtModuleMainWindowBasic::_init_dock_windows (Config &local) {

   Config dockList;

   if (local.lookup_all_config ("dock-windows", dockList)) {

      ConfigIterator it;
      Config dock;

      while (dockList.get_next_config (it, dock)) {

         Config groupList;

         if (dock.lookup_all_config ("group", groupList)) {

            ConfigIterator wit;
            Config group;

            while (groupList.get_next_config (wit, group)) {

               Config widgetList;

               Qt::DockWidgetArea area = Qt::NoDockWidgetArea;

               String AreaName (config_to_string ("area", group).get_lower ());

               if (AreaName == "left") { area = Qt::LeftDockWidgetArea; }
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

                     if (!_dockWidgetTable.lookup (WidgetName)) {

                        DockWidgetStruct *dws = new DockWidgetStruct (WidgetName, widget, area);

                        if (dws) {

                           if (!_dockWidgetTable.store (WidgetName, dws)) {
                              
                              delete dws; dws = 0;
                           }
                        }
                     }
                     else {

                        _log.error << "Widget: " << WidgetName
                           << " has already been mapped to the main window." << endl;
                     }
                  }
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

   setUnifiedTitleAndToolBarOnMac (config_to_boolean (
      "showUnifiedTitleAndToolBar.value", local, False));

   set_qwidget_stylesheet ("stylesheet", local, this);

   setCorner (
      Qt::TopRightCorner,
      config_to_boolean ("corners.top.right", local, False) ?
         Qt::TopDockWidgetArea :
         Qt::RightDockWidgetArea);

   setCorner (
      Qt::TopLeftCorner,
      config_to_boolean ("corners.top.left", local, False) ?
         Qt::TopDockWidgetArea :
         Qt::LeftDockWidgetArea);

   setCorner (
      Qt::BottomRightCorner,
      config_to_boolean ("corners.bottom.right", local, False) ?
         Qt::BottomDockWidgetArea :
         Qt::RightDockWidgetArea);

   setCorner (
      Qt::BottomLeftCorner,
      config_to_boolean ("corners.bottom.left", local, False) ?
         Qt::BottomDockWidgetArea :
         Qt::LeftDockWidgetArea);

   _mainWidgetName = config_to_string ("central-widget.name", local);

   Config menuList;
   if (local.lookup_config ("menu-bar", menuList)) {
      
      ConfigIterator it;
      Config menu;
      
      while (menuList.get_next_config (it, menu)) {

         const String Name (config_to_string ("text", menu));
         if (Name) { lookup_menu (Name); }
      }
   }
   
   _exitAction = new QAction (this);
   qaction_config_read ("exit-action", local, _exitAction);
   connect (_exitAction, SIGNAL (triggered ()), this, SLOT (close ()));
   
   if (_exitAction->text ().isEmpty ()) {
      
      _exitAction->setText ("E&xit"); 
      _exitAction->setShortcut (QKeySequence ("Ctrl+Q"));
      _exitAction->setStatusTip ("Exit the application");
   }

   const String FileMenu (config_to_string ("file-menu.text", local, "&File"));
   add_menu_action (FileMenu, _exitAction);
   
   _windowMenuName = config_to_string ("window-menu.text", local, _windowMenuName);
         
   _init_dock_windows (local);
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
