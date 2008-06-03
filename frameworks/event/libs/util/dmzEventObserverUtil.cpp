#include <dmzEventAttributeMasks.h>
#include <dmzEventModule.h>
#include <dmzEventObserverUtil.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimeConfigRead.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzTypesHashTableStringTemplate.h>
#include <dmzTypesHashTableHandleTemplate.h>

namespace {

struct attrStruct {

   const dmz::Handle AttrHandle;
   dmz::Mask mask;

   attrStruct (const dmz::Handle TheHandle) : AttrHandle (TheHandle) {;}
};

};

struct dmz::EventObserverUtil::State {

   const String EventModuleName;
   EventModule *module;
   HashTableHandleTemplate<attrStruct> table;
   Mask errorMask;
   Log log;
   Definitions defs;
   
   State (
         const Config &Data,
         const String &LogName,
         RuntimeContext *context) :
         EventModuleName (config_to_string ("module.event.name", Data)),
         module (0),
         log (LogName + ".EventObserverUtil", context),
         defs (context, &log) {

      if (EventModuleName) {

         log.info << "Looking for Event Module: " << EventModuleName << endl;
      }
   }

   ~State () { table.empty (); }

   void register_obs (attrStruct &as, EventObserver &obs) {
            
      if (as.AttrHandle && module) {

         module->register_event_observer (as.AttrHandle, as.mask, obs);
      }
   }

   void release_obs (const Mask &EventMask, attrStruct &as, EventObserver &obs) {

      if (as.AttrHandle && module) {

         module->release_event_observer (as.AttrHandle, EventMask, obs);
      }

      as.mask.unset (EventMask);
   }
};


dmz::EventObserverUtil::EventObserverUtil (
      const PluginInfo &Info,
      const Config &Data) :
      EventObserver (Info),
      __state (*(new State (Data, Info.get_name (), Info.get_context ()))) {

}


dmz::EventObserverUtil::~EventObserverUtil () { delete &__state; }


dmz::Handle
dmz::EventObserverUtil::activate_event_attribute (
      const String &AttributeTypeName,
      const Mask &AttributeMask) {

   Handle result (__state.defs.create_named_handle (AttributeTypeName));
   attrStruct *as = __state.table.lookup (result);
   if (!as) {

      as = new attrStruct (result);
      if (!__state.table.store (result, as)) { delete as; as = 0; }
   }

   if (as) { __state.register_obs (*as, *this); }

   return result;
}


dmz::Handle
dmz::EventObserverUtil::activate_default_event_attribute (const Mask &AttributeMask) {

   return activate_event_attribute (EventAttributeDefaultName, AttributeMask);
}


void
dmz::EventObserverUtil::deactivate_event_attribute (
      const String &AttributeTypeName,
      const Mask &AttributeMask) {

   deactivate_event_attribute (
      __state.defs.lookup_named_handle (AttributeTypeName),
      AttributeMask);
}


void
dmz::EventObserverUtil::deactivate_event_attribute (
      const Handle AttrHandle,
      const Mask &AttributeMask) {

   attrStruct *as (__state.table.lookup (AttrHandle));
   if (as) { __state.release_obs (AttributeMask, *as, *this); }
}


void
dmz::EventObserverUtil::deactivate_default_event_attribute (const Mask &AttributeMask) {

   deactivate_event_attribute (EventAttributeDefaultName, AttributeMask);
}


void
dmz::EventObserverUtil::store_event_module (const String &Name, EventModule &module) {

   if (!__state.EventModuleName || (Name == __state.EventModuleName)) {

      if (!__state.module) { __state.module = &module; }
   }
}


void
dmz::EventObserverUtil::remove_event_module (const String &Name, EventModule &module) {

   if (__state.module == &module) { __state.module = 0; }
}


void
dmz::EventObserverUtil::start_event (
      const Handle EventHandle,
      const EventType &Type,
      const EventLocalityEnum Locality) {

   if (!(EventStartMask & __state.errorMask)) {

      __state.errorMask |= EventStartMask;
      __state.log.warn << "Base start_event called."
         << " Function should have been overridden?" << endl;
   }
}


void
dmz::EventObserverUtil::end_event (
      const Handle EventHandle,
      const EventType &Type,
      const EventLocalityEnum Locality) {

   if (!(EventEndMask & __state.errorMask)) {

      __state.errorMask |= EventEndMask;
      __state.log.warn << "Base end_event called."
         << " Function should have been overridden?" << endl;
   }
}


void
dmz::EventObserverUtil::update_event_object_handle (
      const Handle EventHandle,
      const Handle AttributeHandle,
      const Handle Value) {

   if (!(EventObjectMask & __state.errorMask)) {

      __state.errorMask |= EventObjectMask;
      __state.log.warn << "Base update_event_object_handle called."
         << " Function should have been overridden?" << endl;
   }
}


