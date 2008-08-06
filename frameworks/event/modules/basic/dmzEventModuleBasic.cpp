#include <dmzEventDump.h>
#include "dmzEventModuleBasic.h"
#include <dmzRuntimeConfigRead.h>
#include <dmzRuntimeData.h>
#include <dmzRuntimeInit.h>
#include <dmzRuntimeEventType.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzTypesBase.h>
#include <dmzTypesMask.h>
#include <dmzTypesString.h>
#include <dmzTypesVector.h>

namespace {

static const dmz::Mask StartMask (0, dmz::EventCallbackStartEvent);
static const dmz::Mask EndMask (0, dmz::EventCallbackEndEvent);

};


dmz::EventModuleBasic::EventModuleBasic (const PluginInfo &Info, Config &local) :
      Plugin (Info),
      TimeSlice (Info),
      EventModule (Info),
      _log (Info),
      _time (Info.get_context ()),
      _maxEvents (0),
      _eventTTL (1.0),
      _eventCache (0),
      _recycleList (0) {

   _init (local);
}


dmz::EventModuleBasic::~EventModuleBasic () {

   _eventCache = 0;

   if (_recycleList) { delete _recycleList; _recycleList = 0; }

   _eventTable.empty ();

   HashTableHandleIterator it;

   SubscriptionStruct *ss (_subscriptionTable.get_first (it));

   RuntimeContext *context (_PluginInfoData.get_context ());

   while (ss) {

      if (EventObserver::is_valid (ss->SubHandle, context)) {

         ss->obs.remove_event_module (_PluginInfoData.get_name (), *this);
      }

      ss  = _subscriptionTable.get_next (it);
   }

   _subscriptionTable.empty ();
   _startTable.empty ();
   _endTable.empty ();
}

// Plugin Interface
void
dmz::EventModuleBasic::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      EventObserver *obs (EventObserver::cast (PluginPtr));

      if (obs) { obs->store_event_module (_PluginInfoData.get_name (), *this); }
   }
   else if (Mode == PluginDiscoverRemove) {

      EventObserver *obs (EventObserver::cast (PluginPtr));

      if (obs) { obs->remove_event_module (_PluginInfoData.get_name (), *this); }
   }
}


// TimeSlice Interface
void
dmz::EventModuleBasic::update_time_slice (const Float64 TimeDelta) {

   if (_maxEvents > 0) {

      HashTableHandleIterator it;

      Int32 count (0);

      const Int32 Diff (_eventTable.get_count () - _maxEvents);

      EventStruct *event (_eventTable.get_first (it));

      while (event) {

         if (event->closed) {

            event = _eventTable.remove (event->handle);

            if (event) {

               if (event == _eventCache) { _eventCache = 0; }

               count++;
               event->next = _recycleList; _recycleList = event;

               if (Diff < count) { event = 0; }
               else { event = _eventTable.get_next (it); }
            }
         }
         else { event = _eventTable.get_next (it); }
      }
   }
   else {

      const Float64 FrameTime (_time.get_frame_time () - _eventTTL);

      HashTableHandleIterator it;

      EventStruct *event (_eventTable.get_first (it));

      while (event) {

         if (event->closed) {

            if (event->closeTime < FrameTime) {

               event = _eventTable.remove (event->handle);

               if (event == _eventCache) { _eventCache = 0; }

               if (event) { event->next = _recycleList; _recycleList = event; }

               event = _eventTable.get_next (it);
            }
            else { event = 0; }
         }
         else { event = _eventTable.get_next (it); }
      }
   }
}


// EventModule Interface
dmz::Boolean
dmz::EventModuleBasic::register_event_observer (
      const EventType &Type,
      const Mask &CallbackMask,
      EventObserver &observer) {

   Boolean result (False);

   const Handle ObsHandle (observer.get_event_observer_handle ());
   SubscriptionStruct *sub (_subscriptionTable.lookup (ObsHandle));

   if (!sub) {

      sub = new SubscriptionStruct (observer);

      if (!_subscriptionTable.store (ObsHandle, sub)) { delete sub; sub = 0; }
   }

   if (sub) {

      if (CallbackMask & StartMask) {

         EventObserverStruct *eos (
            _create_event_observers (Type.get_handle (), _startTable));

         if (eos && eos->store (ObsHandle, &observer)) {

            sub->startTable.store (Type.get_handle (), eos);
         }
      }

      if (CallbackMask & EndMask) {

         EventObserverStruct *eos (
            _create_event_observers (Type.get_handle (), _endTable));

         if (eos && eos->store (ObsHandle, &observer)) {

            sub->endTable.store (Type.get_handle (), eos);
         }
      }

      result = True;
   }

   return result;
}


