
#include <dmzInputConsts.h>
#include <dmzInputEventController.h>
#include <dmzInputEventKey.h>
#include <dmzInputEventMouse.h>
#include <dmzInputModule.h>
#include "dmzArchivePluginInput.h"
#include <dmzRuntimeConfigRead.h>

#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzRuntimeObjectType.h>

//! \cond

struct dmz::ArchivePluginInput::ChannelState {

   Handle handle;
   Boolean active;
};


struct dmz::ArchivePluginInput::EventStruct {

   const Float64 Delay;
   EventStruct *next;

   ChannelState *channel;
   InputEventAxis *axisEvent;
   InputEventButton *buttonEvent;
   InputEventSwitch *switchEvent;
   InputEventKey *keyEvent;
   InputEventMouse *mouseEvent;
   Handle dataSource;
   Data *data;

   EventStruct (const Float64 TheDelay) :
      Delay (TheDelay),
      next (0),
      channel (0),
      axisEvent (0),
      buttonEvent (0),
      switchEvent (0),
      keyEvent (0),
      mouseEvent (0),
      dataSource (0),
      data (0) {;}

   ~EventStruct ();

   void send_event (InputModule *channels);
};


dmz::ArchivePluginInput::EventStruct::~EventStruct () {

   if (channel) { delete channel; channel = 0; }
   if (axisEvent) { delete axisEvent; axisEvent = 0; }
   if (buttonEvent) { delete buttonEvent; buttonEvent = 0; }
   if (switchEvent) { delete switchEvent; switchEvent = 0; }
   if (keyEvent) { delete keyEvent; keyEvent = 0; }
   if (mouseEvent) { delete mouseEvent; mouseEvent = 0; }
   if (data) { delete data; data = 0; }
}


void
dmz::ArchivePluginInput::EventStruct::send_event (InputModule *channels) {

   if (channels) {

      if (channel) { channels->set_channel_state (channel->handle, channel->active); }
      if (axisEvent) { channels->send_axis_event (*axisEvent); }
      if (buttonEvent) { channels->send_button_event (*buttonEvent); }
      if (switchEvent) { channels->send_switch_event (*switchEvent); }
      if (keyEvent) { channels->send_key_event (*keyEvent); }
      if (mouseEvent) { channels->send_mouse_event (*mouseEvent); }
      if (data) { channels->send_data_event (dataSource, *data); }
   }
}


dmz::ArchivePluginInput::ArchivePluginInput (
      const PluginInfo &Info, Config &local) :
      Plugin (Info),
      TimeSlice (Info, TimeSliceTypeRuntime, TimeSliceModeSingle, 0.0),
      _log (Info),
      _defs (Info, &_log),
      _channels (0),
      _eventList (0),
      _eventListTail (0),
      _activeChannel (0) {

   _init (local);
}


dmz::ArchivePluginInput::~ArchivePluginInput () {

   _destroy_events ();
}


// Plugin Interface
void
dmz::ArchivePluginInput::update_plugin_state (
      const PluginStateEnum State,
      const UInt32 Level) {

   if (State == PluginStateStart) {
      if (_eventList) {

         if (_channels) {

            EventStruct *event (_eventList);

            while (event) {

               if (event->channel) {

                  _channels->create_channel (event->channel->handle);
               }

               event = event->next;
            }
         }

         set_time_slice_interval (_eventList->Delay);
         start_time_slice ();
      }
   }
   else if (State == PluginStateShutdown) { _destroy_events (); }
}


void
dmz::ArchivePluginInput::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      if (!_channels) { _channels = InputModule::cast (PluginPtr); }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_channels && (_channels == InputModule::cast (PluginPtr))) {

         _destroy_events ();
         _channels = 0;
      }
   }
}


// TimeSlice Interface
void
dmz::ArchivePluginInput::update_time_slice (const Float64 DeltaTime) {

   if (_eventList) {

      EventStruct *event (_eventList);

      event->send_event (_channels);

      _eventList = _eventList->next;

      delete event; event = 0;

      if (_eventList) {

         set_time_slice_interval (_eventList->Delay);
         start_time_slice ();
      }
   }
}