void
dmz::EventObserverUtil::update_event_object_type (
      const Handle EventHandle,
      const Handle AttributeHandle,
      const ObjectType &Value) {

   if (!(EventObjectTypeMask & __state.errorMask)) {

      __state.errorMask |= EventObjectTypeMask;
      __state.log.warn << "Base update_event_object_type called."
         << " Function should have been overridden?" << endl;
   }
}


void
dmz::EventObserverUtil::update_event_state (
      const Handle EventHandle,
      const Handle AttributeHandle,
      const Mask &Value) {

   if (!(EventStateMask & __state.errorMask)) {

      __state.errorMask |= EventStateMask;
      __state.log.warn << "Base update_event_state called."
         << " Function should have been overridden?" << endl;
   }
}


void
dmz::EventObserverUtil::update_event_time_stamp (
      const Handle EventHandle,
      const Handle AttributeHandle,
      const Float64 &Value) {

   if (!(EventTimeStampMask & __state.errorMask)) {

      __state.errorMask |= EventTimeStampMask;
      __state.log.warn << "Base update_event_time_stamp called."
         << " Function should have been overridden?" << endl;
   }
}


void
dmz::EventObserverUtil::update_event_position (
      const Handle EventHandle,
      const Handle AttributeHandle,
      const Vector &Value) {

   if (!(EventPositionMask & __state.errorMask)) {

      __state.errorMask |= EventPositionMask;
      __state.log.warn << "Base update_event_position called."
         << " Function should have been overridden?" << endl;
   }
}


void
dmz::EventObserverUtil::update_event_orientation (
      const Handle EventHandle,
      const Handle AttributeHandle,
      const Matrix &Value) {

   if (!(EventOrientationMask & __state.errorMask)) {

      __state.errorMask |= EventOrientationMask;
      __state.log.warn << "Base update_event_orientation called."
         << " Function should have been overridden?" << endl;
   }
}


void
dmz::EventObserverUtil::update_event_velocity (
      const Handle EventHandle,
      const Handle AttributeHandle,
      const Vector &Value) {

   if (!(EventVelocityMask & __state.errorMask)) {

      __state.errorMask |= EventVelocityMask;
      __state.log.warn << "Base update_event_velocity called."
         << " Function should have been overridden?" << endl;
   }
}


void
dmz::EventObserverUtil::update_event_acceleration (
      const Handle EventHandle,
      const Handle AttributeHandle,
      const Vector &Value) {

   if (!(EventAccelerationMask & __state.errorMask)) {

      __state.errorMask |= EventAccelerationMask;
      __state.log.warn << "Base update_event_acceleration called."
         << " Function should have been overridden?" << endl;
   }
}


void
dmz::EventObserverUtil::update_event_scale (
      const Handle EventHandle,
      const Handle AttributeHandle,
      const Vector &Value) {

   if (!(EventScaleMask & __state.errorMask)) {

      __state.errorMask |= EventScaleMask;
      __state.log.warn << "Base update_event_scale called."
         << " Function should have been overridden?" << endl;
   }
}


void
dmz::EventObserverUtil::update_event_vector (
      const Handle EventHandle,
      const Handle AttributeHandle,
      const Vector &Value) {

   if (!(EventVectorMask & __state.errorMask)) {

      __state.errorMask |= EventVectorMask;
      __state.log.warn << "Base update_event_vector called."
         << " Function should have been overridden?" << endl;
   }
}


void
dmz::EventObserverUtil::update_event_scalar (
      const Handle EventHandle,
      const Handle AttributeHandle,
      const Float64 Value) {

   if (!(EventScalarMask & __state.errorMask)) {

      __state.errorMask |= EventScalarMask;
      __state.log.warn << "Base update_event_scalar called."
         << " Function should have been overridden?" << endl;
   }
}


void
dmz::EventObserverUtil::update_event_text (
      const Handle EventHandle,
      const Handle AttributeHandle,
      const String &Value) {

   if (!(EventTextMask & __state.errorMask)) {

      __state.errorMask |= EventTextMask;
      __state.log.warn << "Base update_event_text called."
         << " Function should have been overridden?" << endl;
   }
}


void
dmz::EventObserverUtil::update_event_data (
      const Handle EventHandle,
      const Handle AttributeHandle,
      const Data &Value) {

   if (!(EventDataMask & __state.errorMask)) {

      __state.errorMask |= EventDataMask;
      __state.log.warn << "Base update_event_data called."
         << " Function should have been overridden?" << endl;
   }
}