dmz::Boolean
dmz::EventModuleBasic::release_event_observer (
      const EventType &Type,
      const Mask &CallbackMask,
      EventObserver &observer) {

   Boolean result (False);

   const Handle ObsHandle (observer.get_event_observer_handle ());
   SubscriptionStruct *sub (_subscriptionTable.lookup (ObsHandle));

   if (sub) {

      if (CallbackMask & StartMask) {

         EventObserverStruct *eos (sub->startTable.remove (Type.get_handle ()));

         if (eos) { eos->remove (ObsHandle); }
      }

      if (CallbackMask & EndMask) {

         EventObserverStruct *eos (sub->endTable.remove (Type.get_handle ()));

         if (eos) { eos->remove (ObsHandle); }
      }

      result = True;
   }

   return result;
}


dmz::Boolean
dmz::EventModuleBasic::release_event_observer_all (EventObserver &observer) {

   Boolean result (False);

   const Handle ObsHandle (observer.get_event_observer_handle ());
   SubscriptionStruct *sub (_subscriptionTable.remove (ObsHandle));

   if (sub) {

      HashTableHandleIterator it;

      EventObserverStruct *eos (sub->startTable.get_first (it));

      while (eos) {

         eos->remove (ObsHandle);
         eos = sub->startTable.get_next (it);
      }

      eos = sub->endTable.get_first (it);

      while (eos) {

         eos->remove (ObsHandle);
         eos = sub->endTable.get_next (it);
      }

      delete sub; sub = 0;

      result = True;
   }

   return result;
}


void
dmz::EventModuleBasic::dump_event (const Handle EventHandle, EventDump &dump) {

   EventStruct *event (_lookup_event (EventHandle));

   if (event) {

      dump.start_dump_event (event->handle, event->type, event->locality);

      HashTableHandleIterator it;

      {
         Handle *ptr = event->objectTable.get_first (it);

         while (ptr) {

            dump.store_event_object_handle (event->handle, it.get_hash_key (), *ptr);
            ptr = event->objectTable.get_next (it);
         }
      }

      {
         ObjectType *ptr = event->typeTable.get_first (it);

         while (ptr) {

            dump.store_event_object_type (event->handle, it.get_hash_key (), *ptr);
            ptr = event->typeTable.get_next (it);
         }
      }

      {
         Mask *ptr = event->stateTable.get_first (it);

         while (ptr) {

            dump.store_event_state (event->handle, it.get_hash_key (), *ptr);
            ptr = event->stateTable.get_next (it);
         }
      }

      {
         Float64 *ptr = event->timeStampTable.get_first (it);

         while (ptr) {

            dump.store_event_time_stamp (event->handle, it.get_hash_key (), *ptr);
            ptr = event->timeStampTable.get_next (it);
         }
      }

      {
         Vector *ptr = event->positionTable.get_first (it);

         while (ptr) {

            dump.store_event_position (event->handle, it.get_hash_key (), *ptr);
            ptr = event->positionTable.get_next (it);
         }
      }

      {
         Matrix *ptr = event->orientationTable.get_first (it);

         while (ptr) {

            dump.store_event_orientation (event->handle, it.get_hash_key (), *ptr);
            ptr = event->orientationTable.get_next (it);
         }
      }

      {
         Vector *ptr = event->velocityTable.get_first (it);

         while (ptr) {

            dump.store_event_velocity (event->handle, it.get_hash_key (), *ptr);
            ptr = event->velocityTable.get_next (it);
         }
      }

      {
         Vector *ptr = event->accelerationTable.get_first (it);

         while (ptr) {

            dump.store_event_acceleration (event->handle, it.get_hash_key (), *ptr);
            ptr = event->accelerationTable.get_next (it);
         }
      }

      {
         Vector *ptr = event->scaleTable.get_first (it);

         while (ptr) {

            dump.store_event_scale (event->handle, it.get_hash_key (), *ptr);
            ptr = event->scaleTable.get_next (it);
         }
      }

      {
         Vector *ptr = event->vectorTable.get_first (it);

         while (ptr) {

            dump.store_event_vector (event->handle, it.get_hash_key (), *ptr);
            ptr = event->vectorTable.get_next (it);
         }
      }

      {
         Float64 *ptr = event->scalarTable.get_first (it);

         while (ptr) {

            dump.store_event_scalar (event->handle, it.get_hash_key (), *ptr);
            ptr = event->scalarTable.get_next (it);
         }
      }

      {
         String *ptr = event->textTable.get_first (it);

         while (ptr) {

            dump.store_event_text (event->handle, it.get_hash_key (), *ptr);
            ptr = event->textTable.get_next (it);
         }
      }

      {
         Data *ptr = event->dataTable.get_first (it);

         while (ptr) {

            dump.store_event_data (event->handle, it.get_hash_key (), *ptr);
            ptr = event->dataTable.get_next (it);
         }
      }

      dump.end_dump_event (event->handle);
   }
}


