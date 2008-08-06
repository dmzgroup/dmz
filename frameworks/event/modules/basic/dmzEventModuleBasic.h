#ifndef DMZ_EVENT_MODULE_BASIC_DOT_H
#define DMZ_EVENT_MODULE_BASIC_DOT_H

#include <dmzEventModule.h>
#include <dmzEventObserver.h>
#include <dmzRuntimeEventType.h>
#include <dmzRuntimeHandle.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimeObjectType.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimeTimeSlice.h>
#include <dmzRuntimeTime.h>
#include <dmzTypesHashTableStringTemplate.h>
#include <dmzTypesHashTableHandleTemplate.h>
#include <dmzTypesMatrix.h>
#include <dmzTypesVector.h>

namespace dmz {

   typedef HashTableHandleTemplate<EventObserver> EventObserverStruct;

   class EventModuleBasic :
         public Plugin,
         public TimeSlice,
         public EventModule {

      public:
         EventModuleBasic (const PluginInfo &Info, Config &local);
         ~EventModuleBasic ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level) {;}

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr) {;}

         // TimeSlice Interface
         virtual void update_time_slice (const Float64 TimeDelta);

         // EventModule Interface
         virtual Boolean register_event_observer (
            const EventType &Type,
            const Mask &CallbackMask,
            EventObserver &observer);

         virtual Boolean release_event_observer (
            const EventType &Type,
            const Mask &CallbackMask,
            EventObserver &observer);

         virtual Boolean release_event_observer_all (EventObserver &Observer);

         virtual void dump_event (const Handle EventHandle, EventDump &dump);

         virtual Handle start_event (
            const EventType &Type,
            const EventLocalityEnum Locality);

         virtual Boolean end_event (const Handle EventHandle);

         virtual Boolean lookup_event_type (const Handle EventHandle, EventType &value);

         virtual EventLocalityEnum lookup_locality (const Handle EventHandle);

         virtual Boolean store_object_handle (
            const Handle EventHandle,
            const Handle AttributeHandle,
            const Handle Value);

         virtual Boolean lookup_object_handle (
            const Handle EventHandle,
            const Handle AttributeHandle,
            Handle &value);

         virtual Boolean store_object_type (
            const Handle EventHandle,
            const Handle AttributeHandle,
            const ObjectType &Value);

         virtual Boolean lookup_object_type (
            const Handle EventHandle,
            const Handle AttributeHandle,
            ObjectType &value);

         virtual Boolean store_state (
            const Handle EventHandle,
            const Handle AttributeHandle,
            const Mask &Value);

         virtual Boolean lookup_state (
            const Handle EventHandle,
            const Handle AttributeHandle,
            Mask &value);

         virtual Boolean store_time_stamp (
            const Handle EventHandle,
            const Handle AttributeHandle,
            const Float64 &Value);

         virtual Boolean lookup_time_stamp (
            const Handle EventHandle,
            const Handle AttributeHandle,
            Float64 &value);

         virtual Boolean store_position (
            const Handle EventHandle,
            const Handle AttributeHandle,
            const Vector &Value);

         virtual Boolean lookup_position (
            const Handle EventHandle,
            const Handle AttributeHandle,
            Vector &value);

         virtual Boolean store_orientation (
            const Handle EventHandle,
            const Handle AttributeHandle,
            const Matrix &Value);

         virtual Boolean lookup_orientation (
            const Handle EventHandle,
            const Handle AttributeHandle,
            Matrix &value);

         virtual Boolean store_velocity (
            const Handle EventHandle,
            const Handle AttributeHandle,
            const Vector &Value);

         virtual Boolean lookup_velocity (
            const Handle EventHandle,
            const Handle AttributeHandle,
            Vector &value);

         virtual Boolean store_acceleration (
            const Handle EventHandle,
            const Handle AttributeHandle,
            const Vector &Value);

         virtual Boolean lookup_acceleration (
            const Handle EventHandle,
            const Handle AttributeHandle,
            Vector &value);

         virtual Boolean store_scale (
            const Handle EventHandle,
            const Handle AttributeHandle,
            const Vector &Value);

         virtual Boolean lookup_scale (
            const Handle EventHandle,
            const Handle AttributeHandle,
            Vector &value);

         virtual Boolean store_vector (
            const Handle EventHandle,
            const Handle AttributeHandle,
            const Vector &Value);

         virtual Boolean lookup_vector (
            const Handle EventHandle,
            const Handle AttributeHandle,
            Vector &value);

         virtual Boolean store_scalar (
            const Handle EventHandle,
            const Handle AttributeHandle,
            const Float64 Value);

         virtual Boolean lookup_scalar (
            const Handle EventHandle,
            const Handle AttributeHandle,
            Float64 &value);

         virtual Boolean store_text (
            const Handle EventHandle,
            const Handle AttributeHandle,
            const String &Value);

         virtual Boolean lookup_text (
            const Handle EventHandle,
            const Handle AttributeHandle,
            String &value);

         virtual Boolean store_data (
            const Handle EventHandle,
            const Handle AttributeHandle,
            const Data &Value);

         virtual Boolean lookup_data (
            const Handle EventHandle,
            const Handle AttributeHandle,
            Data &value);

      protected:
         struct EventStruct {

            EventStruct *next;
            Handle handle;
            RuntimeHandle *handlePtr;
            Boolean closed;
            Float64 closeTime;
            EventType type;
            EventLocalityEnum locality;

            HashTableHandleTemplate<Handle> objectTable;
            HashTableHandleTemplate<ObjectType> typeTable;
            HashTableHandleTemplate<Mask> stateTable;
            HashTableHandleTemplate<Float64> timeStampTable;
            HashTableHandleTemplate<Vector> positionTable;
            HashTableHandleTemplate<Matrix> orientationTable;
            HashTableHandleTemplate<Vector> velocityTable;
            HashTableHandleTemplate<Vector> accelerationTable;
            HashTableHandleTemplate<Vector> scaleTable;
            HashTableHandleTemplate<Vector> vectorTable;
            HashTableHandleTemplate<Float64> scalarTable;
            HashTableHandleTemplate<String> textTable;
            HashTableHandleTemplate<Data> dataTable;

            Handle get_handle (const String &TypeName, RuntimeContext *context) {

               if (!handlePtr) {

                  String name (TypeName ? TypeName + "." : "UnknownType.");

                  handlePtr = new RuntimeHandle (name + "Event", context);
               }

               if (handlePtr) { handle = handlePtr->get_runtime_handle (); }
               else { handle = 0; }

               return handle;
            }

            void reset () {

               if (handlePtr) { delete handlePtr; handlePtr = 0; }

               next = 0;
               handle = 0;
               closed = False;
               closeTime = 0.0;
               locality = EventLocalityUnknown;
               objectTable.empty ();
               typeTable.empty ();
               stateTable.empty ();
               timeStampTable.empty ();
               positionTable.empty ();
               orientationTable.empty ();
               velocityTable.empty ();
               accelerationTable.empty ();
               scaleTable.empty ();
               vectorTable.empty ();
               scalarTable.empty ();
               textTable.empty ();
               dataTable.empty ();
            }

            EventStruct () :
                  next (0),
                  handle (0),
                  handlePtr (0),
                  closed (False),
                  closeTime (0.0),
                  locality (EventLocalityUnknown) {;}

            ~EventStruct () { if (next) { delete next; next = 0; } reset (); }
         };

         struct SubscriptionStruct {

            const Handle SubHandle;
            HashTableHandleTemplate<EventObserverStruct> startTable;
            HashTableHandleTemplate<EventObserverStruct> endTable;
            EventObserver &obs;

            SubscriptionStruct (EventObserver &theObs) :
                  SubHandle (theObs.get_event_observer_handle ()),
                  obs (theObs) {;}

            ~SubscriptionStruct () { startTable.clear (); endTable.clear (); }
         };

         EventStruct *_lookup_event (const Handle EventHandle);

         EventObserverStruct *_create_event_observers (
            const Handle TypeHandle,
            HashTableHandleTemplate<EventObserverStruct> &table);

         void _init (Config &local);

         Log _log;
         Time _time;

         Int32 _maxEvents;
         Float64 _eventTTL;

         HashTableHandleTemplate<EventStruct> _eventTable;

         EventStruct *_eventCache;
         EventStruct *_recycleList;

         HashTableHandleTemplate<SubscriptionStruct> _subscriptionTable;
         HashTableHandleTemplate<EventObserverStruct> _startTable;
         HashTableHandleTemplate<EventObserverStruct> _endTable;

      private:
         EventModuleBasic ();
         EventModuleBasic (const EventModuleBasic &);
         EventModuleBasic &operator= (const EventModuleBasic &);
   };
};


inline dmz::EventModuleBasic::EventStruct *
dmz::EventModuleBasic::_lookup_event (const Handle EventHandle) {

   if (!_eventCache || (EventHandle != _eventCache->handle)) {

      _eventCache = _eventTable.lookup (EventHandle);
   }

   return _eventCache;
}

#endif // DMZ_EVENT_MODULE_BASIC_DOT_H
