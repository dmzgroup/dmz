#ifndef DMZ_RUNTIME_TO_CONFIG_DOT_H
#define DMZ_RUNTIME_TO_CONFIG_DOT_H

#include <dmzKernelExport.h>
#include <dmzRuntimeConfig.h>

namespace dmz {

class RuntimeContext;

DMZ_KERNEL_LINK_SYMBOL Config runtime_to_config (RuntimeContext *context);
DMZ_KERNEL_LINK_SYMBOL Config runtime_definitions_to_config (RuntimeContext *context);
DMZ_KERNEL_LINK_SYMBOL Config runtime_states_to_config (RuntimeContext *context);
DMZ_KERNEL_LINK_SYMBOL Config runtime_messages_to_config (RuntimeContext *context);
DMZ_KERNEL_LINK_SYMBOL Config runtime_object_types_to_config (RuntimeContext *context);
DMZ_KERNEL_LINK_SYMBOL Config runtime_event_types_to_config (RuntimeContext *context);
DMZ_KERNEL_LINK_SYMBOL Config runtime_time_to_config (RuntimeContext *context);
DMZ_KERNEL_LINK_SYMBOL Config runtime_resources_to_config (RuntimeContext *context);

};

#endif // DMZ_RUNTIME_TO_CONFIG_DOT_H