dmz::Handle
dmz::EventModuleBasic::start_event (
      const EventType &Type,
      const EventLocalityEnum Locality) {

   Handle result (0);

   if (Type && (Locality != EventLocalityUnknown)) {

      EventStruct *event (_recycleList);

      if (event) { _recycleList = _recycleList->next; event->next = 0; event->reset (); }
      else { event = new EventStruct; }

      if (event) {

         result = event->get_handle (Type.get_name (), _PluginInfoData.get_context ());

         if (result) {

            if (_eventTable.store (result, event)) {

               event->type = Type;
               event->locality = Locality;

               EventType current (Type);

               while (current) {

                  EventObserverStruct *eos (_startTable.lookup (current.get_handle ()));

                  if (eos) {

                     HashTableHandleIterator it;

                     EventObserver *obs (eos->get_first (it));

                     while (obs) {

                        obs->start_event (result, event->type, event->locality);
                        obs = eos->get_next (it);
                     }
                  }

                  current.become_parent ();
               }
            }
            else {

               result = 0;
               event->next = _recycleList; _recycleList = event;
            }
         }
      }
   }

   return result;
}


dmz::Boolean
dmz::EventModuleBasic::end_event (const Handle EventHandle) {

   Boolean result (False);

   EventStruct *event (_lookup_event (EventHandle));

   if (event && !event->closed) {

      event->closed = True;
      event->closeTime = _time.get_frame_time ();

      EventType current (event->type);

      while (current) {

         EventObserverStruct *eos (_endTable.lookup (current.get_handle ()));

         if (eos) {

            HashTableHandleIterator it;

            EventObserver *obs (eos->get_first (it));

            while (obs) {

               obs->end_event (event->handle, event->type, event->locality);
               obs = eos->get_next (it);
            }
         }

         current.become_parent ();
      }

      // Move to end of the table.
      if (_eventTable.remove (event->handle)) {

         if (_eventTable.store (event->handle, event)) { result = True; }
         else { _eventCache = 0; delete event; event = 0; }
      }
   }

   return result;
}


dmz::Boolean
dmz::EventModuleBasic::lookup_event_type (const Handle EventHandle, EventType &value) {

   Boolean result (False);

   EventStruct *event (_lookup_event (EventHandle));

   if (event) { value = event->type; result = True; }

   return result;
}


dmz::EventLocalityEnum
dmz::EventModuleBasic::lookup_locality (const Handle EventHandle) {

   EventLocalityEnum result (EventLocalityUnknown);

   EventStruct *event (_lookup_event (EventHandle));

   if (event) { result = event->locality; }

   return result;
}


dmz::Boolean
dmz::EventModuleBasic::store_object_handle (
      const Handle EventHandle,
      const Handle AttributeHandle,
      const Handle Value) {

   Boolean result (False);

   EventStruct *event (_lookup_event (EventHandle));

   if (event && !event->closed) {

      Handle *valuePtr (event->objectTable.lookup (AttributeHandle));

      if (valuePtr) { *valuePtr = Value; result = True; }
      else {

         valuePtr = new Handle (Value);

         if (event->objectTable.store (AttributeHandle, valuePtr)) { result = True; }
         else { delete valuePtr; valuePtr = 0; }
      }
   }

   return result;
}


dmz::Boolean
dmz::EventModuleBasic::lookup_object_handle (
      const Handle EventHandle,
      const Handle AttributeHandle,
      Handle &value) {

   Boolean result (False);

   EventStruct *event (_lookup_event (EventHandle));

   if (event) {

      Handle *ptr (event->objectTable.lookup (AttributeHandle));

      if (ptr) { value = *ptr; result = True; }
   }

   return result;
}


