#ifndef DMZ_RUNTIME_CONFIG_TO_STRING_CONTAINER_DOT_H
#define DMZ_RUNTIME_CONFIG_TO_STRING_CONTAINER_DOT_H

#include <dmzKernelExport.h>
#include <dmzTypesString.h>
#include <dmzTypesStringContainer.h>

namespace dmz {

   class Config;
   class String;

   DMZ_KERNEL_LINK_SYMBOL StringContainer config_to_string_container (
      const String &Name,
      const Config &Source);

   StringContainer config_to_string_container (const Config &Source);
   StringContainer config_to_path_string_container (const Config &Source);
};


inline dmz::StringContainer
dmz::config_to_string_container (const Config &Source) {

   return config_to_string_container ("", Source);
}


inline dmz::StringContainer
dmz::config_to_path_string_container (const Config &Source) {

   return config_to_string_container ("path", Source);
}

#endif // DMZ_RUNTIME_CONFIG_TO_STRING_CONTAINER_DOT_H

