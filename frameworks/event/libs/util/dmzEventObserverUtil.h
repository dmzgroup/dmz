#ifndef DMZ_EVENT_OBSERVER_UTIL_DOT_H
#define DMZ_EVENT_OBSERVER_UTIL_DOT_H

#include <dmzEventUtilExport.h>
#include <dmzEventObserver.h>
#include <dmzTypesMask.h>

namespace dmz {

   class DMZ_EVENT_UTIL_LINK_SYMBOL EventObserverUtil : public EventObserver {

      public:
         Handle activate_event_attribute (
            const String &AttributeTypeName,
            const Mask &AttributeMask);

         Handle activate_default_event_attribute (const Mask &AttributeMask);

         void deactivate_event_attribute (
            const String &AttributeTypeName,
            const Mask &AttributeMask);

         void deactivate_event_attribute (
            const Handle Handle,
            const Mask &AttributeMask);

         void deactivate_default_event_attribute (const Mask &AttributeMask);

         virtual void store_event_module (const String &Name, EventModule &module);
         virtual void remove_event_module (const String &Name, EventModule &module);

         virtual void start_event (
            const Handle EventHandle,
            const EventType &Type,
            const EventLocalityEnum Locality);

         virtual void end_event (
            const Handle EventHandle,
            const EventType &Type,
            const EventLocalityEnum Locality);

         virtual void update_event_object_handle (
            const Handle EventHandle,
            const Handle AttributeHandle,
            const Handle Value);

         virtual void update_event_object_type (
            const Handle EventHandle,
            const Handle AttributeHandle,
            const ObjectType &Value);

         virtual void update_event_state (
            const Handle EventHandle,
            const Handle AttributeHandle,
            const Mask &Value);

         virtual void update_event_time_stamp (
            const Handle EventHandle,
            const Handle AttributeHandle,
            const Float64 &Value);

         virtual void update_event_position (
            const Handle EventHandle,
            const Handle AttributeHandle,
            const Vector &Value);

         virtual void update_event_orientation (
            const Handle EventHandle,
            const Handle AttributeHandle,
            const Matrix &Value);

         virtual void update_event_velocity (
            const Handle EventHandle,
            const Handle AttributeHandle,
            const Vector &Value);

         virtual void update_event_acceleration (
            const Handle EventHandle,
            const Handle AttributeHandle,
            const Vector &Value);

         virtual void update_event_scale (
            const Handle EventHandle,
            const Handle AttributeHandle,
            const Vector &Value);

         virtual void update_event_vector (
            const Handle EventHandle,
            const Handle AttributeHandle,
            const Vector &Value);

         virtual void update_event_scalar (
            const Handle EventHandle,
            const Handle AttributeHandle,
            const Float64 Value);

         virtual void update_event_text (
            const Handle EventHandle,
            const Handle AttributeHandle,
            const String &Value);

         virtual void update_event_data (
            const Handle EventHandle,
            const Handle AttributeHandle,
            const Data &Value);

      protected:
         EventObserverUtil (const PluginInfo &Info, const Config &Data);
         ~EventObserverUtil ();

      private:
         struct State;
         EventObserverUtil ();
         EventObserverUtil (const EventObserverUtil &);
         EventObserverUtil &operator= (const EventObserverUtil &);
         State &__state;
   };
};

#endif // DMZ_EVENT_OBSERVER_UTIL_DOT_H

