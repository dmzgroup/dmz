#ifndef DMZ_EVENT_MODULE_DOT_H
#define DMZ_EVENT_MODULE_DOT_H

#include <dmzEventConsts.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzRuntimeRTTI.h>
#include <dmzTypesBase.h>
#include <dmzTypesString.h>

namespace dmz {

   //! \cond
   const char EventModuleInterfaceName[] = "EventModuleInterface";
   //! \endcond

   class Data;
   class EventDump;
   class Mask;
   class EventObserver;
   class EventType;
   class Matrix;
   class ObjectType;
   class RuntimeContext;
   class Vector;

   class EventModule {

      public:
         static EventModule *cast (
            const Plugin *PluginPtr,
            const String &PluginName = "");

         virtual Boolean register_event_observer (
            const EventType &Type,
            const Mask &CallbackMask,
            EventObserver &observer) = 0;

         virtual Boolean release_event_observer (
            const EventType &Type,
            const Mask &CallbackMask,
            EventObserver &observer) = 0;

         virtual Boolean release_event_observer_all (EventObserver &observer) = 0;

         virtual void dump_event (const Handle EventHandle, EventDump &dump) = 0;

         virtual Handle create_event (
            const EventType &Type,
            const EventLocalityEnum Locality) = 0;

         virtual Boolean close_event (const Handle EventHandle) = 0;

         virtual Boolean lookup_event_type (
            const Handle EventHandle,
            EventType &value) = 0;

         virtual EventLocalityEnum lookup_locality (const Handle EventHandle) = 0;

         virtual Boolean store_handle (
            const Handle EventHandle,
            const Handle AttributeHandle,
            const Handle Value) = 0;

         virtual Boolean lookup_handle (
            const Handle EventHandle,
            const Handle AttributeHandle,
            Handle &value) = 0;

         virtual Boolean store_object_handle (
            const Handle EventHandle,
            const Handle AttributeHandle,
            const Handle Value) = 0;

         virtual Boolean lookup_object_handle (
            const Handle EventHandle,
            const Handle AttributeHandle,
            Handle &value) = 0;

         virtual Boolean store_object_type (
            const Handle EventHandle,
            const Handle AttributeHandle,
            const ObjectType &Value) = 0;

         virtual Boolean lookup_object_type (
            const Handle EventHandle,
            const Handle AttributeHandle,
            ObjectType &value) = 0;

         virtual Boolean store_state (
            const Handle EventHandle,
            const Handle AttributeHandle,
            const Mask &Value) = 0;

         virtual Boolean lookup_state (
            const Handle EventHandle,
            const Handle AttributeHandle,
            Mask &value) = 0;

         virtual Boolean store_time_stamp (
            const Handle EventHandle,
            const Handle AttributeHandle,
            const Float64 &Value) = 0;

         virtual Boolean lookup_time_stamp (
            const Handle EventHandle,
            const Handle AttributeHandle,
            Float64 &value) = 0;

         virtual Boolean store_position (
            const Handle EventHandle,
            const Handle AttributeHandle,
            const Vector &Value) = 0;

         virtual Boolean lookup_position (
            const Handle EventHandle,
            const Handle AttributeHandle,
            Vector &value) = 0;

         virtual Boolean store_orientation (
            const Handle EventHandle,
            const Handle AttributeHandle,
            const Matrix &Value) = 0;

         virtual Boolean lookup_orientation (
            const Handle EventHandle,
            const Handle AttributeHandle,
            Matrix &value) = 0;

         virtual Boolean store_velocity (
            const Handle EventHandle,
            const Handle AttributeHandle,
            const Vector &Value) = 0;

         virtual Boolean lookup_velocity (
            const Handle EventHandle,
            const Handle AttributeHandle,
            Vector &value) = 0;

         virtual Boolean store_acceleration (
            const Handle EventHandle,
            const Handle AttributeHandle,
            const Vector &Value) = 0;

         virtual Boolean lookup_acceleration (
            const Handle EventHandle,
            const Handle AttributeHandle,
            Vector &value) = 0;

         virtual Boolean store_scale (
            const Handle EventHandle,
            const Handle AttributeHandle,
            const Vector &Value) = 0;

         virtual Boolean lookup_scale (
            const Handle EventHandle,
            const Handle AttributeHandle,
            Vector &value) = 0;

         virtual Boolean store_vector (
            const Handle EventHandle,
            const Handle AttributeHandle,
            const Vector &Value) = 0;

         virtual Boolean lookup_vector (
            const Handle EventHandle,
            const Handle AttributeHandle,
            Vector &value) = 0;

         virtual Boolean store_scalar (
            const Handle EventHandle,
            const Handle AttributeHandle,
            const Float64 Value) = 0;

         virtual Boolean lookup_scalar (
            const Handle EventHandle,
            const Handle AttributeHandle,
            Float64 &value) = 0;

         virtual Boolean store_counter (
            const Handle EventHandle,
            const Handle AttributeHandle,
            const Int64 Value) = 0;

         virtual Boolean lookup_counter (
            const Handle EventHandle,
            const Handle AttributeHandle,
            Int64 &value) = 0;

         virtual Boolean store_text (
            const Handle EventHandle,
            const Handle AttributeHandle,
            const String &Value) = 0;

         virtual Boolean lookup_text (
            const Handle EventHandle,
            const Handle AttributeHandle,
            String &value) = 0;

         virtual Boolean store_data (
            const Handle EventHandle,
            const Handle AttributeHandle,
            const Data &Value) = 0;

         virtual Boolean lookup_data (
            const Handle EventHandle,
            const Handle AttributeHandle,
            Data &value) = 0;

      protected:
         EventModule (const PluginInfo &Info);
         ~EventModule ();

      private:
         EventModule ();
         EventModule (const EventModule &);
         EventModule &operator= (const EventModule &);

         const PluginInfo &__Info;
   };
};


inline dmz::EventModule *
dmz::EventModule::cast (const Plugin *PluginPtr, const String &PluginName) {

   return  (EventModule *)lookup_rtti_interface (
      EventModuleInterfaceName,
      PluginName,
      PluginPtr);
}


inline
dmz::EventModule::EventModule (const PluginInfo &Info) :
      __Info (Info) {

   store_rtti_interface (EventModuleInterfaceName, __Info, (void *)this);
}


inline
dmz::EventModule::~EventModule () {

   remove_rtti_interface (EventModuleInterfaceName, __Info);
}

#endif // DMZ_EVENT_MODULE_DOT_H
