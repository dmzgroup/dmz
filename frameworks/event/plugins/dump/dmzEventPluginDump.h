#ifndef DMZ_EVENT_PLUGIN_DUMP_DOT_H
#define DMZ_EVENT_PLUGIN_DUMP_DOT_H

#include <dmzEventDump.h>
#include <dmzEventObserverUtil.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimeEventType.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>

namespace dmz {

   class EventPluginDump :
         public Plugin,
         public EventObserverUtil,
         public EventDump {

      public:
         EventPluginDump (const PluginInfo &Info, Config &local);
         ~EventPluginDump ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level);

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // Event Observer Interface
         virtual void close_event (
            const Handle EventHandle,
            const EventType &Type,
            const EventLocalityEnum Locality);

         // Event Dump Interface
         virtual void start_dump_event (
            const Handle EventHandle,
            const EventType &Type,
            const EventLocalityEnum Locality);

         virtual void end_dump_event (const Handle EventHandle);

         virtual void store_event_object_handle (
            const Handle EventHandle,
            const Handle AttributeHandle,
            const Handle Value);

         virtual void store_event_object_type (
            const Handle EventHandle,
            const Handle AttributeHandle,
            const ObjectType &Value);

         virtual void store_event_state (
            const Handle EventHandle,
            const Handle AttributeHandle,
            const Mask &Value);

         virtual void store_event_time_stamp (
            const Handle EventHandle,
            const Handle AttributeHandle,
            const Float64 &Value);

         virtual void store_event_position (
            const Handle EventHandle,
            const Handle AttributeHandle,
            const Vector &Value);

         virtual void store_event_orientation (
            const Handle EventHandle,
            const Handle AttributeHandle,
            const Matrix &Value);

         virtual void store_event_velocity (
            const Handle EventHandle,
            const Handle AttributeHandle,
            const Vector &Value);

         virtual void store_event_acceleration (
            const Handle EventHandle,
            const Handle AttributeHandle,
            const Vector &Value);

         virtual void store_event_scale (
            const Handle EventHandle,
            const Handle AttributeHandle,
            const Vector &Value);

         virtual void store_event_vector (
            const Handle EventHandle,
            const Handle AttributeHandle,
            const Vector &Value);

         virtual void store_event_scalar (
            const Handle EventHandle,
            const Handle AttributeHandle,
            const Float64 Value);

         virtual void store_event_text (
            const Handle EventHandle,
            const Handle AttributeHandle,
            const String &Value);

         virtual void store_event_data (
            const Handle EventHandle,
            const Handle AttributeHandle,
            const Data &Value);

      protected:
         String _get_attr_name (const Handle AttributeHandle);
         void _init (Config &local);

         Log _log;

         Definitions _defs;

         Handle _defaultHandle;

         EventType _rootEventType;

      private:
         EventPluginDump ();
         EventPluginDump (const EventPluginDump &);
         EventPluginDump &operator= (const EventPluginDump &);

   };
};

#endif // DMZ_EVENT_PLUGIN_DUMP_DOT_H
