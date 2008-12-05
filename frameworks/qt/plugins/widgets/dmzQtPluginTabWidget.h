#ifndef DMZ_QT_PLUGIN_TAB_WIDGET_DOT_H
#define DMZ_QT_PLUGIN_TAB_WIDGET_DOT_H

#include <dmzQtWidget.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzTypesHashTableStringTemplate.h>

#include <QtGui/QTabWidget>

namespace dmz {

   class QtPluginTabWidget :
         public Plugin,
         public QtWidget {

      public:
         QtPluginTabWidget (const PluginInfo &Info, Config &local);
         ~QtPluginTabWidget ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level);

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // QtWidget Interface
         virtual QWidget *get_qt_widget ();

      protected:
         struct WidgetStruct {

            String title;
            QWidget *widget;

            WidgetStruct () : widget (0) {;}
         };

         void _init (Config &local);

         Log _log;

         QWidget *_parent;
         QTabWidget *_tab;

         HashTableStringTemplate<WidgetStruct> _widgetTable;

      private:
         QtPluginTabWidget ();
         QtPluginTabWidget (const QtPluginTabWidget &);
         QtPluginTabWidget &operator= (const QtPluginTabWidget &);

   };
};

#endif // DMZ_QT_PLUGIN_TAB_WIDGET_DOT_H
