#ifndef DMZ_EVENT_CONSTS_DOT_H
#define DMZ_EVENT_CONSTS_DOT_H

#include <dmzTypesBase.h>

namespace dmz {

   enum EventLocalityEnum { EventLocalityUnknown, EventLocal, EventRemote };

   const UInt32 EventAttributeStartEvent    = 0x00000001;
   const UInt32 EventAttributeEndEvent      = 0x00000002;
   const UInt32 EventAttributeObjectHandle  = 0x00000010;
   const UInt32 EventAttributeObjectType    = 0x00000020;
   const UInt32 EventAttributeState         = 0x00000040;
   const UInt32 EventAttributeTimeStamp     = 0x00000080;
   const UInt32 EventAttributePosition      = 0x00000100;
   const UInt32 EventAttributeOrientation   = 0x00000200;
   const UInt32 EventAttributeVelocity      = 0x00000400;
   const UInt32 EventAttributeAcceleration  = 0x00000800;
   const UInt32 EventAttributeScale         = 0x00001000;
   const UInt32 EventAttributeVector        = 0x00002000;
   const UInt32 EventAttributeScalar        = 0x00004000;
   const UInt32 EventAttributeText          = 0x00008000;
   const UInt32 EventAttributeData          = 0x00010000;
   const UInt32 EventAttributeAll           = 0xffffffff;

   const char EventAttributeDefaultName[]   = "default";
   const char EventAttributeSourceName[]    = "Source";
   const char EventAttributeTargetName[]    = "Target";
   const char EventAttributeMunitionName[]  = "Munition";
};

#endif // DMZ_EVENT_CONSTS_DOT_H
