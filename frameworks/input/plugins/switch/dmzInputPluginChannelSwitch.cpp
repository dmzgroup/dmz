#include "dmzInputPluginChannelSwitch.h"
#include <dmzInputEventKey.h>
#include <dmzInputEventMasks.h>
#include <dmzInputModule.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigRead.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzTypesMask.h>

/*!

\class dmz::InputPluginChannelSwitch
\ingroup Input
\brief Maintains input channel states.
\details Ensures that only one channel in a given group is active. Will also
change channel state when a key event is received. The default key is "tab".

*/

//! \cond
dmz::InputPluginChannelSwitch::InputPluginChannelSwitch (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      InputObserverUtil (Info, local),
      _list (0),
      _current (0),
      _log (Info) {

   _init (local);
}


dmz::InputPluginChannelSwitch::~InputPluginChannelSwitch () {

   _current = 0;
   if (_list) { delete _list; _list = 0; }
}


// Plugin Interface
void
dmz::InputPluginChannelSwitch::discover_plugin (const Plugin *PluginPtr) {

}


void
dmz::InputPluginChannelSwitch::start_plugin () {
}


void
dmz::InputPluginChannelSwitch::stop_plugin () {

}


void
dmz::InputPluginChannelSwitch::shutdown_plugin () {

}


void
dmz::InputPluginChannelSwitch::remove_plugin (const Plugin *PluginPtr) {

}


// Input Observer Interface
void
dmz::InputPluginChannelSwitch::update_channel_state (
      const UInt32 Channel,
      const Boolean State) {

}


void
dmz::InputPluginChannelSwitch::receive_button_event (
      const UInt32 Channel,
      const InputEventButton &Value) {

}


void
dmz::InputPluginChannelSwitch::receive_key_event (
      const UInt32 Channel,
      const InputEventKey &Value) {

   if ((Value.get_key () == KeyTab) && Value.get_key_state ()) {

      if (_current) { _current = _current->next; }

      if (!_current) { _current = _list; }

      if (_current) {

         InputModule *input (get_input_module_channels ());

         if (input) {

            input->set_channel_state (_current->Channel, True);

            ChannelStruct *current (_list);

            while (current) {

               if (current->Channel != _current->Channel) {

                  input->set_channel_state (current->Channel, False);
               }

               current = current->next;
            }
         }
      }
   }
}


void
dmz::InputPluginChannelSwitch::_store_input_module (InputModule &module) {

   if (!_current) { _current = _list; }

   if (_current) {

      module.create_channel (_current->Channel);

      module.set_channel_state (_current->Channel, True);

      ChannelStruct *current (_list);

      while (current) {

         if (current->Channel != _current->Channel) {

            module.create_channel (current->Channel);
            module.set_channel_state (current->Channel, False);
         }

         current = current->next;
      }
   }
}


void
dmz::InputPluginChannelSwitch::_init (Config &local) {

   Config list;

   if (local.lookup_all_config ("switch.channel", list)) {

      Definitions defs (get_plugin_runtime_context (), &_log);

      const Mask EmptyMask;

      ConfigIterator it;

      Config data;

      ChannelStruct *current (0);

      Boolean found (list.get_first_config (it, data));

      while (found)  {

         const String Name = config_to_string ("name", data);

         if (Name) {

            ChannelStruct *next (new ChannelStruct (defs.create_named_handle (Name)));

            if (next) {

               if (current) { current->next = next; current = next; }
               else { _list = current = next; }
            }
         }

         found = list.get_next_config (it, data);
      }

      activate_default_input_channel (InputEventKeyMask);
   }
}
//! \endcond


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzInputPluginChannelSwitch (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::InputPluginChannelSwitch (Info, local);
}

};
