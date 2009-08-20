#include <dmzInputEventMasks.h>
#include <dmzInputModule.h>
#include <dmzInputConsts.h>
#include <dmzQtConfigRead.h>
#include "dmzQtPluginStackedWidget.h"
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <QtGui/QtGui>


dmz::QtPluginStackedWidget::QtPluginStackedWidget (const PluginInfo &Info, Config &local) :
      Plugin (Info),
      InputObserverUtil (Info, local),
      QtWidget (Info),
      _log (Info),
      _parent (0),
      _stack (0) {

   _init (local);
}


dmz::QtPluginStackedWidget::~QtPluginStackedWidget () {

   _widgetTable.clear ();
   _channelTable.empty ();
   
   if (_parent) { delete _parent; _parent = 0; }
}


// Plugin Interface
void
dmz::QtPluginStackedWidget::update_plugin_state (
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
dmz::QtPluginStackedWidget::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   QtWidget *w = QtWidget::cast (PluginPtr);

   if (w && _stack) {
      
      const String WidgetName (w->get_qt_widget_name ());
      ChannelStruct *cs (_widgetTable.lookup (WidgetName));
   
      if (cs) {
      
         if (Mode == PluginDiscoverAdd) {

            cs->widget = w->get_qt_widget ();
            if (cs->widget) { _stack->addWidget (cs->widget); }
         }
         else if (Mode == PluginDiscoverRemove) {

            QWidget *widget (w->get_qt_widget ());
            
            if (widget && (cs->widget == widget)) {
               
               widget->setParent (0);
               
               _stack->removeWidget (widget);
               _widgetTable.remove (WidgetName);
               cs->widget = 0;
            }
         }
      }
   }
}


// Input Observer Interface
void
dmz::QtPluginStackedWidget::update_channel_state (const Handle Channel, const Boolean State) {

   ChannelStruct *cs (_channelTable.lookup (Channel));

   if (cs && _stack) {

      if (State) { cs->count++; }
      else { cs->count--; }

      if (cs->count) {

         _stack->setCurrentWidget (cs->widget);
      }
   }
}


// QtWidget Interface
QWidget *
dmz::QtPluginStackedWidget::get_qt_widget () {

   return _parent;
}


void
dmz::QtPluginStackedWidget::_init_input_channels (Config &local) {

   Config channelList;

   if (local.lookup_all_config ("channel", channelList)) {

      ConfigIterator it;
      Config channel;

      while (channelList.get_next_config (it, channel)) {

         const String ChannelName (
            config_to_string ("name", channel, InputChannelDefaultName));

         const Handle ChannelHandle (
            activate_input_channel (ChannelName, InputEventChannelStateMask));
            
         ChannelStruct *cs (_channelTable.lookup (ChannelHandle));

         if (!cs) {

            cs = new ChannelStruct (ChannelHandle);

            if (cs && _channelTable.store (cs->Channel, cs)) {
               
               const String WidgetName (config_to_string ("widget", channel));

               if (WidgetName) { _widgetTable.store (WidgetName, cs); }
            }
            else { delete cs; cs = 0; }
         }
      }
   }
}


void
dmz::QtPluginStackedWidget::_init (Config &local) {

   _parent = new QFrame;
   _parent->setObjectName (get_plugin_name ().get_buffer ());
   
   qframe_config_read ("", local, _parent);

   _stack = new QStackedLayout;
   
   // QWidget *widget (new QWidget (_parent));
   // _stack->addWidget (widget);

   _init_input_channels (local);
   
   QVBoxLayout *mainLayout = new QVBoxLayout;
   mainLayout->addLayout (_stack);
   _parent->setLayout (mainLayout);
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzQtPluginStackedWidget (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::QtPluginStackedWidget (Info, local);
}

};
