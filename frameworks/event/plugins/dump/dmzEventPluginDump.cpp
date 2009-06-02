#include <dmzEventCallbackMasks.h>
#include <dmzEventConsts.h>
#include <dmzEventModule.h>
#include "dmzEventPluginDump.h"
#include <dmzRuntimeObjectType.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>

/*!

\class dmz::EventPluginDump
\brief Prints event attribute values when an event is created.
\ingroup Event
\details The plugin is useful for debugging events. Any time an
event is created in the EventModule, that event is sent to the
plugin's Log. This class should not be create explicitly but instead should be loaded
as a Plugin by the runtime. This Plugin currently has no configuration parameters.

*/


//! \cond
dmz::EventPluginDump::EventPluginDump (const PluginInfo &Info, Config &local) :
      Plugin (Info),
      EventObserverUtil (Info, local),
      _log (Info),
      _defs (Info.get_context (), &_log),
      _defaultHandle (0) {

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
dmz::EventPluginDump::close_event (
      const Handle EventHandle,
      const EventType &Type,
      const EventLocalityEnum Locality) {

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
      << "Locality: " << lstr << endl << endl;
}


void
dmz::EventPluginDump::end_dump_event (const Handle EventHandle) {

   _log.out << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-" << endl;
}


void
dmz::EventPluginDump::store_event_handle (
      const Handle EventHandle,
      const Handle AttributeHandle,
      const Handle Value) {

   String txt ("[");
   const String HandleName = _get_attr_name (Value);
   if (HandleName) { txt << HandleName << "]"; }
   else { txt.flush (); }
   

   _log.out << _get_attr_name (AttributeHandle) << "Handle" << txt << ": " << Value
      << endl;
}


void
dmz::EventPluginDump::store_event_object_handle (
      const Handle EventHandle,
      const Handle AttributeHandle,
      const Handle Value) {

   _log.out << _get_attr_name (AttributeHandle) << "Object: " << Value << endl;
}


void
dmz::EventPluginDump::store_event_object_type (
      const Handle EventHandle,
      const Handle AttributeHandle,
      const ObjectType &Value) {

   _log.out << _get_attr_name (AttributeHandle) << "Object Type: " << Value.get_name ()
      << endl;
}


void
dmz::EventPluginDump::store_event_state (
      const Handle EventHandle,
      const Handle AttributeHandle,
      const Mask &Value) {

   String stateName;
   _defs.lookup_state_name (Value, stateName);
   _log.out << _get_attr_name (AttributeHandle) << "State: " << stateName << endl;
}


void
dmz::EventPluginDump::store_event_time_stamp (
      const Handle EventHandle,
      const Handle AttributeHandle,
      const Float64 &Value) {

   _log.out << _get_attr_name (AttributeHandle) << "Time Stamp: " << Value << endl;
}


void
dmz::EventPluginDump::store_event_position (
      const Handle EventHandle,
      const Handle AttributeHandle,
      const Vector &Value) {

   _log.out << _get_attr_name (AttributeHandle) << "Position: " << Value << endl;
}


void
dmz::EventPluginDump::store_event_orientation (
      const Handle EventHandle,
      const Handle AttributeHandle,
      const Matrix &Value) {

   _log.out << _get_attr_name (AttributeHandle) << "Orientation: " << Value << endl;
}


void
dmz::EventPluginDump::store_event_velocity (
      const Handle EventHandle,
      const Handle AttributeHandle,
      const Vector &Value) {

   _log.out << _get_attr_name (AttributeHandle) << "Velocity: " << Value << endl;
}


void
dmz::EventPluginDump::store_event_acceleration (
      const Handle EventHandle,
      const Handle AttributeHandle,
      const Vector &Value) {

   _log.out << _get_attr_name (AttributeHandle) << "Acceleration: " << Value << endl;
}


void
dmz::EventPluginDump::store_event_scale (
      const Handle EventHandle,
      const Handle AttributeHandle,
      const Vector &Value) {

   _log.out << _get_attr_name (AttributeHandle) << "Scale: " << Value << endl;
}


void
dmz::EventPluginDump::store_event_vector (
      const Handle EventHandle,
      const Handle AttributeHandle,
      const Vector &Value) {

   _log.out << _get_attr_name (AttributeHandle) << "Vector: " << Value << endl;
}


void
dmz::EventPluginDump::store_event_scalar (
      const Handle EventHandle,
      const Handle AttributeHandle,
      const Float64 Value) {

   _log.out << _get_attr_name (AttributeHandle) << "Scalar: " << Value << endl;
}


void
dmz::EventPluginDump::store_event_counter (
      const Handle EventHandle,
      const Handle AttributeHandle,
      const Int64 Value) {

   _log.out << _get_attr_name (AttributeHandle) << "Counter: " << Value << endl;
}


void
dmz::EventPluginDump::store_event_text (
      const Handle EventHandle,
      const Handle AttributeHandle,
      const String &Value) {

   _log.out << _get_attr_name (AttributeHandle) << "Text: " << Value << endl;
}


void
dmz::EventPluginDump::store_event_data (
      const Handle EventHandle,
      const Handle AttributeHandle,
      const Data &Value) {

   //_log.out << _get_attr_name (AttributeHandle) << "Data: " << Value << endl;
}


dmz::String
dmz::EventPluginDump::_get_attr_name (const Handle AttributeHandle) {

   String result;

   if (AttributeHandle != _defaultHandle) {

      result = _defs.lookup_named_handle_name (AttributeHandle);
      if (result) { result << ":"; }
   }

   return result;
}


void
dmz::EventPluginDump::_init (Config &local) {

   _defaultHandle = _defs.create_named_handle (EventAttributeDefaultName);
   _rootEventType = _defs.get_root_event_type ();
   activate_event_callback (_rootEventType, EventCloseMask);
}
//! \endcond

extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzEventPluginDump (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::EventPluginDump (Info, local);
}

};
