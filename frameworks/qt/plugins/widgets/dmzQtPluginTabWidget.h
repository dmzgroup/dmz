#ifndef DMZ_QT_PLUGIN_TAB_WIDGET_DOT_H
#define DMZ_QT_PLUGIN_TAB_WIDGET_DOT_H

#include <dmzInputObserverUtil.h>
#include <dmzQtWidget.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzTypesHashTableHandleTemplate.h>
#include <dmzTypesHashTableStringTemplate.h>
#include <QtGui/QTabWidget>

class QFrame;


namespace dmz {

   class QtPluginTabWidget :
         public QObject,
         public Plugin,
         public QtWidget,
         public InputObserverUtil {
            
   Q_OBJECT

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
         
         // Input Observer Interface
         virtual void update_channel_state (const Handle Channel, const Boolean State);

         virtual void receive_axis_event (
            const Handle Channel,
            const InputEventAxis &Value) {;}

         virtual void receive_button_event (
            const Handle Channel,
            const InputEventButton &Value) {;}

         virtual void receive_switch_event (
            const Handle Channel,
            const InputEventSwitch &Value) {;}

         virtual void receive_key_event (
            const Handle Channel,
            const InputEventKey &Value) {;}

         virtual void receive_mouse_event (
            const Handle Channel,
            const InputEventMouse &Value) {;}

         virtual void receive_data_event (
            const Handle Channel,
            const Handle Source,
            const Data &Value) {;}

      protected slots:
         void _slot_tab_changed (int index);
         
      protected:
         struct WidgetStruct {

            String title;
            QWidget *widget;
            Handle channel;
            Int32 count;

            WidgetStruct () : widget (0), channel (0), count (0) {;}
         };

         void _init (Config &local);

         Log _log;

         String _widgetFocus;
         QFrame *_parent;
         QTabWidget *_tab;
         WidgetStruct *_defaultTab;

         HashTableStringTemplate<WidgetStruct> _widgetTable;
         HashTableHandleTemplate<WidgetStruct> _channelTable;

      private:
         QtPluginTabWidget ();
         QtPluginTabWidget (const QtPluginTabWidget &);
         QtPluginTabWidget &operator= (const QtPluginTabWidget &);

   };
};

#endif // DMZ_QT_PLUGIN_TAB_WIDGET_DOT_H
