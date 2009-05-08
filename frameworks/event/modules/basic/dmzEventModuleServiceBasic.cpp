#include <dmzEventConsts.h>
#include <dmzEventModule.h>
#include "dmzEventModuleServiceBasic.h"
#include <dmzRuntimeConfigToNamedHandle.h>
#include <dmzRuntimeObjectType.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>

/*!

\class dmz::EventModuleServiceBasic
\ingroup Event
\brief Basic EventModuleService implementation.
\details
\code
<dmz>
<plugin-list>
   <plugin name="dmzEventModuleServiceBasic" scope="service-events"/>
</plugin-list>
<service-events>
   <event-request name="[Optional] ervice Request Event Name"/>
   <event-offer name="[Optional]Service Offer Supplies Event Name"/>
   <event-received name="[Optional]Service Received Supplies Event Name"/>
   <source-attribute name="[Optional]Event Source Object Attribute Name"/>
   <target-attribute name="[Optional]Event Target Object Attribute Name"/>
   <supplies-count-attribute name="[Optional]Event Supplies Count Attribute Name"/>
</service-events>
</dmz>
\endcode
The Config attribute "event-request.name" defaults to dmz::EventServiceRequestName.\n
The Config attribute "event-offer.name" defaults to dmz::EventServiceResupplyOfferName.\n
The Config attribute "event-received.name" defaults to dmz::EventServiceResupplyReceivedName.\n
The Config attribute "source-attribute.name" defaults to dmz::EventAttributeSourceName.\n
The Config attribute "target-attribute.name" defaults to dmz::EventAttributeTargetName.\n
The Config attribute "supplies-count-attribute.name" defaults to dmz::EventAttributeSuppliesCountName.\n
The root used in the attribute for supplies is dmz::EventAttributeSuppliesRoot with the
index appended to the end. For example, the supplies at index "1" would be "Event_Service_Supplies_1". The index starts a zero.\n
\sa EventModuleService

*/

//! \cond
dmz::EventModuleServiceBasic::EventModuleServiceBasic (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      EventModuleService (Info),
      _log (Info),
      _defs (Info, &_log),
      _event (0),
      _suppliesAttributeNameRoot (EventAttributeSuppliesRoot),
      _sourceHandle (0),
      _targetHandle (0),
      _countHandle (0) {

   _init (local);
}


dmz::EventModuleServiceBasic::~EventModuleServiceBasic () {

   _suppliesTable.empty ();
}


// Plugin Interface
void
dmz::EventModuleServiceBasic::update_plugin_state (
      const PluginStateEnum State,
      const UInt32 Level) {

   if (State == PluginStateInit) {

   }
   else if (State == PluginStateStart) {

   }
   else if (State == PluginStateStop) {

   }
   else if (State == PluginStateShutdown) {

   }
}


void
dmz::EventModuleServiceBasic::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      if (!_event) { _event = EventModule::cast (PluginPtr); }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_event && (_event == EventModule::cast (PluginPtr))) { _event = 0; }
   }
}


// EventModuleService Interface
dmz::Handle
dmz::EventModuleServiceBasic::create_open_service_request_event (
      const Handle Source,
      const Handle Target,
      const EventLocalityEnum Locality) {

   return _create_event (_requestType, Source, Target, Locality);
}


dmz::Handle
dmz::EventModuleServiceBasic::create_open_resupply_offer_event (
      const Handle Source,
      const Handle Target,
      const EventLocalityEnum Locality) {

   return _create_event (_offerType, Source, Target, Locality);
}


dmz::Handle
dmz::EventModuleServiceBasic::create_open_resupply_received_event (
      const Handle Source,
      const Handle Target,
      const EventLocalityEnum Locality) {

   return _create_event (_receivedType, Source, Target, Locality);
}


dmz::Boolean
dmz::EventModuleServiceBasic::close_service_event (const Handle EventHandle) {

   return _event ? _event->close_event (EventHandle) : False;
}


dmz::Handle
dmz::EventModuleServiceBasic::add_supplies_to_event (
      const Handle EventHandle,
      const ObjectType &Type,
      const UInt32 Quantity) {

   Handle result (0);

   if (_event) {

      Int64 count (0);

      _event->lookup_counter (EventHandle, _countHandle, count);

      count++;

      result = _create_supplies_handle ((UInt32)count);

      if (result) {

         if (_event->store_object_type (EventHandle, result, Type) &&
               _event->store_counter (EventHandle, result, Quantity)) {

            if (!_event->store_counter (EventHandle, _countHandle, count)) {

               result = 0;
            }
         }
         else { result = 0; }
      }
   }

   return result;
}


dmz::UInt32
dmz::EventModuleServiceBasic::lookup_supply_count (const Handle EventHandle) {

   Int64 result (0);

   if (_event && _countHandle) {

      _event->lookup_counter (EventHandle, _countHandle, result);
   }

   return UInt32 (result);
}


dmz::Handle
dmz::EventModuleServiceBasic::lookup_supply (
      const Handle EventHandle,
      const UInt32 Which,
      ObjectType &type,
      UInt32 &quantity) {

   Handle result (0);

   if (_event) {

      Handle attr = _create_supplies_handle (Which);

      if (attr) {

         ObjectType ftype;
         Int64 fquantity (0);

         if (_event->lookup_object_type (EventHandle, attr, ftype) &&
               _event->lookup_counter (EventHandle, attr, fquantity)) {

            type = ftype;
            quantity = (UInt32)fquantity;
            result = attr;
         }
      }
   }

   return result;
}


dmz::Handle
dmz::EventModuleServiceBasic::_create_event (
      const EventType &Type, 
      const Handle Source, 
      const Handle Target,
      const EventLocalityEnum Locality) {

   Handle result (0);

   if (Type && _event) {

      result = _event->create_event (Type, Locality);

      _event->store_object_handle (result, _sourceHandle, Source);
      _event->store_object_handle (result, _targetHandle, Target);
   }

   return result;
}


dmz::Handle
dmz::EventModuleServiceBasic::_create_supplies_handle (const UInt32 Which) {

   Handle *ptr = _suppliesTable.lookup (Which);

   if (!ptr) {

      String name (_suppliesAttributeNameRoot);
      name << Which;
      ptr = new Handle (_defs.create_named_handle (name));

      if (ptr && !_suppliesTable.store (Which, ptr)) { delete ptr; ptr = 0; }
   }

   return ptr ? *ptr : 0;
}


void
dmz::EventModuleServiceBasic::_init (Config &local) {

   RuntimeContext *context (get_plugin_runtime_context ());

   _requestType = config_to_event_type (
      "event-request.name",
      local,
      EventServiceRequestName,
      context);

   _offerType = config_to_event_type (
      "event-offer.name",
      local,
      EventServiceResupplyOfferName,
      context);

   _receivedType = config_to_event_type (
      "event-received.name",
      local,
      EventServiceResupplyReceivedName,
      context);

   _sourceHandle = config_to_named_handle (
      "source-attribute.name",
      local,
      EventAttributeSourceName,
      context);

   _targetHandle = config_to_named_handle (
      "target-attribute.name",
      local,
      EventAttributeTargetName,
      context);

   _countHandle = config_to_named_handle (
      "supplies-count-attribute.name",
      local,
      EventAttributeSuppliesCountName,
      context);
}
//! \endcond


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzEventModuleServiceBasic (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::EventModuleServiceBasic (Info, local);
}

};
