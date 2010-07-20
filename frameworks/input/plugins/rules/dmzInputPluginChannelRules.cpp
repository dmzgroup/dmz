#include <dmzInputEventMasks.h>
#include <dmzInputModule.h>
#include "dmzInputPluginChannelRules.h"
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzRuntimeConfig.h>
#include <dmzTypesDeleteListTemplate.h>

/*!

\class dmz::InputPluginChannelRules
\ingroup Input
\brief Keeps only one channel in a list active at a time.
\details
If a channel in the list becomes active, every other channel in the list that was active will be deactivated. This ensures only one channel in the list is ever active. If all channels in the list become deactivated, the default channel will be activated.
\code
<scope>
   <channel name="String" default="Boolean"/>
   <channel name="String" default="Boolean"/>
   <!-- ... -->
   <channel name="String" default="Boolean"/>
</scope>
\endcode

- \b channel.name A string containing the channels name
- \b channel.default A boolean indicated if the channel is the default in the list. The last channel set as default will be the default channel if multiple channels are set to true.

*/

//! \cond
dmz::InputPluginChannelRules::InputPluginChannelRules (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      InputObserverUtil (Info, local),
      _log (Info),
      _active (0),
      _channelList (0),
      _defaultChannel (0),
      _input (0) {

   _init (local);
}


dmz::InputPluginChannelRules::~InputPluginChannelRules () {

   delete_list (_channelList);
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

      if (!_input) {

         _input = InputModule::cast (PluginPtr, _inputModuleName);

         if (_input) {

            ChannelStruct *current (_channelList);

            while (current) {

               _input->create_channel (current->Channel);

               _input->register_input_observer (
                     current->Channel,
                     InputEventChannelStateMask,
                     *this);

               current = current->next;
            }

            _input->set_channel_state (_defaultChannel, True);
         }
      }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_input && (_input == InputModule::cast (PluginPtr, _inputModuleName))) {

         _input = 0;
      }
   }
}


// Input Observer Interface
void
dmz::InputPluginChannelRules::update_channel_state (
      const Handle Channel,
      const Boolean State) {

   _active += (State ? 1 : -1);

   if (_active == 0 && _defaultChannel && _input) {

      _input->set_channel_state (_defaultChannel, True);
   }

   else if (_channelList && _input && State) {

      ChannelStruct *current (_channelList);

      while (current) {

         if (current->Channel != Channel) {

            _input->set_channel_state (current->Channel, False);
         }

         current = current->next;
      }
   }
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

               if (config_to_boolean ("default", cd, False)) {

                  _defaultChannel = next->Channel;
               }

               if (current) { current->next = next; current = next; }
               else { _channelList = current = next; }
            }
         }
      }
   }
}
//! \endcond


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzInputPluginChannelRules (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::InputPluginChannelRules (Info, local);
}

};
