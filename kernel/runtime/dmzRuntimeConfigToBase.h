#ifndef DMZ_RUNTIME_CONFIG_TO_BASE_DOT_H
#define DMZ_RUNTIME_CONFIG_TO_BASE_DOT_H

#include <dmzKernelExport.h>
#include <dmzTypesBase.h>
#include <dmzTypesString.h>

namespace dmz {

   class Config;

   BaseTypeEnum config_to_base_type_enum (const Config &Source);
   BaseTypeEnum config_to_base_type_enum (const String &Name, const Config &Source);

   DMZ_KERNEL_LINK_SYMBOL BaseTypeEnum config_to_base_type_enum (
      const String &Name,
      const Config &Source,
      const BaseTypeEnum DefaultValue);

   Boolean config_to_boolean (const Config &Source);
   Boolean config_to_boolean (const String &Name, const Config &Source);

   DMZ_KERNEL_LINK_SYMBOL Boolean config_to_boolean (
      const String &Name,
      const Config &Source,
      const Boolean DefaultValue);

   Int32 config_to_int32 (const Config &Source);
   Int32 config_to_int32 (const String &Name, const Config &Source);

   DMZ_KERNEL_LINK_SYMBOL Int32 config_to_int32 (
      const String &Name,
      const Config &Source,
      const Int32 DefaultValue);

   UInt32 config_to_uint32 (const Config &Source);
   UInt32 config_to_uint32 (const String &Name, const Config &Source);

   DMZ_KERNEL_LINK_SYMBOL UInt32 config_to_uint32 (
      const String &Name,
      const Config &Source,
      const UInt32 DefaultValue);

   Int64 config_to_int64 (const Config &Source);
   Int64 config_to_int64 (const String &Name, const Config &Source);

   DMZ_KERNEL_LINK_SYMBOL Int64 config_to_int64 (
      const String &Name,
      const Config &Source,
      const Int64 DefaultValue);

   UInt64 config_to_uint64 (const Config &Source);
   UInt64 config_to_uint64 (const String &Name, const Config &Source);

   DMZ_KERNEL_LINK_SYMBOL UInt64 config_to_uint64 (
      const String &Name,
      const Config &Source,
      const UInt64 DefaultValue);

   Float32 config_to_float32 (const Config &Source);
   Float32 config_to_float32 (const String &Name, const Config &Source);

   DMZ_KERNEL_LINK_SYMBOL Float32 config_to_float32 (
      const String &Name,
      const Config &Source,
      const Float32 DefaultValue);

   Float64 config_to_float64 (const Config &Source);
   Float64 config_to_float64 (const String &Name, const Config &Source);

   DMZ_KERNEL_LINK_SYMBOL Float64 config_to_float64 (
      const String &Name,
      const Config &Source,
      const Float64 DefaultValue);

   String config_to_string (const Config &Source);
   String config_to_string (const String &Name, const Config &Source);

   DMZ_KERNEL_LINK_SYMBOL String config_to_string (
      const String &Name,
      const Config &Source,
      const String &DefaultValue);
};


inline dmz::BaseTypeEnum
dmz::config_to_base_type_enum (const Config &Source) {

   const BaseTypeEnum Type (BaseTypeUnknown);
   return config_to_base_type_enum ("", Source, Type);
}


inline dmz::BaseTypeEnum
dmz::config_to_base_type_enum (const String &Name, const Config &Source) {

   const BaseTypeEnum Type (BaseTypeUnknown);
   return config_to_base_type_enum (Name, Source, Type);
}


inline dmz::Boolean
dmz::config_to_boolean (const Config &Source) {

   return config_to_boolean ("", Source, False);
}


inline dmz::Boolean
dmz::config_to_boolean (const String &Name, const Config &Source) {

   return config_to_boolean (Name, Source, False);
}


inline dmz::Int32
dmz::config_to_int32 (const Config &Source) {

   return config_to_int32 ("", Source, 0);
}


inline dmz::Int32
dmz::config_to_int32 (const String &Name, const Config &Source) {

   return config_to_int32 (Name, Source, 0);
}


inline dmz::UInt32
dmz::config_to_uint32 (const Config &Source) {

   return config_to_uint32 ("", Source, 0);
}


inline dmz::UInt32
dmz::config_to_uint32 (const String &Name, const Config &Source) {

   return config_to_uint32 (Name, Source, 0);
}


inline dmz::Int64
dmz::config_to_int64 (const Config &Source) {

   return config_to_int64 ("", Source, 0);
}


inline dmz::Int64
dmz::config_to_int64 (const String &Name, const Config &Source) {

   return config_to_int64 (Name, Source, 0);
}


inline dmz::UInt64
dmz::config_to_uint64 (const Config &Source) {

   return config_to_uint64 ("", Source, 0);
}


inline dmz::UInt64
dmz::config_to_uint64 (const String &Name, const Config &Source) {

   return config_to_uint64 (Name, Source, 0);
}


inline dmz::Float32
dmz::config_to_float32 (const Config &Source) {

   return config_to_float32 ("", Source, 0.0f);
}


inline dmz::Float32
dmz::config_to_float32 (const String &Name, const Config &Source) {

   return config_to_float32 (Name, Source, 0.0f);
}


inline dmz::Float64
dmz::config_to_float64 (const Config &Source) {

   return config_to_float64 ("", Source, 0.0);
}


inline dmz::Float64
dmz::config_to_float64 (const String &Name, const Config &Source) {

   return config_to_float64 (Name, Source, 0.0);
}


inline dmz::String
dmz::config_to_string (const Config &Source) {

   return config_to_string ("", Source, "");
}


inline dmz::String
dmz::config_to_string (const String &Name, const Config &Source) {

   return config_to_string (Name, Source, "");
}

#endif // DMZ_RUNTIME_CONFIG_TO_BASE_DOT_H

