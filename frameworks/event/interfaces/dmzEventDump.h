#ifndef DMZ_EVENT_DUMP_DOT_H
#define DMZ_EVENT_DUMP_DOT_H

#include <dmzEventConsts.h>
#include <dmzTypesBase.h>
#include <dmzTypesString.h>

namespace dmz {

   class Data;
   class Mask;
   class EventType;
   class Matrix;
   class ObjectType;
   class Vector;

   class EventDump {

      public:
         virtual void start_dump_event (
            const Handle EventHandle,
            const EventType &Type,
            const EventLocalityEnum Locality) = 0;

         virtual void end_dump_event (const Handle EventHandle) = 0;

         virtual void store_event_object_handle (
            const Handle EventHandle,
            const Handle AttributeHandle,
            const Handle Value) = 0;

         virtual void store_event_object_type (
            const Handle EventHandle,
            const Handle AttributeHandle,
            const ObjectType &Value) = 0;

         virtual void store_event_state (
            const Handle EventHandle,
            const Handle AttributeHandle,
            const Mask &Value) = 0;

         virtual void store_event_time_stamp (
            const Handle EventHandle,
            const Handle AttributeHandle,
            const Float64 &Value) = 0;

         virtual void store_event_position (
            const Handle EventHandle,
            const Handle AttributeHandle,
            const Vector &Value) = 0;

         virtual void store_event_orientation (
            const Handle EventHandle,
            const Handle AttributeHandle,
            const Matrix &Value) = 0;

         virtual void store_event_velocity (
            const Handle EventHandle,
            const Handle AttributeHandle,
            const Vector &Value) = 0;

         virtual void store_event_acceleration (
            const Handle EventHandle,
            const Handle AttributeHandle,
            const Vector &Value) = 0;

         virtual void store_event_scale (
            const Handle EventHandle,
            const Handle AttributeHandle,
            const Vector &Value) = 0;

         virtual void store_event_vector (
            const Handle EventHandle,
            const Handle AttributeHandle,
            const Vector &Value) = 0;

         virtual void store_event_scalar (
            const Handle EventHandle,
            const Handle AttributeHandle,
            const Float64 Value) = 0;

         virtual void store_event_text (
            const Handle EventHandle,
            const Handle AttributeHandle,
            const String &Value) = 0;

         virtual void store_event_data (
            const Handle EventHandle,
            const Handle AttributeHandle,
            const Data &Value) = 0;

      protected:
         EventDump () {;}
         ~EventDump () {;}

      private:
         EventDump (const EventDump &);
         EventDump &operator= (const EventDump &);
   };
};

#endif // DMZ_EVENT_DUMP_DOT_H
