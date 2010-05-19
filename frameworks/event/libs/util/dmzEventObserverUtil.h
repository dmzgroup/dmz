#ifndef DMZ_EVENT_OBSERVER_UTIL_DOT_H
#define DMZ_EVENT_OBSERVER_UTIL_DOT_H

#include <dmzEventUtilExport.h>
#include <dmzEventObserver.h>
#include <dmzTypesMask.h>

namespace dmz {

   class DMZ_EVENT_UTIL_LINK_SYMBOL EventObserverUtil : public EventObserver {

      public:
         EventType activate_event_callback (
            const String &EventTypeName,
            const Mask &CallbackMask);

         void activate_event_callback (
            const EventType &Type,
            const Mask &CallbackMask);

         void deactivate_event_callback (
            const EventType &Type,
            const Mask &CallbackMask);

         void deactivate_all_event_callbacks ();

         EventModule *get_event_module ();

         virtual void store_event_module (const String &Name, EventModule &module);
         virtual void remove_event_module (const String &Name, EventModule &module);

         virtual void create_event (
            const Handle EventHandle,
            const EventType &Type,
            const EventLocalityEnum Locality);

         virtual void close_event (
            const Handle EventHandle,
            const EventType &Type,
            const EventLocalityEnum Locality);

      protected:
         EventObserverUtil (const PluginInfo &Info, const Config &Init);
         ~EventObserverUtil ();

         virtual void _store_event_module (EventModule &eventMod);
         virtual void _remove_event_module (EventModule &eventMod);

      private:
         struct State;
         EventObserverUtil ();
         EventObserverUtil (const EventObserverUtil &);
         EventObserverUtil &operator= (const EventObserverUtil &);
         State &__state;
   };
};

#endif // DMZ_EVENT_OBSERVER_UTIL_DOT_H

