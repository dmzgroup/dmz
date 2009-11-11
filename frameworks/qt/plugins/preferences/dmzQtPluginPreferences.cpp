#include <dmzQtConfigRead.h>
#include <dmzQtModuleMainWindow.h>
#include "dmzQtPluginPreferences.h"
#include <dmzQtWidget.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <QtGui/QtGui>


dmz::QtPluginPreferences::QtPluginPreferences (const PluginInfo &Info, Config &local) :
      QFrame (0),
      Plugin (Info),
      _log (Info),
      _mainWindowModule (0),
      _mainWindowModuleName (),
      _preferencesAction (0),
      _menuName ("&Edit"),
      _widgetTable () {

   setWindowFlags (windowFlags () & ~Qt::WindowContextHelpButtonHint);

   _ui.setupUi (this);

   _init (local);
}


dmz::QtPluginPreferences::~QtPluginPreferences () {

   _widgetTable.empty ();
   if (_preferencesAction) { delete _preferencesAction; _preferencesAction = 0; }
}


// Plugin Interface
void
dmz::QtPluginPreferences::update_plugin_state (
      const PluginStateEnum State,
      const UInt32 Level) {

   if (State == PluginStateInit) {

      HashTableStringIterator it;
      WidgetStruct *ws (_widgetTable.get_first (it));
      
      while (ws) {

         if (ws->widget) {
            
            _ui.tabWidget->addTab (ws->widget, ws->title.get_buffer ());
         }

         ws = _widgetTable.get_next (it);
      }
   }
   else if (State == PluginStateStart) {

   }
   else if (State == PluginStateStop) {

   }
   else if (State == PluginStateShutdown) {

   }
}


void
dmz::QtPluginPreferences::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      if (!_mainWindowModule) {

         _mainWindowModule = QtModuleMainWindow::cast (PluginPtr, _mainWindowModuleName);
         
         if (_mainWindowModule && _preferencesAction) {
            
            setParent (_mainWindowModule->get_qt_main_window (), Qt::Dialog);
            _mainWindowModule->add_menu_action (_menuName, _preferencesAction);
         }
      }
      
      QtWidget *w = QtWidget::cast (PluginPtr);
      
      if (w) {
         
         const String WidgetName (w->get_qt_widget_name ());
         WidgetStruct *ws (_widgetTable.lookup (WidgetName));
         
         if (ws) {
            
            if (!ws->title) { ws->title = WidgetName; }
            
            QWidget *widget (w->get_qt_widget ());
            if (widget) { ws->widget = widget; }
         }
      }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_mainWindowModule &&
          (_mainWindowModule == QtModuleMainWindow::cast (PluginPtr))) {

         _mainWindowModule->remove_menu_action (_menuName, _preferencesAction);
         _preferencesAction->setParent (0);
         setParent (0);
         _mainWindowModule = 0;
      }
      
      QtWidget *w = QtWidget::cast (PluginPtr);
      
      if (w) {
         
         const String WidgetName (w->get_qt_widget_name ());
         WidgetStruct *ws (_widgetTable.lookup (WidgetName));
         
         if (ws) {
            
            QWidget *widget (w->get_qt_widget ());
            
            if (widget && (ws->widget == widget)) {
               
               _ui.tabWidget->removeTab (_ui.tabWidget->indexOf (widget));
               widget->setParent (0);
               ws->widget = 0;
            }
         }
      }      
   }
}


void
dmz::QtPluginPreferences::keyPressEvent (QKeyEvent *event) {

   switch (event->key()) {
      
      case Qt::Key_Escape:
         hide ();
         break;
         
      default:
         QFrame::keyPressEvent (event);
         break;
   }
}


void
dmz::QtPluginPreferences::_init (Config &local) {
   
   RuntimeContext *context (get_plugin_runtime_context ());
      
   setObjectName (get_plugin_name ().get_buffer ());
   
   qframe_config_read ("frame", local, this);
   
   _mainWindowModuleName = config_to_string (
      "module.mainWindow.name",
      local,
      "dmzQtModuleMainWindowBasic");
      
   _preferencesAction = new QAction (this),

   _preferencesAction->setMenuRole (QAction::PreferencesRole);

   _preferencesAction->setText (
      config_to_string ("preferences-menu.text", local, "&Preferences...").get_buffer ());

   connect (
      _preferencesAction, SIGNAL (triggered ()),
      this, SLOT (show ()));

   _menuName = config_to_string ("edit-menu.text", local, _menuName);
   
   _ui.tabWidget->removeTab (0);
   
   Config widgetList;

   if (local.lookup_all_config ("widget", widgetList)) {

      ConfigIterator it;
      Config widget;

      while (widgetList.get_next_config (it, widget)) {

         const String WidgetName (config_to_string ("name", widget));
         
         if (WidgetName && !_widgetTable.lookup (WidgetName)) {

            WidgetStruct *ws (new WidgetStruct);
            
            if (ws) {
               
               ws->title = config_to_string ("title", widget);
            }

            if (!_widgetTable.store (WidgetName, ws)) { delete ws; ws = 0; }
         }
      }
   }
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzQtPluginPreferences (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::QtPluginPreferences (Info, local);
}

};
