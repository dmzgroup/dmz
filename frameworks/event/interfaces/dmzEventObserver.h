#ifndef DMZ_EVENT_OBSERVER_DOT_H
#define DMZ_EVENT_OBSERVER_DOT_H

#include <dmzEventConsts.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzRuntimeRTTI.h>
#include <dmzTypesBase.h>
#include <dmzTypesString.h>

#define DMZ_EVENT_OBSERVER_INTERFACE_NAME "EventObserverInteface"

namespace dmz {

   class EventModule;
   class EventType;
   class RuntimeContext;

   class EventObserver {

      public:
         static EventObserver *cast (
            const Plugin *PluginPtr,
            const String &PluginName);

         static Boolean is_valid (const Handle ObserverHandle, RuntimeContext *context);

         Handle get_event_observer_handle ();

         virtual void store_event_module (const String &Name, EventModule &module) = 0;
         virtual void remove_event_module (const String &Name, EventModule &module) = 0;

         virtual void start_event (
            const Handle EventHandle,
            const EventType &Type,
            const EventLocalityEnum Locality) = 0;

         virtual void end_event (const Handle EventHandle, const EventType &Type) = 0;

      protected:
         EventObserver (const PluginInfo &Info);
         ~EventObserver ();

      private:
         EventObserver ();
         EventObserver (const EventObserver &);
         EventObserver &operator= (const EventObserver &);

         const PluginInfo &__Info;
   };
};


inline dmz::EventObserver *
dmz::EventObserver::cast (const Plugin *PluginPtr, const String &PluginName) {

   return (EventObserver *)lookup_rtti_interface (
      DMZ_EVENT_OBSERVER_INTERFACE_NAME,
      PluginName,
      PluginPtr);
}


dmz::Boolean
dmz::EventObserver::is_valid (const Handle ObserverHandle, RuntimeContext *context) {

   return lookup_rtti_interface (
      DMZ_EVENT_OBSERVER_INTERFACE_NAME,
      ObserverHandle,
      context) != 0;
}


inline
dmz::EventObserver::EventObserver (const PluginInfo &Info) :
      __Info (Info) {

   store_rtti_interface (DMZ_EVENT_OBSERVER_INTERFACE_NAME, __Info, (void *)this);
}


inline
dmz::EventObserver::~EventObserver () {

   remove_rtti_interface (DMZ_EVENT_OBSERVER_INTERFACE_NAME, __Info);
}


inline dmz::Handle
dmz::EventObserver::get_event_observer_handle () { return __Info.get_handle (); }

#endif // DMZ_EVENT_OBSERVER_DOT_H

