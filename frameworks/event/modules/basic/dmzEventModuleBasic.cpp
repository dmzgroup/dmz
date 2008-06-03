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

   static const dmz::Mask StartMask (0, dmz::EventAttributeStartEvent);
   static const dmz::Mask EndMask (0, dmz::EventAttributeEndEvent);
   static const dmz::Mask ObjectMask (0, dmz::EventAttributeObjectHandle);
   static const dmz::Mask TypeMask (0, dmz::EventAttributeObjectType);
   static const dmz::Mask StateMask (0, dmz::EventAttributeState);
   static const dmz::Mask TimeStampMask (0, dmz::EventAttributeTimeStamp);
   static const dmz::Mask PositionMask (0, dmz::EventAttributePosition);
   static const dmz::Mask OrientationMask (0, dmz::EventAttributeOrientation);
   static const dmz::Mask VelocityMask (0, dmz::EventAttributeVelocity);
   static const dmz::Mask AccelerationMask (0, dmz::EventAttributeAcceleration);
   static const dmz::Mask ScaleMask (0, dmz::EventAttributeScale);
   static const dmz::Mask VectorMask (0, dmz::EventAttributeVector);
   static const dmz::Mask ScalarMask (0, dmz::EventAttributeVector);
   static const dmz::Mask TextMask (0, dmz::EventAttributeText);
   static const dmz::Mask DataMask (0, dmz::EventAttributeData);
   static const dmz::Mask AllMask (0, dmz::EventAttributeAll);
};


dmz::EventModuleBasic::EventModuleBasic (const PluginInfo &Info, Config &local) :
      Plugin (Info),
      Sync (Info),
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
   _startTable.clear ();
   _endTable.clear ();
   _objectTable.empty ();
   _stateTable.empty ();
   _time_stampTable.empty ();
   _positionTable.empty ();
   _orientationTable.empty ();
   _velocityTable.empty ();
   _accelerationTable.empty ();
   _scaleTable.empty ();
   _vectorTable.empty ();
   _scalarTable.empty ();
   _textTable.empty ();
   _dataTable.empty ();
}


// Plugin Interface
void
dmz::EventModuleBasic::discover_plugin (const Plugin *PluginPtr) {

}


void
dmz::EventModuleBasic::start_plugin () {

}


void
dmz::EventModuleBasic::update_sync (const Float64 TimeDelta) {

   if (_maxEvents > 0) {

      HashTableHandleIterator it;

      Int32 count (0);

      const Int32 Diff (_eventTable.get_count () - _maxEvents);

      EventStruct *event (_eventTable.get_first (it));

      while (event) {

         if (event->closed) {

            event = _eventTable.remove (event->handle);

            if (event) {

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

               if (event) { event->next = _recycleList; _recycleList = event; }

               event = _eventTable.get_next (it);
            }
            else { event = 0; }
         }
         else { event = _eventTable.get_next (it); }
      }
   }
}


void
dmz::EventModuleBasic::stop_plugin () {

}


void
dmz::EventModuleBasic::shutdown_plugin () {

}


void
dmz::EventModuleBasic::remove_plugin (const Plugin *PluginPtr) {

}


// EventModule Interface
dmz::Boolean
dmz::EventModuleBasic::register_event_observer (
      const Handle AttributeHandle,
      const Mask &AttributeMask,
      EventObserver &Observer) {

   Boolean result (False); 
 
   const Handle ObsHandle (Observer.get_event_observer_handle ()); 
   SubscriptionStruct *sub (_subscriptionTable.lookup (ObsHandle)); 
 
   if (!sub) { 
 
      sub = new SubscriptionStruct (Observer); 
 
      if (!_subscriptionTable.store (ObsHandle, sub)) { delete sub; sub = 0; } 
   } 
 
   if (sub) { 
 
      Mask *attrMaskPtr (sub->table.lookup (AttributeHandle)); 
 
      if (!attrMaskPtr) { 
 
         attrMaskPtr = new Mask (AttributeMask);  
 
         if (attrMaskPtr) { 
 
            if (!sub->table.store (AttributeHandle, attrMaskPtr)) { 
 
               delete attrMaskPtr; attrMaskPtr = 0; 
            } 
         } 
      } 
      else { *attrMaskPtr |= AttributeMask; } 
 
      if (attrMaskPtr) { 
 
         result = True; 
 
         _update_subscription (AttributeHandle, AttributeMask, True, Observer); 
      } 
   }

   return result;
}


