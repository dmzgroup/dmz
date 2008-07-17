#ifndef DMZ_RUNTIME_CONFIG_TO_PATH_CONTAINER_DOT_H
#define DMZ_RUNTIME_CONFIG_TO_PATH_CONTAINER_DOT_H

#include <dmzKernelExport.h>
#include <dmzSystemFile.h>
#include <dmzTypesString.h>

namespace dmz {

   class Config;
   class String;

   DMZ_KERNEL_LINK_SYMBOL PathContainer config_to_path_container (
      const String &Name,
      const Config &Source);

   PathContainer config_to_path_container (const Config &Source);
};


inline dmz::PathContainer
dmz::config_to_path_container (const Config &Source) {

   return config_to_path_container ("", Source);
}

#endif // DMZ_RUNTIME_CONFIG_TO_PATH_CONTAINER_DOT_H

