#ifndef DMZ_TYPES_STRING_UTIL_DOT_H
#define DMZ_TYPES_STRING_UTIL_DOT_H

#include <dmzKernelExport.h>
#include <dmzTypesBase.h>

namespace dmz {

   class String;

   DMZ_KERNEL_LINK_SYMBOL Boolean is_ascii_white_space (const char Value);
   DMZ_KERNEL_LINK_SYMBOL Boolean is_ascii_white_space (const String &Value);
   DMZ_KERNEL_LINK_SYMBOL void convert_ascii_white_space (
      String &value,
      const char Target = ' ');
   DMZ_KERNEL_LINK_SYMBOL void trim_ascii_white_space (String &value);
};

#endif //  DMZ_TYPES_STRING_UTIL_DOT_H
