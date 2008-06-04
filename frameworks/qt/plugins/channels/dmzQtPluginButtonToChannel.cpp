#include <dmzInputModule.h>
#include <dmzQtModuleMainWindow.h>
#include "dmzQtPluginButtonToChannel.h"
#include <dmzQtUtil.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzRuntimeSession.h>
#include <QtGui/QtGui>


dmz::QtPluginButtonToChannel::QtPluginButtonToChannel (const PluginInfo &Info, Config &local) :
      QWidget (0),
      Plugin (Info),
      _log (Info),
      _defs (Info, &_log),
      _inputModule (0),
      _inputModuleName (),
      _mainWindowModule (0),
      _mainWindowModuleName (),
      _channel (0),
      _dockWidgetTitle (tr ("Channels")),
      _dock (0),
      _actionGroup (0),
      _channelList (0),
      _defaultChannel (0) {
   
   setObjectName (get_plugin_name ().get_buffer ());

   _init (local);
}


dmz::QtPluginButtonToChannel::~QtPluginButtonToChannel () {

   if (_channelList) { delete _channelList; _channelList = 0; }
}


// Plugin Interface
void
dmz::QtPluginButtonToChannel::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   const String PluginName (PluginPtr ? PluginPtr->get_plugin_name () : "");
   
   if (Mode == PluginDiscoverAdd) {
      
      if (!_inputModule) {

         _inputModule = InputModule::cast (PluginPtr, _inputModuleName);

         if (_inputModule) {

            _inputModule->create_channel (_defaultChannel);
            _inputModule->set_channel_state (_defaultChannel, True);

            ChannelStruct *current (_channelList);

            while (current) {

               _inputModule->create_channel (current->Channel);
               _inputModule->set_channel_state (current->Channel, False);

               current = current->next;
            }
         }
      }

      if (!_mainWindowModule) {

         _mainWindowModule = QtModuleMainWindow::cast (PluginPtr, _mainWindowModuleName);

         if (_mainWindowModule) {

            _dock = new QDockWidget (_dockWidgetTitle, this);
            _dock->setObjectName (get_plugin_name ().get_buffer ());
            _dock->setAllowedAreas (Qt::AllDockWidgetAreas);

         _dock->setFeatures (QDockWidget::NoDockWidgetFeatures);
//            QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);

            _mainWindowModule->add_dock_widget (_channel, Qt::RightDockWidgetArea, _dock);

            _dock->setWidget (this);
         }
      }
   }
   else if (Mode == PluginDiscoverRemove) {
      
      if (_inputModule && (_inputModule == InputModule::cast (PluginPtr))) {

         _inputModule = 0;
      }

      if (_mainWindowModule && (_mainWindowModule == QtModuleMainWindow::cast (PluginPtr))) {

         _mainWindowModule->remove_dock_widget (_channel, _dock);

         _mainWindowModule = 0;
      }
   }
}


void
dmz::QtPluginButtonToChannel::_slot_change_channel (QAction *theAction) {

   if (_inputModule && theAction) {
   
      if (theAction->isChecked ()) {
      
         foreach (QAction *action, _actionGroup->actions ()) {
      
            if (action != theAction) { action->setChecked (False); }
         }
         
         _inputModule->set_channel_state (_defaultChannel, False);
      }
      else { _inputModule->set_channel_state (_defaultChannel, True); }
      
      ChannelStruct *current (_channelList);
      
      while (current) {
      
         if (current->action) {
      
            _inputModule->set_channel_state (
               current->Channel,
               current->action->isChecked ());
         }
      
         current = current->next;
      }
   }
}


void
dmz::QtPluginButtonToChannel::_init (Config &local) {

   Definitions defs (get_plugin_runtime_context (), &_log);

   _inputModuleName = config_to_string ("module.input.name", local);
   _mainWindowModuleName = config_to_string ("module.mainWindow.name", local);

   _dockWidgetTitle = config_to_string (
      "dockWidget.title",
      local,
      qPrintable (_dockWidgetTitle)).get_buffer ();

   _channel = defs.create_named_handle (
      config_to_string ("channel.name", local, "NetworkAnalysisChannel"));
      
   const String DefaultName = config_to_string ("defaultChannel.name", local);

   _defaultChannel = defs.create_named_handle (DefaultName);

   qwidget_config_read ("widget", local, this);
   
   QHBoxLayout *layout (new QHBoxLayout ());
   
   Config buttonList;

   if (local.lookup_all_config_merged ("buttons", buttonList)) {
    
      ConfigIterator it;
      Config cd;

      _actionGroup = new QActionGroup (this);
      _actionGroup->setExclusive (False);
      
      connect (
         _actionGroup, SIGNAL (triggered (QAction *)),
         this, SLOT (_slot_change_channel (QAction *)));

      String value;
      ChannelStruct *current (0);

      while (buttonList.get_next_config (it, cd)) {
         
         const String Name (config_to_string ("channel", cd));

         if (Name) {
            
            ChannelStruct *next (new ChannelStruct (defs.create_named_handle (Name)));
            
            if (next) {

               QToolButton *button (new QToolButton (this));
               
               qtoolbutton_config_read ("", cd, button);
               
               next->action = button->defaultAction ();
               if (next->action) {
                  
                  next->action->setData ((quint64)next->Channel);
                  _actionGroup->addAction (next->action);
               }
                  
               layout->addWidget (button);
   
               if (current) { current->next = next; current = next; }
               else { _channelList = current = next; }
            }
         }
      }
   }
   
   layout->addStretch ();
   setLayout (layout);
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzQtPluginButtonToChannel (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::QtPluginButtonToChannel (Info, local);
}

};
