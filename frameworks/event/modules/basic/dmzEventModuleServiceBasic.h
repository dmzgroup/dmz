#ifndef DMZ_EVENT_MODULE_SERVICE_BASIC_DOT_H
#define DMZ_EVENT_MODULE_SERVICE_BASIC_DOT_H

#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzEventModuleService.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimeEventType.h>
#include <dmzTypesHashTableUInt32Template.h>

namespace dmz {

   class EventModule;

   class EventModuleServiceBasic :
         public Plugin,
         public EventModuleService {

      public:
         //! \cond
         EventModuleServiceBasic (const PluginInfo &Info, Config &local);
         ~EventModuleServiceBasic ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level);

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // EventModuleService Interface
         virtual Handle create_open_service_request_event (
            const Handle Source,
            const Handle Target,
            const EventLocalityEnum Locality);

         virtual Handle create_open_resupply_offer_event (
            const Handle Source,
            const Handle Target,
            const EventLocalityEnum Locality);

         virtual Handle create_open_resupply_received_event (
            const Handle Source,
            const Handle Target,
            const EventLocalityEnum Locality);

         virtual Boolean close_service_event (const Handle EventHandle);

         virtual Handle add_supplies_to_event (
            const Handle EventHandle,
            const ObjectType &Type,
            const UInt32 Quantity);

         virtual UInt32 lookup_supply_count (const Handle EventHandle);

         virtual Handle lookup_supply (
            const Handle EventHandle,
            const UInt32 Which,
            ObjectType &type,
            UInt32 &quantity);

      protected:
         Handle _create_event (
            const EventType &Type,
            const Handle Source,
            const Handle Target,
            const EventLocalityEnum Locality);

         Handle _create_supplies_handle (const UInt32 Which);
         void _init (Config &local);

         Log _log;
         Definitions _defs;

         EventModule *_event;

         EventType _requestType;
         EventType _offerType;
         EventType _receivedType;

         String _suppliesAttributeNameRoot;

         HashTableUInt32Template<Handle> _suppliesTable;

         Handle _sourceHandle;
         Handle _targetHandle;
         Handle _countHandle;
         //! \endcond

      private:
         EventModuleServiceBasic ();
         EventModuleServiceBasic (const EventModuleServiceBasic &);
         EventModuleServiceBasic &operator= (const EventModuleServiceBasic &);

   };
};

#endif // DMZ_EVENT_MODULE_SERVICE_BASIC_DOT_H
