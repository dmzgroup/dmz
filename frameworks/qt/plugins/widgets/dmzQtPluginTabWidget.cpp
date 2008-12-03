#include "dmzQtPluginTabWidget.h"
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>

dmz::QtPluginTabWidget::QtPluginTabWidget (const PluginInfo &Info, Config &local) :
      Plugin (Info),
      QtWidget (Info),
      _log (Info),
      _tab (0) {

   _init (local);
}


dmz::QtPluginTabWidget::~QtPluginTabWidget () {

   _widgetTable.empty ();
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

   return _tab;
}


void
dmz::QtPluginTabWidget::_init (Config &local) {

   _tab = new QTabWidget;
   _tab->setMinimumSize (300, 250);;

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
