#ifndef DMZ_RUNTIME_CONFIG_TO_VECTOR_DOT_H
#define DMZ_RUNTIME_CONFIG_TO_VECTOR_DOT_H

#include <dmzKernelExport.h>
#include <dmzTypesBase.h>
#include <dmzTypesVector.h>

namespace dmz {

   class Config;

   Vector config_to_vector (const Config &Source);
   Vector config_to_vector (const String &Name, const Config &Source);
   Vector config_to_vector (const Config &Source, const Vector &DefaultValue);

   DMZ_KERNEL_LINK_SYMBOL Vector config_to_vector (
      const String &Name,
      const Config &Source,
      const Vector &DefaultValue);
};


inline dmz::Vector
dmz::config_to_vector (const Config &Source) {

   const Vector Value;
   return config_to_vector ("", Source, Value);
}


inline dmz::Vector
dmz::config_to_vector (const String &Name, const Config &Source) {

   const Vector Value;
   return config_to_vector (Name, Source, Value);
}


inline dmz::Vector
dmz::config_to_vector (const Config &Source, const Vector &Value) {

   return config_to_vector ("", Source, Value);
}

#endif // DMZ_RUNTIME_CONFIG_TO_VECTOR_DOT_H

