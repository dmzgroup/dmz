#include <dmzInputEventMasks.h>
#include <dmzInputModule.h>
#include <dmzInputConsts.h>
#include "dmzQtPluginTabWidget.h"
#include <dmzQtConfigRead.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzRuntimeSession.h>
#include <QtGui/QGridLayout>


dmz::QtPluginTabWidget::QtPluginTabWidget (const PluginInfo &Info, Config &local) :
      QFrame (0),
      Plugin (Info),
      QtWidget (Info),
      InputObserverUtil (Info, local),
      _log (Info),
      _tab (0),
      _defaultTab (0),
      _saveToSession (True) {
   
   _init (local);
}


dmz::QtPluginTabWidget::~QtPluginTabWidget () {

   _widgetTable.empty ();
   _channelTable.clear ();
   _tab = 0; // will be auto deleted by _parent
}


// Plugin Interface
void
dmz::QtPluginTabWidget::update_plugin_state (
      const PluginStateEnum State,
      const UInt32 Level) {

   if (State == PluginStateInit) {
      
      if (_widgetFocus) {

         WidgetStruct *ws = _widgetTable.lookup (_widgetFocus);

         if (ws) { _tab->setCurrentWidget (ws->widget); }
      }
      else {

         Config session (get_session_config (
            get_plugin_name (),
            get_plugin_runtime_context ()));

         const Int32 Index = config_to_int32 ("tab-index.value", session, -1);

         if (_tab && Index >= 0) {

            _tab->setCurrentIndex ((int)Index);
         }
         else if (_tab && _defaultTab && _defaultTab->widget) {
         
            _tab->setCurrentWidget (_defaultTab->widget);
         }
      }
   }
   else if (State == PluginStateStart) {

   }
   else if (State == PluginStateStop) {

   }
   else if (State == PluginStateShutdown) {

      if (_tab && _saveToSession) {

         Config session (get_plugin_name ());
         Config data ("tab-index");
         data.store_attribute ("value", String::number (_tab->currentIndex ()));
         session.add_config (data);
         set_session_config (get_plugin_runtime_context (), session);
      }
   }
}


void
dmz::QtPluginTabWidget::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   QtWidget *w = QtWidget::cast (PluginPtr);

   if (w && _tab) {

      const String WidgetName (w->get_qt_widget_name ());
      WidgetStruct *ws (_widgetTable.lookup (WidgetName));

      if (ws) {

         if (Mode == PluginDiscoverAdd) {

            if (!ws->title) { ws->title = WidgetName; }

            QWidget *widget = w->get_qt_widget ();
            
            if (widget) {
               
               ws->widget = widget;
               
               _tab->addTab (ws->widget, ws->title.get_buffer ());
            }
         }
         else if (Mode == PluginDiscoverRemove) {

            QWidget *widget (w->get_qt_widget ());

            if (widget && (ws->widget == widget)) {

               _tab->removeTab (_tab->indexOf (widget));

               widget->setParent (0);
               ws->widget = 0;
            }
         }
      }
   }
}


// QtWidget Interface
QWidget *
dmz::QtPluginTabWidget::get_qt_widget () {

   return this;
}


// Input Observer Interface
void
dmz::QtPluginTabWidget::update_channel_state (const Handle Channel, const Boolean State) {

   WidgetStruct *ws (_channelTable.lookup (Channel));

   if (ws && _tab) {

      if (State) { ws->count++; }
      else { ws->count--; }

      if (ws->count) { _tab->setCurrentWidget (ws->widget); }
   }
}


void
dmz::QtPluginTabWidget::_slot_tab_changed (int index) {

   InputModule *input (get_input_module ());
   
   if (input && _tab) {

      HashTableHandleIterator it;
      WidgetStruct *ws (0);
      
      while (_channelTable.get_next (it, ws)) {

         if (ws->channel) {

            Boolean currentTab (False);

            if (ws->widget == _tab->widget (index)) { currentTab = True; }

            input->set_channel_state (ws->channel, currentTab);
         }
      }
   }
}


void
dmz::QtPluginTabWidget::_init (Config &local) {

   setObjectName (get_plugin_name ().get_buffer ());

   qframe_config_read ("frame", local, this);

   QVBoxLayout *layout = new QVBoxLayout;

   setLayout (layout);
   
   _tab = new QTabWidget (this);
   
   qwidget_config_read ("tab", local, _tab);
   
   connect (
      _tab, SIGNAL (currentChanged (int)),
      this, SLOT (_slot_tab_changed (int)));

   const String TabPosName (
      config_to_string ("position.value", local, "north").get_lower ());
   
   QTabWidget::TabPosition tabPos (QTabWidget::North);
   
   if (TabPosName == "north") { tabPos = QTabWidget::North; }
   else if (TabPosName == "south") { tabPos = QTabWidget::South; }
   else if (TabPosName == "west") { tabPos = QTabWidget::West; }
   else if (TabPosName == "east") { tabPos = QTabWidget::East; }
   
   _tab->setTabPosition (tabPos);
   
   //_tab->setDocumentMode (config_to_boolean ("document-mode.value", local, False));
   
   layout->addWidget (_tab);

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
               
               const String ChannelName (config_to_string ("channel", widget));

               if (ChannelName) {
                  
                  ws->channel = activate_input_channel (
                     ChannelName,
                     InputEventChannelStateMask);
               }
               
               if (config_to_boolean ("default", widget, False)) {
                  
                  if (!_defaultTab) { _defaultTab = ws; }
               }
            }

            if (_widgetTable.store (WidgetName, ws)) {
               
               if (ws->channel) { _channelTable.store (ws->channel, ws); }
            }
            else { delete ws; ws = 0; }
         }
      }
   }

   _widgetFocus = config_to_string ("focus.widget", local);
   
   _saveToSession = config_to_boolean ("remember-state.value", local, _saveToSession);
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
