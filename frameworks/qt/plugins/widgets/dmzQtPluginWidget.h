#ifndef DMZ_QT_PLUGIN_WIDGET_DOT_H
#define DMZ_QT_PLUGIN_WIDGET_DOT_H

#include <dmzQtWidget.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzTypesHashTableStringTemplate.h>

#include <QtGui/QWidget>

namespace dmz {

   class QtPluginWidget :
         public Plugin,
         public QtWidget {

      public:
         QtPluginWidget (const PluginInfo &Info, Config &local);
         ~QtPluginWidget ();

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
         QLayout *_layout;

         HashTableStringTemplate<WidgetStruct> _widgetTable;

      private:
         QtPluginWidget ();
         QtPluginWidget (const QtPluginWidget &);
         QtPluginWidget &operator= (const QtPluginWidget &);

   };
};

#endif // DMZ_QT_PLUGIN_WIDGET_DOT_H
