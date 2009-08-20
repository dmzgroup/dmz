#ifndef DMZ_QT_PLUGIN_STACKED_WIDGET_DOT_H
#define DMZ_QT_PLUGIN_STACKED_WIDGET_DOT_H

#include <dmzInputObserverUtil.h>
#include <dmzQtWidget.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzTypesHashTableHandleTemplate.h>
#include <dmzTypesHashTableStringTemplate.h>

class QFrame;
class QStackedLayout;
class QWidget;


namespace dmz {

   class QtPluginStackedWidget :
         public Plugin,
         public InputObserverUtil,
         public QtWidget {

      public:
         QtPluginStackedWidget (const PluginInfo &Info, Config &local);
         ~QtPluginStackedWidget ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level);

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

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
            
         // QtWidget Interface
         virtual QWidget *get_qt_widget ();

      protected:
         struct ChannelStruct {

            const Handle Channel;
            Int32 count;
            QWidget *widget;

            ChannelStruct (const Handle TheChannel) :
               Channel (TheChannel),
               count (0),
               widget (0) {;}
         };

         void _init_input_channels (Config &local);
         void _init (Config &local);
         
         Log _log;
         QFrame *_parent;
         QStackedLayout *_stack;
         HashTableHandleTemplate<ChannelStruct> _channelTable;
         HashTableStringTemplate<ChannelStruct> _widgetTable;

      private:
         QtPluginStackedWidget ();
         QtPluginStackedWidget (const QtPluginStackedWidget &);
         QtPluginStackedWidget &operator= (const QtPluginStackedWidget &);
   };
};


#endif // DMZ_QT_PLUGIN_STACKED_WIDGET_DOT_H
