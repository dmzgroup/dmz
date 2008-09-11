#ifndef DMZ_EVENT_CONSTS_DOT_H
#define DMZ_EVENT_CONSTS_DOT_H

#include <dmzTypesBase.h>

namespace dmz {

   enum EventLocalityEnum { EventLocalityUnknown, EventLocal, EventRemote };

   const UInt32 EventCallbackCreateEvent    = 0x00000001;
   const UInt32 EventCallbackCloseEvent      = 0x00000002;
   const UInt32 EventCallbackAll           = 0xFFFFFFFF;

   const char EventAttributeDefaultName[]   = "Event_Default_Attribute";
   const char EventAttributeSourceName[]    = "Event_Source_Attribute";
   const char EventAttributeTargetName[]    = "Event_Target_Attribute";
   const char EventAttributeMunitionsName[] = "Event_Munitions_Attribute";

   const char EventLaunchName[]     = "Event_Launch";
   const char EventDetonationName[] = "Event_Detonation";
   const char EventCollisionName[]  = "Event_Collision";
};

#endif // DMZ_EVENT_CONSTS_DOT_H
