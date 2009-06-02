#include "dmzQtPluginWidget.h"
#include <dmzQtConfigRead.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>

#include <QtGui/QGridLayout>


dmz::QtPluginWidget::QtPluginWidget (const PluginInfo &Info, Config &local) :
      Plugin (Info),
      QtWidget (Info),
      _log (Info),
      _parent (0),
      _layout (0) {

   _init (local);
}


dmz::QtPluginWidget::~QtPluginWidget () {

   _widgetTable.empty ();
   if (_parent) { delete _parent; _parent = 0; }
}


// Plugin Interface
void
dmz::QtPluginWidget::update_plugin_state (
      const PluginStateEnum State,
      const UInt32 Level) {

   if (State == PluginStateInit) {

      if (_parent) {

         HashTableStringIterator it;
         WidgetStruct *ws (_widgetTable.get_first (it));

         while (ws) {

            if (ws->widget) {

               ws->widget->setParent (_parent);
               if (_layout) { _layout->addWidget (ws->widget); }
            }

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
dmz::QtPluginWidget::discover_plugin (
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

               if (_parent) {

                  // _parent->removeWidget (widget);
               }

               widget->setParent (0);
               if (_layout) { _layout->removeWidget (widget); }
               ws->widget = 0;
            }
         }
      }
   }
}


QWidget *
dmz::QtPluginWidget::get_qt_widget () {

   return _parent;
}


void
dmz::QtPluginWidget::_init (Config &local) {

   _parent = new QWidget;

   _layout = new QGridLayout (_parent);

   qwidget_config_read ("", local, _parent);

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
create_dmzQtPluginWidget (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::QtPluginWidget (Info, local);
}

};
