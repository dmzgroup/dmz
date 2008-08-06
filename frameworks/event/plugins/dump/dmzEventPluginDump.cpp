#include <dmzEventCallbackMasks.h>
#include <dmzEventConsts.h>
#include <dmzEventModule.h>
#include "dmzEventPluginDump.h"
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>

dmz::EventPluginDump::EventPluginDump (const PluginInfo &Info, Config &local) :
      Plugin (Info),
      EventObserverUtil (Info, local),
      _log (Info),
      _defs (Info.get_context (), &_log) {

   _init (local);
}


dmz::EventPluginDump::~EventPluginDump () {

}


// Plugin Interface
void
dmz::EventPluginDump::update_plugin_state (
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
dmz::EventPluginDump::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

   }
   else if (Mode == PluginDiscoverRemove) {

   }
}


// Event Observer Interface
void
dmz::EventPluginDump::end_event (
      const Handle EventHandle,
      const EventType &Type) {

_log.warn << "End event!" << endl;

   EventModule *eventMod (get_event_module ());

   if (eventMod) { eventMod->dump_event (EventHandle, *this); }
}


// Event Dump Interface
void
dmz::EventPluginDump::start_dump_event (
      const Handle EventHandle,
      const EventType &Type,
      const EventLocalityEnum Locality) {

   String lstr ("Unknown");
   if (Locality == EventLocal) { lstr = "Local"; }
   else if (Locality == EventRemote) { lstr = "Remote"; }

   _log.out << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-" << endl
      << "    Type: " << Type.get_name () << endl
      << "  Handle: " << EventHandle << endl
      << "Locality: " << lstr << endl;
}


void
dmz::EventPluginDump::end_dump_event (const Handle EventHandle) {

   _log.out << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-" << endl;
}


void
dmz::EventPluginDump::store_event_object_handle (
      const Handle EventHandle,
      const Handle AttributeHandle,
      const Handle Value) {

}


void
dmz::EventPluginDump::store_event_object_type (
      const Handle EventHandle,
      const Handle AttributeHandle,
      const ObjectType &Value) {

}


void
dmz::EventPluginDump::store_event_state (
      const Handle EventHandle,
      const Handle AttributeHandle,
      const Mask &Value) {

}


void
dmz::EventPluginDump::store_event_time_stamp (
      const Handle EventHandle,
      const Handle AttributeHandle,
      const Float64 &Value) {

}


void
dmz::EventPluginDump::store_event_position (
      const Handle EventHandle,
      const Handle AttributeHandle,
      const Vector &Value) {

}


void
dmz::EventPluginDump::store_event_orientation (
      const Handle EventHandle,
      const Handle AttributeHandle,
      const Matrix &Value) {

}


void
dmz::EventPluginDump::store_event_velocity (
      const Handle EventHandle,
      const Handle AttributeHandle,
      const Vector &Value) {

}


void
dmz::EventPluginDump::store_event_acceleration (
      const Handle EventHandle,
      const Handle AttributeHandle,
      const Vector &Value) {

}


void
dmz::EventPluginDump::store_event_scale (
      const Handle EventHandle,
      const Handle AttributeHandle,
      const Vector &Value) {

}


void
dmz::EventPluginDump::store_event_vector (
      const Handle EventHandle,
      const Handle AttributeHandle,
      const Vector &Value) {

}


void
dmz::EventPluginDump::store_event_scalar (
      const Handle EventHandle,
      const Handle AttributeHandle,
      const Float64 Value) {

}


void
dmz::EventPluginDump::store_event_text (
      const Handle EventHandle,
      const Handle AttributeHandle,
      const String &Value) {

}


void
dmz::EventPluginDump::store_event_data (
      const Handle EventHandle,
      const Handle AttributeHandle,
      const Data &Value) {

}


void
dmz::EventPluginDump::_init (Config &local) {

   _launchEvent = activate_event_callback (EventLaunchName, EventEndMask);
   _detonationEvent = activate_event_callback (EventDetonationName, EventEndMask);
   _collisionEvent = activate_event_callback (EventCollisionName, EventEndMask);
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzEventPluginDump (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::EventPluginDump (Info, local);
}

};
