#include "dmzExPluginInput.h"
#include <dmzInputEventController.h>
#include <dmzInputEventKey.h>
#include <dmzInputEventMasks.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>


dmz::ExPluginInput::ExPluginInput (const PluginInfo &Info, Config &local) :
      Plugin (Info),
      InputObserverUtil (Info, local),
      _log (Info) {

   _init (local);
}


dmz::ExPluginInput::~ExPluginInput () {

}


// Plugin Interface
void
dmz::ExPluginInput::discover_plugin (const Plugin *PluginPtr) {

}


void
dmz::ExPluginInput::start_plugin () {

}


void
dmz::ExPluginInput::stop_plugin () {

}


void
dmz::ExPluginInput::shutdown_plugin () {

}


void
dmz::ExPluginInput::remove_plugin (const Plugin *PluginPtr) {

}


// Input Observer Interface
void
dmz::ExPluginInput::update_channel_state (const UInt32 Channel, const Boolean State) {

}


void
dmz::ExPluginInput::receive_axis_event (
   const UInt32 Channel,
   const InputEventAxis &Value) {

   _log.out << "[receive_axis_event] channel: " << Channel
            << " source: " << Value.get_source_handle ()
            << " axis: " << Value.get_axis_id ()
            << " value: " << Value.get_axis_value () << endl;
}


void
dmz::ExPluginInput::receive_button_event (
   const UInt32 Channel,
   const InputEventButton &Value) {

   _log.out << "[receive_button_event] channel: " << Channel
            << " source: " << Value.get_source_handle ()
            << " button: " << Value.get_button_id ()
            << " value: " << (Value.get_button_value () ? "Down" : "Up") << endl;
}


void
dmz::ExPluginInput::receive_key_event (
      const UInt32 Channel,
      const InputEventKey &Value) {

   _log.out << "[receive_key_event] channel: " << Channel
            << " source: " << Value.get_source_handle ()
            << " key: " << Value.get_key ()
            << " state: " << (Value.get_key_state () ? "True" : "False") << endl;
}


void
dmz::ExPluginInput::_init (Config &local) {

   init_input_channels (
      local,
      InputEventAxisMask |
      InputEventButtonMask |
      InputEventKeyMask |
      InputEventChannelStateMask,
      &_log);
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzExPluginInput (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::ExPluginInput (Info, local);
}

};