dmz::Boolean
dmz::EventModuleBasic::store_object_type (
      const Handle EventHandle,
      const Handle AttributeHandle,
      const ObjectType &Value) {

   Boolean result (False);

   EventStruct *event (_lookup_event (EventHandle));

   if (event && !event->closed) {

      ObjectType *valuePtr (event->typeTable.lookup (AttributeHandle));

      if (valuePtr) { *valuePtr = Value; result = True; }
      else {

         valuePtr = new ObjectType (Value);

         if (event->typeTable.store (AttributeHandle, valuePtr)) { result = True; }
         else { delete valuePtr; valuePtr = 0; }
      }
   }

   return result;
}


dmz::Boolean
dmz::EventModuleBasic::lookup_object_type (
      const Handle EventHandle,
      const Handle AttributeHandle,
      ObjectType &value) {

   Boolean result (False);

   EventStruct *event (_lookup_event (EventHandle));

   if (event) {

      ObjectType *ptr (event->typeTable.lookup (AttributeHandle));

      if (ptr) { value = *ptr; result = True; }
   }

   return result;
}


dmz::Boolean
dmz::EventModuleBasic::store_state (
      const Handle EventHandle,
      const Handle AttributeHandle,
      const Mask &Value) {

   Boolean result (False);

   EventStruct *event (_lookup_event (EventHandle));

   if (event && !event->closed) {

      Mask *valuePtr (event->stateTable.lookup (AttributeHandle));

      if (valuePtr) { *valuePtr = Value; result = True; }
      else {

         valuePtr = new Mask (Value);

         if (event->stateTable.store (AttributeHandle, valuePtr)) { result = True; }
         else { delete valuePtr; valuePtr = 0; }
      }
   }

   return result;
}


dmz::Boolean
dmz::EventModuleBasic::lookup_state (
      const Handle EventHandle,
      const Handle AttributeHandle,
      Mask &value) {

   Boolean result (False);

   EventStruct *event (_lookup_event (EventHandle));

   if (event) {

      Mask *ptr (event->stateTable.lookup (AttributeHandle));

      if (ptr) { value = *ptr; result = True; }
   }

   return result;
}


dmz::Boolean
dmz::EventModuleBasic::store_time_stamp (
      const Handle EventHandle,
      const Handle AttributeHandle,
      const Float64 &Value) {

   Boolean result (False);

   EventStruct *event (_lookup_event (EventHandle));

   if (event && !event->closed) {

      Float64 *valuePtr (event->timeStampTable.lookup (AttributeHandle));

      if (valuePtr) { *valuePtr = Value; result = True; }
      else {

         valuePtr = new Float64 (Value);

         if (event->timeStampTable.store (AttributeHandle, valuePtr)) { result = True; }
         else { delete valuePtr; valuePtr = 0; }
      }
   }

   return result;
}


dmz::Boolean
dmz::EventModuleBasic::lookup_time_stamp (
      const Handle EventHandle,
      const Handle AttributeHandle,
      Float64 &value) {

   Boolean result (False);

   EventStruct *event (_lookup_event (EventHandle));

   if (event) {

      Float64 *ptr (event->timeStampTable.lookup (AttributeHandle));

      if (ptr) { value = *ptr; result = True; }
   }

   return result;
}


dmz::Boolean
dmz::EventModuleBasic::store_position (
      const Handle EventHandle,
      const Handle AttributeHandle,
      const Vector &Value) {

   Boolean result (False);

   EventStruct *event (_lookup_event (EventHandle));

   if (event && !event->closed) {

      Vector *valuePtr (event->positionTable.lookup (AttributeHandle));

      if (valuePtr) { *valuePtr = Value; result = True; }
      else {

         valuePtr = new Vector (Value);

         if (event->positionTable.store (AttributeHandle, valuePtr)) { result = True; }
         else { delete valuePtr; valuePtr = 0; }
      }
   }

   return result;
}


dmz::Boolean
dmz::EventModuleBasic::lookup_position (
      const Handle EventHandle,
      const Handle AttributeHandle,
      Vector &value) {

   Boolean result (False);

   EventStruct *event (_lookup_event (EventHandle));

   if (event) {

      Vector *ptr (event->positionTable.lookup (AttributeHandle));

      if (ptr) { value = *ptr; result = True; }
   }

   return result;
}


