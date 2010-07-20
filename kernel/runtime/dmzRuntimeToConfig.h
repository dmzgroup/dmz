#ifndef DMZ_RUNTIME_TO_CONFIG_DOT_H
#define DMZ_RUNTIME_TO_CONFIG_DOT_H

#include <dmzKernelExport.h>
#include <dmzRuntimeConfig.h>

namespace dmz {

class RuntimeContext;

DMZ_KERNEL_LINK_SYMBOL Config runtime_to_config (RuntimeContext *context);
DMZ_KERNEL_LINK_SYMBOL Config definitions_to_config (RuntimeContext *context);
DMZ_KERNEL_LINK_SYMBOL Config time_to_config (RuntimeContext *context);
DMZ_KERNEL_LINK_SYMBOL Config resource_to_config (RuntimeContext *context);

};

#endif // DMZ_RUNTIME_TO_CONFIG_DOT_H