dmz::Boolean
dmz::EventModuleBasic::release_event_observer (
      const Handle AttributeHandle,
      const Mask &AttributeMask,
      EventObserver &observer) {

   Boolean result (False);

   const Handle ObsHandle (observer.get_event_observer_handle ());
   SubscriptionStruct *sub (_subscriptionTable.lookup (ObsHandle));

   if (sub) {

      Mask *attrMaskPtr (sub->table.lookup (AttributeHandle));

      if (attrMaskPtr) {

         result = True;

         _update_subscription (AttributeHandle, AttributeMask, False, observer);

         attrMaskPtr->unset (AttributeMask);

         if (!attrMaskPtr->is_set ()) {

            attrMaskPtr = sub->table.remove (ObsHandle);

            if (attrMaskPtr) { delete attrMaskPtr; attrMaskPtr = 0; }

            if (sub->table.get_count () <= 0) {

               sub = _subscriptionTable.remove (ObsHandle);
               if (sub) { delete sub; sub = 0; }
            }
         }
      }
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

      Mask *attrMaskPtr (sub->table.get_first (it));

      while (attrMaskPtr) {

         _update_subscription (it.get_hash_key (), *attrMaskPtr, False, observer);

         attrMaskPtr = sub->table.get_next (it);
      }

      delete sub; sub = 0;

      result = True;
   }

   return result;
}


dmz::Boolean
dmz::EventModuleBasic::dump_event_attributes (
      const Handle EventHandle,
      EventObserver &Observer) {

   Boolean result (False);

   return result;
}


