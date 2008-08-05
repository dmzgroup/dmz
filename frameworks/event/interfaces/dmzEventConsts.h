#ifndef DMZ_EVENT_CONSTS_DOT_H
#define DMZ_EVENT_CONSTS_DOT_H

#include <dmzTypesBase.h>

namespace dmz {

   enum EventLocalityEnum { EventLocalityUnknown, EventLocal, EventRemote };

   const UInt32 EventCallbackStartEvent    = 0x00000001;
   const UInt32 EventCallbackEndEvent      = 0x00000002;
   const UInt32 EventCallbackAll           = 0xFFFFFFFF;

   const char EventAttributeDefaultName[]   = "default";
   const char EventAttributeSourceName[]    = "Source";
   const char EventAttributeTargetName[]    = "Target";
   const char EventAttributeMunitionName[]  = "Munition";
};

#endif // DMZ_EVENT_CONSTS_DOT_H
