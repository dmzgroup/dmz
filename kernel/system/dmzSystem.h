#ifndef DMZ_SYSTEM_DOT_H
#define DMZ_SYSTEM_DOT_H

#include <dmzKernelExport.h>
#include <dmzTypesBase.h>
#include <dmzTypesString.h>

namespace dmz {

   class UUID;

   enum ByteOrderEnum {
      ByteOrderBigEndian, //!< Big endian byte order.
      ByteOrderLittleEndian, //!< Little endian byte order.
   };

   DMZ_KERNEL_LINK_SYMBOL String get_system_name ();
   DMZ_KERNEL_LINK_SYMBOL ByteOrderEnum get_byte_order ();
   DMZ_KERNEL_LINK_SYMBOL Float64 get_time ();
   DMZ_KERNEL_LINK_SYMBOL void sleep (const Float64 Time);
   DMZ_KERNEL_LINK_SYMBOL String get_env (const String &Name);
   DMZ_KERNEL_LINK_SYMBOL void set_env (const String &Name, const String &Value);
   DMZ_KERNEL_LINK_SYMBOL Float64 random ();
   DMZ_KERNEL_LINK_SYMBOL Boolean create_uuid (UUID &uuid);
};

#endif // DMZ_SYSTEM_DOT_H