dmz::Boolean
dmz::EventModuleBasic::store_orientation (
      const Handle EventHandle,
      const Handle AttributeHandle,
      const Matrix &Value) {

   Boolean result (False);

   EventStruct *event (_lookup_event (EventHandle));

   if (event && !event->closed) {

      Matrix *valuePtr (event->orientationTable.lookup (AttributeHandle));

      if (valuePtr) { *valuePtr = Value; result = True; }
      else {

         valuePtr = new Matrix (Value);

         if (event->orientationTable.store (AttributeHandle, valuePtr)) { result = True; }
         else { delete valuePtr; valuePtr = 0; }
      }
   }

   return result;
}


dmz::Boolean
dmz::EventModuleBasic::lookup_orientation (
      const Handle EventHandle,
      const Handle AttributeHandle,
      Matrix &value) {

   Boolean result (False);

   EventStruct *event (_lookup_event (EventHandle));

   if (event) {

      Matrix *ptr (event->orientationTable.lookup (AttributeHandle));

      if (ptr) { value = *ptr; result = True; }
   }

   return result;
}


dmz::Boolean
dmz::EventModuleBasic::store_velocity (
      const Handle EventHandle,
      const Handle AttributeHandle,
      const Vector &Value) {

   Boolean result (False);

   EventStruct *event (_lookup_event (EventHandle));

   if (event && !event->closed) {

      Vector *valuePtr (event->velocityTable.lookup (AttributeHandle));

      if (valuePtr) { *valuePtr = Value; result = True; }
      else {

         valuePtr = new Vector (Value);

         if (event->velocityTable.store (AttributeHandle, valuePtr)) { result = True; }
         else { delete valuePtr; valuePtr = 0; }
      }
   }

   return result;
}


dmz::Boolean
dmz::EventModuleBasic::lookup_velocity (
      const Handle EventHandle,
      const Handle AttributeHandle,
      Vector &value) {

   Boolean result (False);

   EventStruct *event (_lookup_event (EventHandle));

   if (event) {

      Vector *ptr (event->velocityTable.lookup (AttributeHandle));

      if (ptr) { value = *ptr; result = True; }
   }

   return result;
}


dmz::Boolean
dmz::EventModuleBasic::store_acceleration (
      const Handle EventHandle,
      const Handle AttributeHandle,
      const Vector &Value) {

   Boolean result (False);

   EventStruct *event (_lookup_event (EventHandle));

   if (event && !event->closed) {

      Vector *valuePtr (event->velocityTable.lookup (AttributeHandle));

      if (valuePtr) { *valuePtr = Value; result = True; }
      else {

         valuePtr = new Vector (Value);

         if (event->accelerationTable.store (AttributeHandle, valuePtr)) {

            result = True;
         }
         else { delete valuePtr; valuePtr = 0; }
      }
   }

   return result;
}


dmz::Boolean
dmz::EventModuleBasic::lookup_acceleration (
      const Handle EventHandle,
      const Handle AttributeHandle,
      Vector &value) {

   Boolean result (False);

   EventStruct *event (_lookup_event (EventHandle));

   if (event) {

      Vector *ptr (event->accelerationTable.lookup (AttributeHandle));

      if (ptr) { value = *ptr; result = True; }
   }

   return result;
}


dmz::Boolean
dmz::EventModuleBasic::store_scale (
      const Handle EventHandle,
      const Handle AttributeHandle,
      const Vector &Value) {

   Boolean result (False);

   EventStruct *event (_lookup_event (EventHandle));

   if (event && !event->closed) {

      Vector *valuePtr (event->scaleTable.lookup (AttributeHandle));

      if (valuePtr) { *valuePtr = Value; result = True; }
      else {

         valuePtr = new Vector (Value);

         if (event->scaleTable.store (AttributeHandle, valuePtr)) { result = True; }
         else { delete valuePtr; valuePtr = 0; }
      }
   }

   return result;
}


dmz::Boolean
dmz::EventModuleBasic::lookup_scale (
      const Handle EventHandle,
      const Handle AttributeHandle,
      Vector &value) {

   Boolean result (False);

   EventStruct *event (_lookup_event (EventHandle));

   if (event) {

      Vector *ptr (event->scaleTable.lookup (AttributeHandle));

      if (ptr) { value = *ptr; result = True; }
   }

   return result;
}


dmz::Boolean
dmz::EventModuleBasic::store_vector (
      const Handle EventHandle,
      const Handle AttributeHandle,
      const Vector &Value) {

   Boolean result (False);

   EventStruct *event (_lookup_event (EventHandle));

   if (event && !event->closed) {

      Vector *valuePtr (event->vectorTable.lookup (AttributeHandle));

      if (valuePtr) { *valuePtr = Value; result = True; }
      else {

         valuePtr = new Vector (Value);

         if (event->vectorTable.store (AttributeHandle, valuePtr)) { result = True; }
         else { delete valuePtr; valuePtr = 0; }
      }
   }

   return result;
}


