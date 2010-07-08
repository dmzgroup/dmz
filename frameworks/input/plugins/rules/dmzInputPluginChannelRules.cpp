#include <dmzInputEventMasks.h>
#include <dmzInputModule.h>
#include "dmzInputPluginChannelRules.h"
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzRuntimeConfig.h>

dmz::InputPluginChannelRules::InputPluginChannelRules (const PluginInfo &Info, Config &local) :
      Plugin (Info),
      InputObserverUtil (Info, local),
      _log (Info),
      _channelList (0),
      _defaultChannel (0),
      _inputModule (0),
      __activeChannelCount (0) {

   _init (local);
}


dmz::InputPluginChannelRules::~InputPluginChannelRules () {

}


// Plugin Interface
void
dmz::InputPluginChannelRules::update_plugin_state (
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
dmz::InputPluginChannelRules::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      if (!_inputModule) {

         _inputModule = InputModule::cast (PluginPtr, _inputModuleName);

         if (_inputModule) {

            ChannelStruct *current (_channelList);

            while (current) {

               _inputModule->create_channel (current->Channel);

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

   }
}


// Input Observer Interface
void
dmz::InputPluginChannelRules::update_channel_state (const Handle Channel, const Boolean State) {

   if (State) { ++__activeChannelCount; }
   else { --__activeChannelCount; }

   if (__activeChannelCount == 0 && _defaultChannel) {
      _inputModule->set_channel_state (_defaultChannel,True);
   }

   else if (_channelList && _inputModule && State) {

      ChannelStruct *current (_channelList);

      while (current) {

         if (current->Channel != Channel) {
            _inputModule->set_channel_state (current->Channel,False);
         }

         current = current->next;
      }
   }
}


void
dmz::InputPluginChannelRules::receive_axis_event (
      const Handle Channel,
      const InputEventAxis &Value) {

}


void
dmz::InputPluginChannelRules::receive_button_event (
      const Handle Channel,
      const InputEventButton &Value) {

}


void
dmz::InputPluginChannelRules::receive_switch_event (
      const Handle Channel,
      const InputEventSwitch &Value) {

}


void
dmz::InputPluginChannelRules::receive_key_event (
      const Handle Channel,
      const InputEventKey &Value) {

}


void
dmz::InputPluginChannelRules::receive_mouse_event (
      const Handle Channel,
      const InputEventMouse &Value) {

}


void
dmz::InputPluginChannelRules::receive_data_event (
      const Handle Channel,
      const Handle Source,
      const Data &Value) {

}


// InputPluginChannelRules Interface
void
dmz::InputPluginChannelRules::_init (Config &local) {

   Definitions defs (get_plugin_runtime_context (), &_log);

   _inputModuleName = config_to_string ("module.input.name", local);

   Config channelList;
   if (local.lookup_all_config ("channel", channelList)) {

      ConfigIterator it;
      Config cd;

      ChannelStruct *current (0);
      while (channelList.get_next_config (it, cd)) {

         const String Name (config_to_string ("name", cd));
         if (Name) {

            ChannelStruct *next (new ChannelStruct (defs.create_named_handle (Name)));

            if (next) {

               if (!_defaultChannel || config_to_boolean ("default", cd, False)) {
                  _defaultChannel = next->Channel;
               }

               if (current) { current->next = next; current = next; }
               else { _channelList = current = next; }
            }
         }
      }
   }
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzInputPluginChannelRules (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::InputPluginChannelRules (Info, local);
}

};
