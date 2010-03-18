#include <dmzEventCallbackMasks.h>
#include <dmzEventModule.h>
#include <dmzEventObserverUtil.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimeEventType.h>
#include <dmzTypesHashTableStringTemplate.h>
#include <dmzTypesHashTableHandleTemplate.h>

/*!

\class dmz::EventObserverUtil
\ingroup Event
\brief Event observer utility class.

*/

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
         const Config &Init,
         const String &LogName,
         RuntimeContext *context) :
         EventModuleName (config_to_string ("module.event.name", Init)),
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

   void release_all (EventObserver &obs) {

      if (module) { module->release_event_observer_all (obs); }

      table.empty ();
   }
};


/*!

\brief Constructor
\details The EventObserverUtil uses the Config object passed in to specify which
EventModule to register with. This is done by giving the name of the EventModule. The
format is as follows:
\code
<module>
   <event name="Event Module Name"/>
</module>
\endcode
If the EventModule name is not specified, the observer will register with the first
EventModule that discovers it.
\param[in] Info PluginInfo used to initialize the Plugin that is derived from this class.
\param[in] Init Config used to initialize the class. This is most often the Config object
passed in as local to the Plugin.

*/
dmz::EventObserverUtil::EventObserverUtil (
      const PluginInfo &Info,
      const Config &Init) :
      EventObserver (Info),
      __state (*(new State (Init, Info.get_name (), Info.get_context ()))) {

}


//! Destructor.
dmz::EventObserverUtil::~EventObserverUtil () {

   if (__state.module) { remove_event_module ("", *(__state.module)); }
   delete &__state;
}


/*!

\brief Activates callbacks for a specific event type.
\param[in] EventTypeName String containing the name of the EventType callback to
activate.
\param[in] CallbackMask Mask containing the callbacks to activate. The masks defined
in dmzEventCallbackMasks.h should be used to compose \a CallbackMask.
\return Returns the EventType.
\sa EventModule

*/
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


/*!

\brief Activates callbacks for a specific event type.
\param[in] Type EventType of callback to activate.
\param[in] CallbackMask Mask containing the callbacks to activate. The masks defined
in dmzEventCallbackMasks.h should be used to compose \a CallbackMask.
\note To subscribe to all events, Use the root event type which can be obtained from
the dmz::Definitions class.
\sa EventModule \n dmz::Definitions::get_root_event_type()

*/
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


/*!

\brief Deactivates callbacks for a specific event type.
\param[in] Type EventType of callback to deactivate.
\param[in] CallbackMask Mask containing the callbacks to deactivate. The masks defined
in dmzEventCallbackMasks.h should be used to compose \a CallbackMask.
\note The EventType must be the exact type used to activate callbacks. Sub EventTypes may
not be used to deactivate callbacks to subtypes of activate EventTypes.

*/
void
dmz::EventObserverUtil::deactivate_event_callback (
      const EventType &Type,
      const Mask &CallbackMask) {

   typeStruct *ts = __state.table.lookup (Type.get_handle ());

   if (ts) { __state.release_obs (CallbackMask, *ts, *this); }
}


/*!

\brief Deactivates callbacks for a specific event type.
\param[in] Type EventType of callback to deactivate.
\param[in] CallbackMask Mask containing the callbacks to deactivate. The masks defined
in dmzEventCallbackMasks.h should be used to compose \a CallbackMask.
\note The EventType must be the exact type used to activate callbacks. Sub EventTypes may
not be used to deactivate callbacks to subtypes of activate EventTypes.

*/
void
dmz::EventObserverUtil::deactivate_all_event_callbacks () {

   __state.release_all (*this);
}


/*!

\brief Gets event module with which the observer is registered.
\return Returns a pointer to the EventModule with which the observer is registered.
Returns NULL if the observer is not registered with any EventModule.

*/
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

         _store_event_module (module);
      }
   }
}


void
dmz::EventObserverUtil::remove_event_module (const String &Name, EventModule &module) {

   if (__state.module == &module) {

      _remove_event_module (module);
      __state.release_all (*this);
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

