#include "dmzQtModuleMainWindowBasic.h"
#include <dmzQtUtil.h>
#include <dmzQtWidget.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzRuntimeSession.h>
#include <QtGui/QtGui>
#include <QtCore/QDebug>


class QMainWindodw;


namespace {

static const int LocalSessionVersion = 6;

};


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
      _menuBar (0),
      _windowMenuName ("&Window"),
      _fileMenuName ("&File"),
      _fixedSize (False) {

   setObjectName (get_plugin_name ().get_buffer ());

   _ui.setupUi (this);

#if 0
   // Not working.
   _menuBar = new QMenuBar;
   _menuBar->setObjectName (objectName () + QLatin1String ("MainMenu"));
   setMenuBar (_menuBar);
#else
   _menuBar = menuBar ();
#endif

   _init (local);
}


dmz::QtModuleMainWindowBasic::~QtModuleMainWindowBasic () {

   _dockTable.clear ();
}


// Plugin Interface
void
dmz::QtModuleMainWindowBasic::update_plugin_state (
      const PluginStateEnum State,
      const UInt32 Level) {

   if (State == PluginStateInit) {

      QAction *action (new QAction (this));
      action->setSeparator (True);
      add_menu_action (_fileMenuName, action);
      add_menu_action (_fileMenuName, _exitAction);
   }
   else if (State == PluginStateStart) {

      _load_session ();
      _restore_state ();

      show ();
      raise ();
      activateWindow ();
   }
   else if (State == PluginStateStop) {

      _save_session ();

      HashTableStringIterator it;
      QDockWidget *dock (_dockTable.get_first (it));
      while (dock) {

         dock->hide ();
         dock = _dockTable.get_next (it);
      }

      close ();
   }
   else if (State == PluginStateShutdown) {

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

            QDockWidget *dock = lookup_dock_widget (Name);
            if (dock && widget) { dock->setWidget (widget); }
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

            if (lookup_dock_widget (Name)) { remove_dock_widget (Name); }
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

      menu = _menuBar->addMenu (Text.get_buffer ());
      if (!_menuTable.store (Text, menu)) { delete menu; menu = 0; }
   }

   return menu;
}


void
dmz::QtModuleMainWindowBasic::add_menu_separator (const String &MenuName) {

   if (MenuName) {

      QMenu *menu (lookup_menu (MenuName));
      if (menu) { menu->addSeparator (); }
   }
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


QDockWidget *
dmz::QtModuleMainWindowBasic::create_dock_widget (
      const String &DockName,
      const Qt::DockWidgetArea Area,
      QWidget *widget) {

   QDockWidget *dock (0);

   if (DockName && !_dockTable.lookup (DockName)) {

      dock = new QDockWidget (DockName.get_buffer (), this);
      dock->setObjectName (DockName.get_buffer ());

      if (_dockTable.store (DockName, dock)) {

         if (widget) {

            widget->setParent (dock);
            dock->setWidget (widget);
         }

         addDockWidget (Area, dock);
         add_menu_action (_windowMenuName, dock->toggleViewAction ());
      }
      else { delete dock; dock = 0; }
   }

   return dock;
}


QDockWidget *
dmz::QtModuleMainWindowBasic::lookup_dock_widget (const String &DockName) {

   return _dockTable.lookup (DockName);
}


dmz::Boolean
dmz::QtModuleMainWindowBasic::remove_dock_widget (const String &DockName) {

   Boolean result (False);
   QDockWidget *dock = _dockTable.remove (DockName);
   if (dock) {

      remove_menu_action (_windowMenuName, dock->toggleViewAction ());
      removeDockWidget (dock);

      QWidget *widget (dock->widget ());
      if (widget) { widget->setParent (0); }

      dock->setWidget (0);
      dock->setParent (0);

      delete dock; dock = 0;

      result = True;
   }

   return result;
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

   Config session (get_plugin_name ());
   session.add_config (qbytearray_to_config ("geometry", saveGeometry ()));
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

      QDockWidget *dock (0);
      foreach (dock, _docksToShow) { dock->setVisible (True); }
      foreach (dock, _docksToFloat) { dock->setFloating (True); }

      _docksToShow.clear ();
      _docksToFloat.clear ();

      QRect grect = QApplication::desktop ()->availableGeometry (this);
      move(grect.center () - rect ().center ());

      HashTableStringIterator it;
      dock = _dockTable.get_first (it);
      while (dock) {

         dock->move (grect.center () - dock->rect ().center ());
         dock = _dockTable.get_next (it);
      }
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
               const String AreaName (config_to_string ("area", group).get_lower ());

               if (AreaName == "left") { area = Qt::LeftDockWidgetArea; }
               else if (AreaName == "right") { area = Qt::RightDockWidgetArea; }
               else if (AreaName == "top") { area = Qt::TopDockWidgetArea; }
               else if (AreaName == "bottom") { area = Qt::BottomDockWidgetArea; }
               else { _log.error << "Unknown widget area: " << AreaName << endl; }

               if (group.lookup_all_config ("widget", widgetList)) {

                  ConfigIterator it;
                  Config widget;

                  while (widgetList.get_next_config (it, widget)) {

                     const String WidgetName (config_to_string ("name", widget));

                     if (WidgetName && !_dockTable.lookup (WidgetName)) {

                        QDockWidget *dock = create_dock_widget (WidgetName, area, 0);
                        if (dock) {

                           const String Title (config_to_string ("title", widget, ""));
                           if (Title) { dock->setWindowTitle (Title.get_buffer ()); }

                           dock->setAllowedAreas (
                                 config_to_dock_widget_areas ("allowed-areas", widget, area));

                           dock->setFeatures (QDockWidget::AllDockWidgetFeatures);

                           dock->setVisible (False);

                           if (config_to_boolean ("visible", widget, True)) {

                              _docksToShow.append (dock);
                           }

                           if (config_to_boolean ("floating", widget, False)) {

                              _docksToFloat.append (dock);
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

   _fileMenuName = config_to_string ("file-menu.text", local, _fileMenuName);

   _windowMenuName = config_to_string ("window-menu.text", local, _windowMenuName);

   Config fixed;

   if (local.lookup_config ("fixed-size", fixed)) {

      _fixedSize = True;
      const int TheX = (int)config_to_int32 ("x", fixed, 800);
      const int TheY = (int)config_to_int32 ("y", fixed, 600);
      setFixedSize (TheX, TheY);
      statusBar ()->setSizeGripEnabled (false);
   }

   _init_dock_windows (local);

   statusBar ()->setHidden (config_to_boolean ("statusbar-hidden.value", local, False));
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
