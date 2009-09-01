#include <dmzQtModuleMainWindow.h>
#include "dmzQtPluginVersion.h"
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>

dmz::QtPluginVersion::QtPluginVersion (
      const PluginInfo &Info,
      Config &local,
      Config &global) :
      Plugin (Info),
      _global (global),
      _log (Info),
      _version (0),
      _aboutAction (0) {

   _init (local);
}


dmz::QtPluginVersion::~QtPluginVersion () {

   if (_version) { delete _version; _version = 0; }
}


// Plugin Interface
void
dmz::QtPluginVersion::update_plugin_state (
      const PluginStateEnum State,
      const UInt32 Level) {

   if (State == PluginStateInit) {

   }
   else if (State == PluginStateStart) {

   }
   else if (State == PluginStateStop) {

   }
   else if (State == PluginStateShutdown) {

   }
}


void
dmz::QtPluginVersion::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   const String AboutMenu ("About");

   if (Mode == PluginDiscoverAdd) {

      QtModuleMainWindow *window (QtModuleMainWindow::cast (PluginPtr));

      if (window && _aboutAction) {

         _version = new QtVersion (window->get_qt_main_window (), _global);
         window->add_menu_action (AboutMenu, _aboutAction);
      } 
   }
   else if (Mode == PluginDiscoverRemove) {

      QtModuleMainWindow *window (QtModuleMainWindow::cast (PluginPtr));

      if (window) {
      
         window->remove_menu_action (AboutMenu, _aboutAction);
         _version->setParent (0);
      }
   }
}


void
dmz::QtPluginVersion::_slot_about () { _version->exec (); }


void
dmz::QtPluginVersion::_init (Config &local) {

   _aboutAction = new QAction (this);

   if (_aboutAction) {

      _aboutAction->setMenuRole (QAction::AboutRole);

      connect (
         _aboutAction, SIGNAL (triggered ()),
         this, SLOT (_slot_about ()));
   }
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzQtPluginVersion (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::QtPluginVersion (Info, local, global);
}

};
