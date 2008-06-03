#ifndef DMZ_RUNTIME_SESSION_DOT_H
#define DMZ_RUNTIME_SESSION_DOT_H

#include <dmzKernelExport.h>
#include <dmzRuntimeConfig.h>
#include <dmzTypesBase.h>

namespace dmz {

   class PluginInfo;
   class RuntimeContext;

   DMZ_KERNEL_LINK_SYMBOL void set_session_config (
      RuntimeContext *context,
      Config &data);

   DMZ_KERNEL_LINK_SYMBOL Config get_session_config (
      const String &Name,
      RuntimeContext *context);

   DMZ_KERNEL_LINK_SYMBOL Config get_session_config (const PluginInfo &Info);
};
 
#endif // DMZ_RUNTIME_SESSION_DOT_H
