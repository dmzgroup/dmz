#ifndef DMZ_OBJECT_MASK_CONSTS_DOT_H
#define DMZ_OBJECT_MASK_CONSTS_DOT_H

#include <dmzTypesBase.h>
#include <dmzTypesString.h>

/*

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

NOTE: DO NOT USE THESE FOR OBJECT ATTRIBUTE SUBSCRIPTION! USE THE MASKS DEFINED IN:

   dmzObjectAttributeMasks.h

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

*/

namespace dmz {

   const UInt32 ObjectAttributeCreateObject  = 0x00000001;
   const UInt32 ObjectAttributeDestroyObject = 0x00000002;
   const UInt32 ObjectAttributeUUID          = 0x00000004;
   const UInt32 ObjectAttributeRemove        = 0x00000008;
   const UInt32 ObjectAttributeLink          = 0x00000010;
   const UInt32 ObjectAttributeUnlink        = 0x00000020;
   const UInt32 ObjectAttributeLinkObject    = 0x00000040;
   const UInt32 ObjectAttributeLocality      = 0x00000080;
   const UInt32 ObjectAttributeObjectType    = 0x00000100;
   const UInt32 ObjectAttributeState         = 0x00000200;
   const UInt32 ObjectAttributeFlag          = 0x00000400;
   const UInt32 ObjectAttributeTimeStamp     = 0x00000800;
   const UInt32 ObjectAttributePosition      = 0x00001000;
   const UInt32 ObjectAttributeOrientation   = 0x00002000;
   const UInt32 ObjectAttributeVelocity      = 0x00004000;
   const UInt32 ObjectAttributeAcceleration  = 0x00008000;
   const UInt32 ObjectAttributeScale         = 0x00010000;
   const UInt32 ObjectAttributeVector        = 0x00020000;
   const UInt32 ObjectAttributeScalar        = 0x00040000;
   const UInt32 ObjectAttributeText          = 0x00080000;
   const UInt32 ObjectAttributeData          = 0x00100000;
   const UInt32 ObjectAttributeAll           = 0xffffffff;
};

#endif // DMZ_OBJECT_MASK_CONSTS_DOT_H
