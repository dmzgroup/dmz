#ifndef DMZ_RUNTIME_RTTI_NAMED_DOT_H
#define DMZ_RUNTIME_RTTI_NAMED_DOT_H

#include <dmzKernelExport.h>
#include <dmzTypesString.h>

namespace dmz {

   class RuntimeContext;

   DMZ_KERNEL_LINK_SYMBOL Boolean store_rtti_named_interface (
      const String &InterfaceName,
      RuntimeContext *context,
      void *ptr);

   DMZ_KERNEL_LINK_SYMBOL void *lookup_rtti_named_interface (
      const String &InterfaceName,
      RuntimeContext *context);

   DMZ_KERNEL_LINK_SYMBOL void *remove_rtti_named_interface (
      const String &InterfaceName,
      RuntimeContext *context,
      void *ptr);
};

#endif // DMZ_RUNTIME_RTTI_NAMED_DOT_H
