#ifndef DMZ_AUDIO_PLUGIN_EVENT_DOT_H
#define DMZ_AUDIO_PLUGIN_EVENT_DOT_H

#include <dmzEventObserverUtil.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>

namespace dmz {

   class AudioPluginEvent :
         public Plugin,
         public EventObserverUtil {

      public:
         AudioPluginEvent (const PluginInfo &Info, Config &local);
         ~AudioPluginEvent ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level) {;}

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr) {;}

         // Event Observer Interface
         virtual void start_event (
            const Handle EventHandle,
            const EventType &Type,
            const EventLocalityEnum Locality);

         virtual void end_event (
            const Handle EventHandle,
            const EventType &Type,
            const EventLocalityEnum Locality);

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
         void _init (Config &local);

         Log _log;

      private:
         AudioPluginEvent ();
         AudioPluginEvent (const AudioPluginEvent &);
         AudioPluginEvent &operator= (const AudioPluginEvent &);

   };
};

#endif // DMZ_AUDIO_PLUGIN_EVENT_DOT_H
