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

VectorComponentEnum config_to_vector_component (const Config &Source);

VectorComponentEnum config_to_vector_component (
   const String &Name,
   const Config &Source);

VectorComponentEnum config_to_vector_component (
   const Config &Source,
   const VectorComponentEnum DefaultValue);

DMZ_KERNEL_LINK_SYMBOL VectorComponentEnum config_to_vector_component (
   const String &Name,
   const Config &Source,
   const VectorComponentEnum DefaultValue);
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


inline dmz::VectorComponentEnum
dmz::config_to_vector_component (const Config &Source) {

   const VectorComponentEnum Value (VectorComponentX);
   return config_to_vector_component ("", Source, Value);
}


inline dmz::VectorComponentEnum
dmz::config_to_vector_component (
      const String &Name,
      const Config &Source) {

   const VectorComponentEnum Value (VectorComponentX);
   return config_to_vector_component (Name, Source, Value);
}


inline dmz::VectorComponentEnum
dmz::config_to_vector_component (
      const Config &Source,
      const VectorComponentEnum DefaultValue) {

   return config_to_vector_component ("", Source, DefaultValue);
}

#endif // DMZ_RUNTIME_CONFIG_TO_VECTOR_DOT_H

