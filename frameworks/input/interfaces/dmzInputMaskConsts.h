#ifndef DMZ_INPUT_MASK_CONSTS_DOT_H
#define DMZ_INPUT_MASK_CONSTS_DOT_H

#include <dmzTypesBase.h>

namespace dmz {

   const UInt32 InputEventChannelStateConst = 0x00000001;
   const UInt32 InputEventAxisConst         = 0x00000002;
   const UInt32 InputEventButtonConst       = 0x00000004;
   const UInt32 InputEventSwitchConst       = 0x00000008;
   const UInt32 InputEventKeyConst          = 0x00000010;
   const UInt32 InputEventMouseConst        = 0x00000020;
   const UInt32 InputEventDataConst         = 0x00000040;
   const UInt32 InputEventAllConst          = 0xffffffff;
};

#endif //  DMZ_INPUT_MASK_CONSTS_DOT_H