dmz::Boolean
dmz::EventModuleBasic::dump_all_event_attributes (
      const Handle EventHandle,
      EventObserver &Observer) {

   Boolean result (False);

   return result;
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

               HashTableHandleIterator it;

               EventObserver *obs (_startTable.get_first (it));

               while (obs) {

                  obs->start_event (result, Type, Locality);
                  obs = _startTable.get_next (it);
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

      HashTableHandleIterator it;

      EventObserver *obs (_endTable.get_first (it));

      while (obs) {

         obs->end_event (EventHandle, event->type, event->locality);
         obs = _endTable.get_next (it);
      }

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

      EventObserverStruct *os (_objectTable.lookup (AttributeHandle));

      if (os) {

         HashTableHandleIterator it;

         EventObserver *obs (os->get_first (it));

         while (obs) {

            obs->update_event_object_handle (EventHandle, AttributeHandle, Value);
            obs = os->get_next (it);
         }
      }

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

      EventObserverStruct *os (_typeTable.lookup (AttributeHandle));

      if (os) {

         HashTableHandleIterator it;

         EventObserver *obs (os->get_first (it));

         while (obs) {

            obs->update_event_object_type (EventHandle, AttributeHandle, Value);
            obs = os->get_next (it);
         }
      }

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

      EventObserverStruct *os (_stateTable.lookup (AttributeHandle));

      if (os) {

         HashTableHandleIterator it;

         EventObserver *obs (os->get_first (it));

         while (obs) {

            obs->update_event_state (EventHandle, AttributeHandle, Value);
            obs = os->get_next (it);
         }
      }

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

      EventObserverStruct *os (_time_stampTable.lookup (AttributeHandle));

      if (os) {

         HashTableHandleIterator it;

         EventObserver *obs (os->get_first (it));

         while (obs) {

            obs->update_event_time_stamp (EventHandle, AttributeHandle, Value);
            obs = os->get_next (it);
         }
      }

      Float64 *valuePtr (event->time_stampTable.lookup (AttributeHandle));

      if (valuePtr) { *valuePtr = Value; result = True; }
      else {

         valuePtr = new Float64 (Value);

         if (event->time_stampTable.store (AttributeHandle, valuePtr)) { result = True; }
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

      Float64 *ptr (event->time_stampTable.lookup (AttributeHandle));

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

      EventObserverStruct *os (_positionTable.lookup (AttributeHandle));

      if (os) {

         HashTableHandleIterator it;

         EventObserver *obs (os->get_first (it));

         while (obs) {

            obs->update_event_position (EventHandle, AttributeHandle, Value);
            obs = os->get_next (it);
         }
      }

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

      EventObserverStruct *os (_orientationTable.lookup (AttributeHandle));

      if (os) {

         HashTableHandleIterator it;

         EventObserver *obs (os->get_first (it));

         while (obs) {

            obs->update_event_orientation (EventHandle, AttributeHandle, Value);
            obs = os->get_next (it);
         }
      }

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

      EventObserverStruct *os (_velocityTable.lookup (AttributeHandle));

      if (os) {

         HashTableHandleIterator it;

         EventObserver *obs (os->get_first (it));

         while (obs) {

            obs->update_event_velocity (EventHandle, AttributeHandle, Value);
            obs = os->get_next (it);
         }
      }

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

      EventObserverStruct *os (_accelerationTable.lookup (AttributeHandle));

      if (os) {

         HashTableHandleIterator it;

         EventObserver *obs (os->get_first (it));

         while (obs) {

            obs->update_event_acceleration (EventHandle, AttributeHandle, Value);
            obs = os->get_next (it);
         }
      }

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

      EventObserverStruct *os (_scaleTable.lookup (AttributeHandle));

      if (os) {

         HashTableHandleIterator it;

         EventObserver *obs (os->get_first (it));

         while (obs) {

            obs->update_event_scale (EventHandle, AttributeHandle, Value);
            obs = os->get_next (it);
         }
      }

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

      EventObserverStruct *os (_vectorTable.lookup (AttributeHandle));

      if (os) {

         HashTableHandleIterator it;

         EventObserver *obs (os->get_first (it));

         while (obs) {

            obs->update_event_vector (EventHandle, AttributeHandle, Value);
            obs = os->get_next (it);
         }
      }

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

      EventObserverStruct *os (_scalarTable.lookup (AttributeHandle));

      if (os) {

         HashTableHandleIterator it;

         EventObserver *obs (os->get_first (it));

         while (obs) {

            obs->update_event_scalar (EventHandle, AttributeHandle, Value);
            obs = os->get_next (it);
         }
      }

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

      EventObserverStruct *os (_textTable.lookup (AttributeHandle));

      if (os) {

         HashTableHandleIterator it;

         EventObserver *obs (os->get_first (it));

         while (obs) {

            obs->update_event_text (EventHandle, AttributeHandle, Value);
            obs = os->get_next (it);
         }
      }

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

      EventObserverStruct *os (_dataTable.lookup (AttributeHandle));

      if (os) {

         HashTableHandleIterator it;

         EventObserver *obs (os->get_first (it));

         while (obs) {

            obs->update_event_data (EventHandle, AttributeHandle, Value);
            obs = os->get_next (it);
         }
      }

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


static inline void
local_update_os (
      const dmz::Boolean AddObs,
      const dmz::Handle ObsHandle,
      const dmz::Handle Attribute,
      dmz::HashTableHandleTemplate<dmz::EventObserverStruct> &table,
      dmz::EventObserver *obs) {

   dmz::EventObserverStruct *os (table.lookup (Attribute));

   if (!os) {

      os = new dmz::EventObserverStruct;
      if (os) { if (!table.store (Attribute, os)) { delete os; os = 0;  } }
   }

   if (os) {

      if (AddObs) { os->store (ObsHandle, obs); }
      else { os->remove (ObsHandle); }
   }
}


void
dmz::EventModuleBasic::_update_subscription (
      const Handle AttributeHandle,
      const Mask &AttributeMask,
      const Boolean AddObs,
      EventObserver &obs) {

   const Handle ObsHandle (obs.get_event_observer_handle ());

   if (StartMask & AttributeMask) {

      if (AddObs) { _startTable.store (ObsHandle, &obs); }
      else { _startTable.remove (ObsHandle); }
   }

   if (EndMask & AttributeMask) {

      if (AddObs) { _endTable.store (ObsHandle, &obs); }
      else { _endTable.remove (ObsHandle); }
   }

   if (ObjectMask & AttributeMask) {

      local_update_os (AddObs, ObsHandle, AttributeHandle, _objectTable, &obs);
   }

   if (TypeMask & AttributeMask) {

      local_update_os (AddObs, ObsHandle, AttributeHandle, _typeTable, &obs);
   }

   if (StateMask & AttributeMask) {

      local_update_os (AddObs, ObsHandle, AttributeHandle, _stateTable, &obs);
   }

   if (TimeStampMask & AttributeMask) {

      local_update_os (AddObs, ObsHandle, AttributeHandle, _time_stampTable, &obs);
   }

   if (PositionMask & AttributeMask) {

      local_update_os (AddObs, ObsHandle, AttributeHandle, _positionTable, &obs);
   }

   if (OrientationMask & AttributeMask) {

      local_update_os (AddObs, ObsHandle, AttributeHandle, _orientationTable, &obs);
   }

   if (VelocityMask & AttributeMask) {

      local_update_os (AddObs, ObsHandle, AttributeHandle, _velocityTable, &obs);
   }

   if (AccelerationMask & AttributeMask) {

      local_update_os (AddObs, ObsHandle, AttributeHandle, _accelerationTable, &obs);
   }

   if (ScaleMask & AttributeMask) {

      local_update_os (AddObs, ObsHandle, AttributeHandle, _scaleTable, &obs);
   }

   if (VectorMask & AttributeMask) {

      local_update_os (AddObs, ObsHandle, AttributeHandle, _vectorTable, &obs);
   }

   if (ScalarMask & AttributeMask) {

      local_update_os (AddObs, ObsHandle, AttributeHandle, _scalarTable, &obs);
   }

   if (TextMask & AttributeMask) {

      local_update_os (AddObs, ObsHandle, AttributeHandle, _textTable, &obs);
   }

   if (DataMask & AttributeMask) {

      local_update_os (AddObs, ObsHandle, AttributeHandle, _dataTable, &obs);
   }
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
