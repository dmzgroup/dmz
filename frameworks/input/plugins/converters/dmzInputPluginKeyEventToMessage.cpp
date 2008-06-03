#include "dmzInputPluginKeyEventToMessage.h"
#include <dmzInputEventKey.h>
#include <dmzInputEventMasks.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigRead.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>

/*!

\class dmz::InputPluginKeyEventToMessage
\ingroup Input
\brief Converts key events to messages.
\details The Plugin sends messages when a key is pressed or released.
The XML format:
\code
<dmz>
<dmzInputPluginKeyEventToMessage>
   <!--
      Multiple channels may be defined.
      Attribute: "name" specifies the channel name.
   -->
   <channel name="ChannelName">
      <!--
         Multiple converters may be defined.
         Attribute: "key" specifies the which key to monitor.
      -->
      <converter key="r">
         <!--
            Attribute: "source" specifies the name of the source of the event.
            Attribute: "name" specifies the name of the target of the message.
            Attribute: "message" specifies the name of the message to send.
            Attribute: "pressed" is a boolean specifying if the message should
            be sent when the key is pressed or released. Default value is true.
         -->
         <target
            source="Source Name"
            name="Target Name"
            message="Message Name"
            pressed="boolean value"
          />
      </converter>
   </channel>
</dmzInputPluginKeyEventToMessage>
</dmz>
\endcode
\sa dmz::string_to_key_value() for the names of keys that may be used.
*/

//! \cond
dmz::InputPluginKeyEventToMessage::InputPluginKeyEventToMessage (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      InputObserverUtil (Info, local),
      _log (Info) {

   _init (local);
}


dmz::InputPluginKeyEventToMessage::~InputPluginKeyEventToMessage () {

   _channelTable.empty ();
}


// Plugin Interface
void
dmz::InputPluginKeyEventToMessage::discover_plugin (const Plugin *PluginPtr) {

}


void
dmz::InputPluginKeyEventToMessage::start_plugin () {

}


void
dmz::InputPluginKeyEventToMessage::stop_plugin () {

}


void
dmz::InputPluginKeyEventToMessage::shutdown_plugin () {

}


void
dmz::InputPluginKeyEventToMessage::remove_plugin (const Plugin *PluginPtr) {

}


// Input Observer Interface
void
dmz::InputPluginKeyEventToMessage::update_channel_state (
      const Handle Channel,
      const Boolean State) {

}


void
dmz::InputPluginKeyEventToMessage::receive_key_event (
      const Handle Channel,
      const InputEventKey &Value) {

   ChannelStruct *cs (_channelTable.lookup (Channel));

   if (cs) {

      KeyStruct *ks (cs->table.lookup (Value.get_key ()));

      if (ks) {

         const Handle Source (Value.get_source_handle ());
         const Boolean Pressed (Value.get_key_state ());

         TargetStruct *current (ks->targets);

         while (current) {

            if ((!current->Source || (current->Source == Source)) &&
                  (current->Pressed == Pressed)) {

               current->Message.send_message (current->Target, 0, 0);
            }

            current = current->next;
         }
      }
   }
}


void
dmz::InputPluginKeyEventToMessage::_create_targets (
      Config &config,
      KeyStruct &ks) {

   ConfigIterator it;
   Config target;

   TargetStruct *current (0);

   while (config.get_next_config (it, target)) {
 
      RuntimeContext *context (get_plugin_runtime_context ());

      const Handle Source (config_to_named_handle ("source", target, context));
      const Handle Target (config_to_named_handle ("name", target, context));
      const MessageType Message (
         config_create_message_type ("message", target, "", context));
      const Boolean Pressed (config_to_boolean ("pressed", target, True));

      if (Message) {

         TargetStruct *next (new TargetStruct (Source, Target, Message, Pressed));

         if (current) { current->next = next; current = next; }
         else { ks.targets = current = next; }
      }
   }
}


void
dmz::InputPluginKeyEventToMessage::_create_key_converters (
      Config &config,
      ChannelStruct &cs) {

   Config converterList;

   if (config.lookup_all_config ("converter", converterList)) {

      ConfigIterator it;
      Config converter;

      while (converterList.get_next_config (it, converter)) {

         const UInt32 Key (config_to_key_value (converter, &_log));

         if (Key) {

            KeyStruct *ks (cs.table.lookup (Key));

            if(!ks) {

               ks = new KeyStruct;

               if (ks && !cs.table.store (Key, ks)) { delete ks; ks = 0; }
            }

            if (ks) {

               Config targetList;

               if (converter.lookup_all_config ("target", targetList)) {

                  _create_targets (targetList, *ks);
               }
            }
         }
      }
   }
}


void
dmz::InputPluginKeyEventToMessage::_init (Config &local) {

   Config channelList;

   if (local.lookup_all_config ("channel", channelList)) {

      ConfigIterator it;
      Config channel;

      while (channelList.get_next_config (it, channel)) {

         const String ChannelName (config_to_string ("name", channel));

         const Handle ChannelHandle (ChannelName ?
            activate_input_channel (ChannelName, InputEventKeyMask) :
            activate_default_input_channel (InputEventKeyMask));

         ChannelStruct *cs (_channelTable.lookup (ChannelHandle));

         if (!cs) {

            cs = new ChannelStruct;

            if (cs && !_channelTable.store (ChannelHandle, cs)) { delete cs; cs = 0; }
         }

         if (cs) { _create_key_converters (channel, *cs); }
      }
   }
}
//! \endcond


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzInputPluginKeyEventToMessage (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::InputPluginKeyEventToMessage (Info, local);
}

};
