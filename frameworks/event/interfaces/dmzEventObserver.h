#ifndef DMZ_EVENT_OBSERVER_DOT_H
#define DMZ_EVENT_OBSERVER_DOT_H

#include <dmzEventConsts.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzRuntimeRTTI.h>
#include <dmzTypesBase.h>
#include <dmzTypesString.h>


namespace dmz {

   //! \cond
   const char EventObserverInterfaceName[] = "EventObserverInteface";
   //! \endcond

   class EventModule;
   class EventType;
   class RuntimeContext;

   class EventObserver {

      public:
         static EventObserver *cast (
            const Plugin *PluginPtr,
            const String &PluginName = "");

         static Boolean is_valid (const Handle ObserverHandle, RuntimeContext *context);

         String get_event_observer_name ();
         Handle get_event_observer_handle ();

         virtual void store_event_module (const String &Name, EventModule &module) = 0;
         virtual void remove_event_module (const String &Name, EventModule &module) = 0;

         virtual void create_event (
            const Handle EventHandle,
            const EventType &Type,
            const EventLocalityEnum Locality) = 0;

         virtual void close_event (
            const Handle EventHandle,
            const EventType &Type,
            const EventLocalityEnum Locality) = 0;

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
      EventObserverInterfaceName,
      PluginName,
      PluginPtr);
}


inline dmz::Boolean
dmz::EventObserver::is_valid (const Handle ObserverHandle, RuntimeContext *context) {

   return lookup_rtti_interface (
      EventObserverInterfaceName,
      ObserverHandle,
      context) != 0;
}


inline
dmz::EventObserver::EventObserver (const PluginInfo &Info) :
      __Info (Info) {

   store_rtti_interface (EventObserverInterfaceName, __Info, (void *)this);
}


inline
dmz::EventObserver::~EventObserver () {

   remove_rtti_interface (EventObserverInterfaceName, __Info);
}


inline dmz::String
dmz::EventObserver::get_event_observer_name () { return __Info.get_name (); }


inline dmz::Handle
dmz::EventObserver::get_event_observer_handle () { return __Info.get_handle (); }

#endif // DMZ_EVENT_OBSERVER_DOT_H

