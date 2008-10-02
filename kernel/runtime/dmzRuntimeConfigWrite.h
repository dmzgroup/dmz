#ifndef DMZ_RUNTIME_CONFIG_WRITE_DOT_H
#define DMZ_RUNTIME_CONFIG_WRITE_DOT_H

#include <dmzRuntimeConfig.h>
#include <dmzKernelExport.h>
#include <dmzTypesBase.h>

namespace dmz {

   class Data;
   class Log;
   class Matrix;
   class RuntimeContext;
   class String;
   class Vector;

   DMZ_KERNEL_LINK_SYMBOL Config vector_to_config (
      const String &Name,
      const Vector &Value);

   DMZ_KERNEL_LINK_SYMBOL Config matrix_to_config (
      const String &Name,
      const Matrix &Value);

   DMZ_KERNEL_LINK_SYMBOL Config base_type_enum_to_config (
      const String &Name,
      const String &AttrName,
      const BaseTypeEnum Value);

   Config base_type_enum_to_config (const String &Name, const BaseTypeEnum Value);

   DMZ_KERNEL_LINK_SYMBOL Config boolean_to_config (
      const String &Name,
      const String &AttrName,
      const Boolean Value);

   Config boolean_to_config (const String &Name, const Boolean Value);

   DMZ_KERNEL_LINK_SYMBOL Config int32_to_config (
      const String &Name,
      const String &AttrName,
      const Int32 Value);

   Config int32_to_config (const String &Name, const Int32 Value);

   DMZ_KERNEL_LINK_SYMBOL Config uint32_to_config (
      const String &Name,
      const String &AttrName,
      const UInt32 Value);

   Config uint32_to_config (const String &Name, const UInt32 Value);

   DMZ_KERNEL_LINK_SYMBOL Config int64_to_config (
      const String &Name,
      const String &AttrName,
      const Int64 Value);

   Config int64_to_config (const String &Name, const Int64 Value);

   DMZ_KERNEL_LINK_SYMBOL Config uint64_to_config (
      const String &Name,
      const String &AttrName,
      const UInt64 Value);

   Config uint64_to_config (const String &Name, const UInt64 Value);

   DMZ_KERNEL_LINK_SYMBOL Config float32_to_config (
      const String &Name,
      const String &AttrName,
      const Float32 Value);

   Config float32_to_config (const String &Name, const Float32 Value);

   DMZ_KERNEL_LINK_SYMBOL Config float64_to_config (
      const String &Name,
      const String &AttrName,
      const Float64 Value);

   Config float64_to_config (const String &Name, const Float64 Value);

   DMZ_KERNEL_LINK_SYMBOL Config string_to_config (
      const String &Name,
      const String &AttrName,
      const String &Value);

   Config string_to_config (const String &Name, const String &Value);

   DMZ_KERNEL_LINK_SYMBOL Config data_to_config (
      const Data &Source,
      RuntimeContext *context,
      Log *log = 0);
};


inline dmz::Config
dmz::base_type_enum_to_config (const String &Name, const BaseTypeEnum Value) {

   return base_type_enum_to_config (Name, "value", Value);
}


inline dmz::Config
dmz::boolean_to_config (const String &Name, const Boolean Value) {

   return boolean_to_config (Name, "value", Value);
}


inline dmz::Config
dmz::int32_to_config (const String &Name, const Int32 Value) {

   return int32_to_config (Name, "value", Value);
}


inline dmz::Config
dmz::uint32_to_config (const String &Name, const UInt32 Value) {

   return uint32_to_config (Name, "value", Value);
}


inline dmz::Config
dmz::int64_to_config (const String &Name, const Int64 Value) {

   return int64_to_config (Name, "value", Value);
}


inline dmz::Config
dmz::uint64_to_config (const String &Name, const UInt64 Value) {

   return uint64_to_config (Name, "value", Value);
}


inline dmz::Config
dmz::float32_to_config (const String &Name, const Float32 Value) {

   return float32_to_config (Name, "value", Value);
}


inline dmz::Config
dmz::float64_to_config (const String &Name, const Float64 Value) {

   return float64_to_config (Name, "value", Value);
}


inline dmz::Config
dmz::string_to_config (const String &Name, const String &Value) {

   return string_to_config (Name, "value", Value);
}

#endif // DMZ_RUNTIME_CONFIG_WRITE_DOT_H
