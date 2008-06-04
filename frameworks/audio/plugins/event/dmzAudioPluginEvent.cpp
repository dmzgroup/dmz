#include "dmzAudioPluginEvent.h"
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>

dmz::AudioPluginEvent::AudioPluginEvent (const PluginInfo &Info, Config &local) :
      Plugin (Info),
      EventObserverUtil (Info, local),
      _log (Info) {

   _init (local);
}


dmz::AudioPluginEvent::~AudioPluginEvent () {

}


// Event Observer Interface
void
dmz::AudioPluginEvent::start_event (
      const Handle EventHandle,
      const EventType &Type,
      const EventLocalityEnum Locality) {

}


void
dmz::AudioPluginEvent::end_event (
      const Handle EventHandle,
      const EventType &Type,
      const EventLocalityEnum Locality) {

}


void
dmz::AudioPluginEvent::store_event_object_handle (
      const Handle EventHandle,
      const Handle AttributeHandle,
      const Handle Value) {

}


void
dmz::AudioPluginEvent::store_event_object_type (
      const Handle EventHandle,
      const Handle AttributeHandle,
      const ObjectType &Value) {

}


void
dmz::AudioPluginEvent::store_event_state (
      const Handle EventHandle,
      const Handle AttributeHandle,
      const Mask &Value) {

}


void
dmz::AudioPluginEvent::store_event_time_stamp (
      const Handle EventHandle,
      const Handle AttributeHandle,
      const Float64 &Value) {

}


void
dmz::AudioPluginEvent::store_event_position (
      const Handle EventHandle,
      const Handle AttributeHandle,
      const Vector &Value) {

}


void
dmz::AudioPluginEvent::store_event_orientation (
      const Handle EventHandle,
      const Handle AttributeHandle,
      const Matrix &Value) {

}


void
dmz::AudioPluginEvent::store_event_velocity (
      const Handle EventHandle,
      const Handle AttributeHandle,
      const Vector &Value) {

}


void
dmz::AudioPluginEvent::store_event_acceleration (
      const Handle EventHandle,
      const Handle AttributeHandle,
      const Vector &Value) {

}


void
dmz::AudioPluginEvent::store_event_scale (
      const Handle EventHandle,
      const Handle AttributeHandle,
      const Vector &Value) {

}


void
dmz::AudioPluginEvent::store_event_vector (
      const Handle EventHandle,
      const Handle AttributeHandle,
      const Vector &Value) {

}


void
dmz::AudioPluginEvent::store_event_scalar (
      const Handle EventHandle,
      const Handle AttributeHandle,
      const Float64 Value) {

}


void
dmz::AudioPluginEvent::store_event_text (
      const Handle EventHandle,
      const Handle AttributeHandle,
      const String &Value) {

}


void
dmz::AudioPluginEvent::store_event_data (
      const Handle EventHandle,
      const Handle AttributeHandle,
      const Data &Value) {

}


void
dmz::AudioPluginEvent::_init (Config &local) {

}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzAudioPluginEvent (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::AudioPluginEvent (Info, local);
}

};