void
dmz::ArchivePluginInput::_config_to_channel_state (Config &eventData) {

   String channelName (config_to_string ("channel", eventData, "default"));

   if (!channelName || channelName == "default") {

      channelName = InputChannelDefaultName;
   }

   const Boolean Active (config_to_boolean ("active", eventData, False));
   const Float64 Delay (config_to_float64 ("delay", eventData, 0.0));

   EventStruct *event = new EventStruct (Delay);
   event->channel = new ChannelState ();
   event->channel->handle = _defs.create_named_handle (channelName);
   event->channel->active = Active;

   _add_event (event);
}


void
dmz::ArchivePluginInput::_config_to_axis_event (Config &eventData) {

   const UInt32 Source (config_to_uint32 ("source", eventData, 0));
   const UInt32 Axis (config_to_uint32 ("axis", eventData, 0));
   const Float32 Value (config_to_float32 ("value", eventData, False));
   const Float64 Delay (config_to_float64 ("delay", eventData, 0.0));

   EventStruct *event = new EventStruct (Delay);
   event->axisEvent = new InputEventAxis ();
   event->axisEvent->set_source_handle (Source);
   event->axisEvent->set_axis_id (Axis);
   event->axisEvent->set_axis_value (Value);

   _add_event (event);
}


void
dmz::ArchivePluginInput::_config_to_button_event (Config &eventData) {

   const UInt32 Source (config_to_uint32 ("source", eventData, 0));
   const UInt32 Button (config_to_uint32 ("button", eventData, 0));
   const Boolean Value (config_to_boolean ("value", eventData, False));
   const Float64 Delay (config_to_float64 ("delay", eventData, 0.0));

   EventStruct *event = new EventStruct (Delay);
   event->buttonEvent = new InputEventButton ();
   event->buttonEvent->set_source_handle (Source);
   event->buttonEvent->set_button_id (Button);
   event->buttonEvent->set_button_value (Value);

   _add_event (event);
}


void
dmz::ArchivePluginInput::_config_to_key_event (Config &eventData) {

   const UInt32 Source (config_to_uint32 ("source", eventData, 0));
   const UInt32 Key (config_to_key_value (eventData, &_log));
   const Boolean Value (config_to_boolean ("value", eventData, False));
   const Float64 Delay (config_to_float64 ("delay", eventData, 0.0));

   EventStruct *event = new EventStruct (Delay);
   event->keyEvent = new InputEventKey ();
   event->keyEvent->set_source_handle (Source);
   event->keyEvent->set_key (Key);
   event->keyEvent->set_key_state (Value);

   _add_event (event);
}


void
dmz::ArchivePluginInput::_config_to_event (Config &eventData) {

   ConfigIterator it;
   String value;

   if (eventData.lookup_attribute ("channel", value)) {

      _config_to_channel_state (eventData);
   }
   else if (eventData.lookup_attribute ("axis", value)) {

      _config_to_axis_event (eventData);
   }
   else if (eventData.lookup_attribute ("button", value)) {

      _config_to_button_event (eventData);
   }
   else if (eventData.lookup_attribute ("switch", value)) {

//      _config_to_switch_event (eventData);
   }
   else if (eventData.lookup_attribute ("key", value)) {

      _config_to_key_event (eventData);
   }
   else if (eventData.lookup_attribute ("mouse", value)) {

//      _config_to_mouse_event (eventData);
   }
   else if (eventData.lookup_attribute ("data", value)) {

//      _config_to_data_event (eventData);
   }
   else {

      _log.warn <<
         "Failed to convert config data to an input event: unknown input type" << endl;
   }
}


void
dmz::ArchivePluginInput::_add_event (EventStruct *event) {

   if (event) {

      if (_eventListTail) {

         _eventListTail->next = event;
         _eventListTail = event;
      }
      else { _eventList = _eventListTail = event; }
   }
}


void
dmz::ArchivePluginInput::_destroy_events () {

   while (_eventList) {

      EventStruct *event (_eventList);

      _eventList = _eventList->next;

      delete event; event = 0;
   }

   _eventListTail = 0;
}


void
dmz::ArchivePluginInput::_init (Config &local) {

   Config eventList;

   if (local.lookup_all_config ("input", eventList)) {

      Config eventData;

      ConfigIterator it;

      Boolean found (eventList.get_first_config (it, eventData));

      while (found) {

         _config_to_event (eventData);
         found = eventList.get_next_config (it, eventData);
      }
   }
}

//! \endcond

extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzArchivePluginInput (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::ArchivePluginInput (Info, local);
}

};
