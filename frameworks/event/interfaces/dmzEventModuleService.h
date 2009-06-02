#ifndef DMZ_EVENT_MODULE_SERVICE_DOT_H
#define DMZ_EVENT_MODULE_SERVICE_DOT_H

#include <dmzRuntimePlugin.h>
#include <dmzRuntimeRTTI.h>
#include <dmzTypesBase.h>
#include <dmzTypesString.h>

namespace dmz {

   class ObjectType;

   class EventModuleService {

      public:
         static EventModuleService *cast (
            const Plugin *PluginPtr,
            const String &PluginName = "");

         String get_event_module_service_name () const;
         Handle get_event_module_service_handle () const;

         // EventModuleService Interface
         virtual Handle create_open_service_request_event (
            const Handle Source,
            const Handle Target,
            const EventLocalityEnum Locality) = 0;

         virtual Handle create_open_resupply_offer_event (
            const Handle Source,
            const Handle Target,
            const EventLocalityEnum Locality) = 0;

         virtual Handle create_open_resupply_received_event (
            const Handle Source,
            const Handle Target,
            const EventLocalityEnum Locality) = 0;

         virtual Boolean close_service_event (const Handle EventHandle) = 0;

         virtual Handle add_supplies_to_event (
            const Handle EventHandle,
            const ObjectType &Type,
            const UInt32 Quantity) = 0;

         virtual UInt32 lookup_supply_count (const Handle EventHandle) = 0;

         virtual Handle lookup_supply (
            const Handle EventHandle,
            const UInt32 Which,
            ObjectType &type,
            UInt32 &quantity) = 0;

      protected:
         EventModuleService (const PluginInfo &Info);
         ~EventModuleService ();

      private:
         EventModuleService ();
         EventModuleService (const EventModuleService &);
         EventModuleService &operator= (const EventModuleService &);

         const PluginInfo &__Info;
   };

   //! \cond
   const char EventModuleServiceInterfaceName[] = "EventModuleServiceInterface";
   //! \endcond
};


inline dmz::EventModuleService *
dmz::EventModuleService::cast (const Plugin *PluginPtr, const String &PluginName) {

   return (EventModuleService *)lookup_rtti_interface (
      EventModuleServiceInterfaceName,
      PluginName,
      PluginPtr);
}


inline
dmz::EventModuleService::EventModuleService (const PluginInfo &Info) :
      __Info (Info) {

   store_rtti_interface (EventModuleServiceInterfaceName, __Info, (void *)this);
}


inline
dmz::EventModuleService::~EventModuleService () {

   remove_rtti_interface (EventModuleServiceInterfaceName, __Info);
}


inline dmz::String
dmz::EventModuleService::get_event_module_service_name () const {

   return __Info.get_name ();
}


inline dmz::Handle
dmz::EventModuleService::get_event_module_service_handle () const {

   return __Info.get_handle ();
}

#endif // DMZ_EVENT_MODULE_SERVICE_DOT_H
