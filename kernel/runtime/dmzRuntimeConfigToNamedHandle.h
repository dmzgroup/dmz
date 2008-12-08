#ifndef DMZ_RUNTIME_CONFIG_TO_NAMED_HANDLE_DOT_H
#define DMZ_RUNTIME_CONFIG_TO_NAMED_HANDLE_DOT_H

#include <dmzKernelExport.h>
#include <dmzTypesBase.h>
#include <dmzTypesHandleContainer.h>
#include <dmzTypesString.h>

namespace dmz {

   class Config;
   class Log;
   class RuntimeContext;

   DMZ_KERNEL_LINK_SYMBOL Handle config_to_named_handle (
      const String &Name,
      const Config &Source,
      const String &DefaultValue,
      RuntimeContext *context);

   Handle config_to_named_handle (
      const String &Name,
      const Config &Source,
      RuntimeContext *context);

   DMZ_KERNEL_LINK_SYMBOL HandleContainer config_to_handle_container (
      const String &Name,
      const Config &Source,
      RuntimeContext *context);
};


inline dmz::Handle
dmz::config_to_named_handle (
      const String &Name,
      const Config &Source,
      RuntimeContext *context) {

   return config_to_named_handle (Name, Source, "", context);
}

#endif // DMZ_RUNTIME_CONFIG_TO_NAMED_HANDLE_DOT_H

