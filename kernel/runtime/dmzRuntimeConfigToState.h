#ifndef DMZ_RUNTIME_CONFIG_TO_STATE
#define DMZ_RUNTIME_CONFIG_TO_STATE

#include <dmzKernelExport.h>
#include <dmzTypesMask.h>

namespace dmz {

   class Config;
   class RuntimeContext;

   DMZ_KERNEL_LINK_SYMBOL Mask
   config_to_state (
      const String &Name,
      const Config &Source,
      const String &DefaultStateName,
      RuntimeContext *context);

   DMZ_KERNEL_LINK_SYMBOL Mask
   config_to_state (
      const String &Name,
      const Config &Source,
      const Mask &DefaultState,
      RuntimeContext *context);

   Mask config_to_state (
      const String &Name,
      const Config &Source,
      RuntimeContext *context);
};


inline dmz::Mask
dmz::config_to_state (
      const String &Name,
      const Config &Source,
      RuntimeContext *context) {

   const Mask EmptyMask;
   return config_to_state (Name, Source, EmptyMask, context);
}

#endif // DMZ_RUNTIME_CONFIG_TO_STATE
