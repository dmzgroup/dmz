#include <dmzInputEventMasks.h>
#include <dmzInputModule.h>
#include <dmzQtModuleMainWindow.h>
#include "dmzQtPluginButtonToChannel.h"
#include <dmzQtUtil.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzRuntimeSession.h>
#include <QtGui/QtGui>


dmz::QtPluginButtonToChannel::QtPluginButtonToChannel (
      const PluginInfo &Info,
      Config &local) :
      QFrame (0),
      Plugin (Info),
      QtWidget (Info),
      InputObserverUtil (Info, local),
      _log (Info),
      _defs (Info, &_log),
      _inputModule (0),
      _inputModuleName (),
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

            ChannelStruct *current (_channelList);

            while (current) {

               _inputModule->create_channel (current->Channel);
               
//               _inputModule->set_channel_state (
//                  current->Channel, current->action->isChecked ());

               _inputModule->register_input_observer (
                     current->Channel,
                     InputEventChannelStateMask,
                     *this);

               current = current->next;
            }
         }
      }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_inputModule && (_inputModule == InputModule::cast (PluginPtr))) {

         _inputModule = 0;
      }
   }
}


// QtWidget Interface
QWidget *
dmz::QtPluginButtonToChannel::get_qt_widget () { return this; }


// Input Observer Interface

void
dmz::QtPluginButtonToChannel::update_channel_state (
      const Handle Channel,
      const Boolean State) {

   if (_channelList) {

      ChannelStruct *current (_channelList);

         while (current) {

            if (current->Channel == Channel) {

               current->action->setChecked (State);
               current = 0;

            }
            else { current = current->next; }
         }
   }
}


void
dmz::QtPluginButtonToChannel::_slot_change_channel (QAction *theAction) {

   if (_inputModule && theAction) {

      ChannelStruct *current (_channelList);

      while (current) {

         if (theAction == current->action) {

            _inputModule->set_channel_state (
               current->Channel,
               current->action->isChecked ());
            current = 0;
         }
         else { current = current->next; }
      }
   }
}


void
dmz::QtPluginButtonToChannel::_init (Config &local) {

   Definitions defs (get_plugin_runtime_context (), &_log);

   _inputModuleName = config_to_string ("module.input.name", local);

   qframe_config_read ("frame", local, this);

   QBoxLayout *layout;
   if (config_to_boolean ("layout.vertical", local, False)) {

      layout = new QVBoxLayout ();
   }
   else { layout = new QHBoxLayout (); }

   Config buttonList;
   if (local.lookup_all_config ("channel", buttonList)) {

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

         const String Name (config_to_string ("name", cd));
         if (Name) {

            ChannelStruct *next (new ChannelStruct (defs.create_named_handle (Name)));

            if (next) {

               QToolButton *button (new QToolButton (this));

               qtoolbutton_config_read ("toolButton", cd, button);

               next->action = button->defaultAction ();
               next->action->setChecked (False);

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
   
   QVBoxLayout *mainLayout = new QVBoxLayout;
      mainLayout->addLayout (layout);
   mainLayout->addStretch ();
   setLayout (mainLayout);
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
