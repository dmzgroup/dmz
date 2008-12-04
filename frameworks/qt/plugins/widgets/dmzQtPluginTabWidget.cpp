#include "dmzQtPluginTabWidget.h"
#include <dmzQtConfigRead.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>

#include <QtGui/QGridLayout>


dmz::QtPluginTabWidget::QtPluginTabWidget (const PluginInfo &Info, Config &local) :
      Plugin (Info),
      QtWidget (Info),
      _log (Info),
      _parent (0),
      _tab (0) {

   _init (local);
}


dmz::QtPluginTabWidget::~QtPluginTabWidget () {

   _widgetTable.empty ();
   _tab = 0; // will be auto deleted by _parent
   if (_parent) { delete _parent; _parent = 0; }
}


// Plugin Interface
void
dmz::QtPluginTabWidget::update_plugin_state (
      const PluginStateEnum State,
      const UInt32 Level) {

   if (State == PluginStateInit) {

      if (_tab) {

         HashTableStringIterator it;
         WidgetStruct *ws (_widgetTable.get_first (it));

         while (ws) {

            if (ws->widget) { _tab->addTab (ws->widget, ws->title.get_buffer ()); }
            ws = _widgetTable.get_next (it);
         }
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
dmz::QtPluginTabWidget::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   QtWidget *w = QtWidget::cast (PluginPtr);

   if (w) {

      const String WidgetName (w->get_qt_widget_name ());
      WidgetStruct *ws (_widgetTable.lookup (WidgetName));

      if (ws) {

         if (Mode == PluginDiscoverAdd) {

            ws->widget = w->get_qt_widget ();

            if (!ws->title) { ws->title = WidgetName; }
         }
         else if (Mode == PluginDiscoverRemove) {

            QWidget *widget (w->get_qt_widget ());

            if (widget && (ws->widget == widget)) {

               if (_tab) {

                  _tab->removeTab (_tab->indexOf (widget));
               }

               widget->setParent (0);
               ws->widget = 0;
            }
         }
      }
   }
}


QWidget *
dmz::QtPluginTabWidget::get_qt_widget () {

   return _parent;
}


void
dmz::QtPluginTabWidget::_init (Config &local) {

   _parent = new QWidget;

   QGridLayout *layout = new QGridLayout (_parent);

   _tab = new QTabWidget (_parent);

   _tab->setMinimumSize (config_to_qsize ("minimum-size", local, _tab->minimumSize ()));
   _tab->setMaximumSize (config_to_qsize ("maximum-size", local, _tab->maximumSize ()));

   layout->addWidget (_tab);

   Config widgetList;

   if (local.lookup_all_config ("widget", widgetList)) {

      ConfigIterator it;
      Config widget;

      while (widgetList.get_next_config (it, widget)) {

         const String WidgetName (config_to_string ("name", widget));

         if (WidgetName && !_widgetTable.lookup (WidgetName)) {

            WidgetStruct *ws (new WidgetStruct);
            if (ws) { ws->title = config_to_string ("title", widget); }

            if (!_widgetTable.store (WidgetName, ws)) { delete ws; ws = 0; }
         }
      }
   }
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzQtPluginTabWidget (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::QtPluginTabWidget (Info, local);
}

};
