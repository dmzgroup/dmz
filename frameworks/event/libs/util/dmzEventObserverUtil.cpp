#include <dmzEventCallbackMasks.h>
#include <dmzEventModule.h>
#include <dmzEventObserverUtil.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimeConfigRead.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimeEventType.h>
#include <dmzTypesHashTableStringTemplate.h>
#include <dmzTypesHashTableHandleTemplate.h>

namespace {

struct typeStruct {

   const dmz::EventType Type;
   dmz::Mask mask;

   typeStruct (const dmz::EventType &TheType) : Type (TheType) {;}
};

};

struct dmz::EventObserverUtil::State {

   const String EventModuleName;
   EventModule *module;
   HashTableHandleTemplate<typeStruct> table;
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

   void register_obs (typeStruct &ts, EventObserver &obs) {

      if (ts.Type && module) {

         module->register_event_observer (ts.Type, ts.mask, obs);
      }
   }

   void release_obs (const Mask &EventMask, typeStruct &ts, EventObserver &obs) {

      if (ts.Type && module) {

         module->release_event_observer (ts.Type, EventMask, obs);
      }

      ts.mask.unset (EventMask);
   }
};


dmz::EventObserverUtil::EventObserverUtil (
      const PluginInfo &Info,
      const Config &Data) :
      EventObserver (Info),
      __state (*(new State (Data, Info.get_name (), Info.get_context ()))) {

}


dmz::EventObserverUtil::~EventObserverUtil () {

   if (__state.module) { remove_event_module ("", *(__state.module)); }
   delete &__state;
}


dmz::EventType
dmz::EventObserverUtil::activate_event_callback (
      const String &EventTypeName,
      const Mask &CallbackMask) {

   EventType result;

   if (__state.defs.lookup_event_type (EventTypeName, result)) {

      activate_event_callback (result, CallbackMask);
   }

   return result;
}


void
dmz::EventObserverUtil::activate_event_callback (
      const EventType &Type,
      const Mask &CallbackMask) {

   typeStruct *ts = __state.table.lookup (Type.get_handle ());

   if (Type && !ts) {

      ts = new typeStruct (Type);
      if (!__state.table.store (Type.get_handle (), ts)) { delete ts; ts = 0; }
   }

   if (ts) { ts->mask |= CallbackMask; __state.register_obs (*ts, *this); }
}


void
dmz::EventObserverUtil::deactivate_event_callback (
      const EventType &Type,
      const Mask &CallbackMask) {

   typeStruct *ts = __state.table.lookup (Type.get_handle ());

   if (ts) { __state.release_obs (CallbackMask, *ts, *this); }
}


dmz::EventModule *
dmz::EventObserverUtil::get_event_module () { return __state.module; }


void
dmz::EventObserverUtil::store_event_module (const String &Name, EventModule &module) {

   if (!__state.EventModuleName || (Name == __state.EventModuleName)) {

      if (!__state.module) {

         __state.module = &module;

         HashTableHandleIterator it;

         typeStruct *ts (__state.table.get_first (it));

         while (ts) {

            __state.register_obs (*ts, *this);
            ts = __state.table.get_next (it);
         }
      }
   }
}


void
dmz::EventObserverUtil::remove_event_module (const String &Name, EventModule &module) {

   if (__state.module == &module) {

      HashTableHandleIterator it;

      typeStruct *ts (__state.table.get_first (it));

      while (ts) {

         __state.release_obs (EventAllMask, *ts, *this);
         ts = __state.table.get_next (it);
      }

      __state.module = 0;
   }
}


void
dmz::EventObserverUtil::create_event (
      const Handle EventHandle,
      const EventType &Type,
      const EventLocalityEnum Locality) {

   if (!(EventCreateMask & __state.errorMask)) {

      __state.errorMask |= EventCreateMask;
      __state.log.warn << "Base create_event called."
         << " Function should have been overridden?" << endl;
   }
}


void
dmz::EventObserverUtil::close_event (
      const Handle EventHandle,
      const EventType &Type,
      const EventLocalityEnum Locality) {

   if (!(EventCloseMask & __state.errorMask)) {

      __state.errorMask |= EventCloseMask;
      __state.log.warn << "Base close_event called."
         << " Function should have been overridden?" << endl;
   }
}

