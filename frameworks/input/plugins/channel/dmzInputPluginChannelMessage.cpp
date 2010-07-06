#include <dmzInputEventMasks.h>
#include "dmzInputPluginChannelMessage.h"
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigToNamedHandle.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>

dmz::InputPluginChannelMessage::InputPluginChannelMessage (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      InputObserverUtil (Info, local),
      _log (Info) {

   _init (local);
}


dmz::InputPluginChannelMessage::~InputPluginChannelMessage () {

   _channelTable.empty ();
}


// Plugin Interface
void
dmz::InputPluginChannelMessage::update_plugin_state (
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
dmz::InputPluginChannelMessage::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

   }
   else if (Mode == PluginDiscoverRemove) {

   }
}


// Input Observer Interface
void
dmz::InputPluginChannelMessage::update_channel_state (
      const Handle Channel,
      const Boolean State) {

   if (State) {

      ChannelStruct *cs = _channelTable.lookup (Channel);

      if (cs) { cs->Msg.send (cs->Target, cs->data); }
   }
}


// InputPluginChannelMessage Interface
void
dmz::InputPluginChannelMessage::_init (Config &local) {

   RuntimeContext *context = get_plugin_runtime_context ();

   Config list;

   if (local.lookup_all_config ("channel", list)) {

      ConfigIterator it;
      Config init;

      while (list.get_next_config (it, init)) {

         const String ChannelName = config_to_string ("name", init);

         const Handle Channel = activate_input_channel (
            ChannelName,
            InputEventChannelStateMask);

         if (Channel) {

            _log.info << "Activating channel: " << ChannelName << endl;

            const Handle Target = config_to_named_handle ("target", init, context);
            const Message Msg = config_create_message ("message", init, "", context);

            Data value (context);
            const Boolean FoundData = config_to_data (init, context, value, &_log);

            if (Msg) {

               ChannelStruct *cs =
                  new ChannelStruct (Target, Msg, FoundData ? &value : 0);

               if (cs && !_channelTable.store (Channel, cs)) {

                  delete cs; cs = 0;
               }
            }
            else {

               _log.error << "No message defined for channel: " << ChannelName << endl;
            }
         }
         else {

            _log.error << "No channel defined for channel name: " << ChannelName << endl;
         }
      }
   }
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzInputPluginChannelMessage (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::InputPluginChannelMessage (Info, local);
}

};