dmz::Boolean
dmz::EventModuleBasic::lookup_vector (
      const Handle EventHandle,
      const Handle AttributeHandle,
      Vector &value) {

   Boolean result (False);

   EventStruct *event (_lookup_event (EventHandle));

   if (event) {

      Vector *ptr (event->vectorTable.lookup (AttributeHandle));

      if (ptr) { value = *ptr; result = True; }
   }

   return result;
}


dmz::Boolean
dmz::EventModuleBasic::store_scalar (
      const Handle EventHandle,
      const Handle AttributeHandle,
      const Float64 Value) {

   Boolean result (False);

   EventStruct *event (_lookup_event (EventHandle));

   if (event && !event->closed) {

      Float64 *valuePtr (event->scalarTable.lookup (AttributeHandle));

      if (valuePtr) { *valuePtr = Value; result = True; }
      else {

         valuePtr = new Float64 (Value);

         if (event->scalarTable.store (AttributeHandle, valuePtr)) { result = True; }
         else { delete valuePtr; valuePtr = 0; }
      }
   }

   return result;
}


dmz::Boolean
dmz::EventModuleBasic::lookup_scalar (
      const Handle EventHandle,
      const Handle AttributeHandle,
      Float64 &value) {

   Boolean result (False);

   EventStruct *event (_lookup_event (EventHandle));

   if (event) {

      Float64 *ptr (event->scalarTable.lookup (AttributeHandle));

      if (ptr) { value = *ptr; result = True; }
   }

   return result;
}


dmz::Boolean
dmz::EventModuleBasic::store_text (
      const Handle EventHandle,
      const Handle AttributeHandle,
      const String &Value) {

   Boolean result (False);

   EventStruct *event (_lookup_event (EventHandle));

   if (event && !event->closed) {

      String *valuePtr (event->textTable.lookup (AttributeHandle));

      if (valuePtr) { *valuePtr = Value; result = True; }
      else {

         valuePtr = new String (Value);

         if (event->textTable.store (AttributeHandle, valuePtr)) { result = True; }
         else { delete valuePtr; valuePtr = 0; }
      }
   }

   return result;
}


dmz::Boolean
dmz::EventModuleBasic::lookup_text (
      const Handle EventHandle,
      const Handle AttributeHandle,
      String &value) {

   Boolean result (False);

   EventStruct *event (_lookup_event (EventHandle));

   if (event) {

      String *ptr (event->textTable.lookup (AttributeHandle));

      if (ptr) { value = *ptr; result = True; }
   }

   return result;
}


dmz::Boolean
dmz::EventModuleBasic::store_data (
      const Handle EventHandle,
      const Handle AttributeHandle,
      const Data &Value) {

   Boolean result (False);

   EventStruct *event (_lookup_event (EventHandle));

   if (event && !event->closed) {

      Data *valuePtr (event->dataTable.lookup (AttributeHandle));

      if (valuePtr) { *valuePtr = Value; result = True; }
      else {

         valuePtr = new Data (Value);

         if (event->dataTable.store (AttributeHandle, valuePtr)) { result = True; }
         else { delete valuePtr; valuePtr = 0; }
      }
   }

   return result;
}


dmz::Boolean
dmz::EventModuleBasic::lookup_data (
      const Handle EventHandle,
      const Handle AttributeHandle,
      Data &value) {

   Boolean result (False);

   EventStruct *event (_lookup_event (EventHandle));

   if (event) {

      Data *ptr (event->dataTable.lookup (AttributeHandle));

      if (ptr) { value = *ptr; result = True; }
   }

   return result;
}


dmz::EventObserverStruct *
dmz::EventModuleBasic::_create_event_observers (
      const Handle TypeHandle,
      HashTableHandleTemplate<EventObserverStruct> &table) {

   EventObserverStruct *result (table.lookup (TypeHandle));

   if (!result) {

      result = new EventObserverStruct;

      if (result && !table.store (TypeHandle, result)) { delete result, result = 0; }
   }

   return result;
}


void
dmz::EventModuleBasic::_init (Config &local) {

   _eventTTL = config_to_float64 ("ttl.value", local, 1.0);
   _maxEvents = config_to_int32 ("max.value", local, 0);
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzEventModuleBasic (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::EventModuleBasic (Info, local);
}

};
