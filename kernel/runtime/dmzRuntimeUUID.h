#ifndef DMZ_RUNTIME_UUID_DOT_H
#define DMZ_RUNTIME_UUID_DOT_H

#include <dmzKernelExport.h>
#include <dmzTypesUUID.h>

namespace dmz {

   class PluginInfo;
   class RuntimeContext;

   DMZ_KERNEL_LINK_SYMBOL UUID get_runtime_uuid (RuntimeContext *context);
   DMZ_KERNEL_LINK_SYMBOL UUID get_runtime_uuid (const PluginInfo &Info);
};

#endif // DMZ_RUNTIME_UUID_DOT_H
