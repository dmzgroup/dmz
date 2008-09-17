#ifndef DMZ_EVENT_CONSTS_DOT_H
#define DMZ_EVENT_CONSTS_DOT_H

#include <dmzTypesBase.h>

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
   const UInt32 EventCallbackCreateEvent    = 0x00000001;
   const UInt32 EventCallbackCloseEvent      = 0x00000002;
   const UInt32 EventCallbackAll           = 0xFFFFFFFF;
   //! \endcond

   //! Event default attribute handle name.
   const char EventAttributeDefaultName[]   = "Event_Default_Attribute";
   //! Event source attribute handle name.
   const char EventAttributeSourceName[]    = "Event_Source_Attribute";
   //! Event target attribute handle name.
   const char EventAttributeTargetName[]    = "Event_Target_Attribute";
   //! Event munitions attribute handle name.
   const char EventAttributeMunitionsName[] = "Event_Munitions_Attribute";

   //! Launch event name.
   const char EventLaunchName[]     = "Event_Launch";
   //! Detonation event name.
   const char EventDetonationName[] = "Event_Detonation";
   //! Collision event name.
   const char EventCollisionName[]  = "Event_Collision";
//! @}
};

#endif // DMZ_EVENT_CONSTS_DOT_H
