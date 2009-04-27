#ifndef DMZ_EVENT_CONSTS_DOT_H
#define DMZ_EVENT_CONSTS_DOT_H

#include <dmzTypesBase.h>

/*!

\file dmzEventConsts.h
\ingroup Event
\brief Contains event enumeration, attribute names, and event type names.

*/

namespace dmz {

//! \addtogroup Event
//! @{

   //! Event locality enumeration.
   enum EventLocalityEnum {
      EventLocalityUnknown, //!< Unknown Locality.
      EventLocal,           //!< Event is local.
      EventRemote,          //!< Event is remote.
   };

   //! \cond
   const UInt32 EventCallbackCreateEvent = 0x01;
   const UInt32 EventCallbackCloseEvent  = 0x02;
   const UInt32 EventCallbackAll         = 0xFFFFFFFF;
   //! \endcond

   //! Event default attribute handle name.
   const char EventAttributeDefaultName[]   = "Event_Default_Attribute";
   //! Event source attribute handle name.
   const char EventAttributeSourceName[]    = "Event_Source_Attribute";
   //! Event target attribute handle name.
   const char EventAttributeTargetName[]    = "Event_Target_Attribute";
   //! Event munitions attribute handle name.
   const char EventAttributeMunitionsName[] = "Event_Munitions_Attribute";

   //! Event supplies count attribute handle name.
   const char EventAttributeSuppliesCountName[] = "Event_Supplies_Count_Attribute";
   //! Event supplies attribute handle root.
   const char EventAttributeSuppliesRoot[] = "Event_Service_Supplies_";

   //! Launch event name.
   const char EventLaunchName[]     = "Event_Launch";
   //! Detonation event name.
   const char EventDetonationName[] = "Event_Detonation";
   //! Collision event name.
   const char EventCollisionName[]  = "Event_Collision";

   //! Service request event name.
   const char EventServiceRequestName[] = "Event_Service_Request";
   //! Service resupply offer event name.
   const char EventServiceResupplyOfferName[] = "Event_Service_Resupply_Offer";
   //! Service resupply received event name.
   const char EventServiceResupplyReceivedName[] = "Event_Service_Resupply_Received";

//! @}
};

#endif // DMZ_EVENT_CONSTS_DOT_H
