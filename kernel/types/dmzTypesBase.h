#ifndef DMZ_TYPES_BASE_DOT_H
#define DMZ_TYPES_BASE_DOT_H

#include <dmzKernelExport.h>

#ifdef _WIN32
#pragma warning (disable : 4275)

// these macro must be used to initialize 64 bit wide data
// otherwise the compiler will truncate it
// ex: dmz::Int64 value = DMZ_INT64_DATA (0x123456789abcdef0);

#define DMZ_UINT64_DATA(x) x ## ui64
#define DMZ_INT64_DATA(x) x ## i64

#else

//! This macro must be used to initialize 64 bit wide unsigned integers.
#define DMZ_UINT64_DATA(x) x ## ull
//! This macro must be used to initialize 64 bit wide signed integers.
#define DMZ_INT64_DATA(x) x ## ll

#endif

namespace dmz {

   class String;
   class Vector;

//! \addtogroup Types
//! @{

   enum BaseTypeEnum {
      BaseTypeBoolean, //!< dmz::Boolean
      BaseTypeInt8, //!< dmz::Int8
      BaseTypeInt16, //!< dmz::Int16
      BaseTypeInt32, //!< dmz::Int32
      BaseTypeInt64, //!< dmz::Int64
      BaseTypeUInt8, //!< dmz::UInt8
      BaseTypeUInt16, //!< dmz::UInt16
      BaseTypeUInt32, //!< dmz::UInt32
      BaseTypeUInt64, //!< dmz::UInt64
      BaseTypeFloat32, //!< dmz::Float32
      BaseTypeFloat64, //!< dmz::Float64
      BaseTypeString, //!< dmz::String
      BaseTypeUnknown //!< Unknown Type
   };

   typedef signed char Int8; //!< 8 Bit signed integer.
   typedef short Int16; //!< 16 Bit signed integer.
   typedef int Int32; //!< 32 Bit signed integer.

#ifdef _WIN32
   typedef __int64 Int64;
#else
   typedef long long Int64; //!< 64 Bit signed integer.
#endif

   typedef unsigned char UInt8; //!< 8 Bit unsigned integer.
   typedef unsigned short UInt16; //!< 16 Bit unsigned integer.
   typedef unsigned int UInt32; //!< 32 Bit unsigned integer.

#if _WIN32
   typedef unsigned __int64 UInt64;
#else
   typedef unsigned long long UInt64; //!< 64 Bit unsigned integer.
#endif

   typedef float Float32; //!< 32 Bit floating point.
   typedef double Float64; //!< 64 Bit floating point.

   DMZ_KERNEL_LINK_SYMBOL extern const Int8 MinInt8;
   DMZ_KERNEL_LINK_SYMBOL extern const Int8 MaxInt8;
   DMZ_KERNEL_LINK_SYMBOL extern const UInt8 MaxUInt8;
   DMZ_KERNEL_LINK_SYMBOL extern const Int16 MinInt16;
   DMZ_KERNEL_LINK_SYMBOL extern const Int16 MaxInt16;
   DMZ_KERNEL_LINK_SYMBOL extern const UInt16 MaxUInt16;
   DMZ_KERNEL_LINK_SYMBOL extern const Int32 MinInt32;
   DMZ_KERNEL_LINK_SYMBOL extern const Int32 MaxInt32;
   DMZ_KERNEL_LINK_SYMBOL extern const UInt32 MaxUInt32;
   DMZ_KERNEL_LINK_SYMBOL extern const Int64 MinInt64;
   DMZ_KERNEL_LINK_SYMBOL extern const Int64 MaxInt64;
   DMZ_KERNEL_LINK_SYMBOL extern const UInt64 MaxUInt64;

   typedef bool Boolean; //!< Boolean type values. Compatible with built in type bool.
   DMZ_KERNEL_LINK_SYMBOL extern const Boolean False;
   DMZ_KERNEL_LINK_SYMBOL extern const Boolean True;

   typedef unsigned int Handle; //!< Handle type.

   DMZ_KERNEL_LINK_SYMBOL BaseTypeEnum string_to_base_type_enum  (const String &Value);
   DMZ_KERNEL_LINK_SYMBOL String base_type_enum_to_string  (const BaseTypeEnum Value);

   DMZ_KERNEL_LINK_SYMBOL Boolean string_to_boolean (const String &Value);

   DMZ_KERNEL_LINK_SYMBOL Int32 string_to_int32 (const String &Value);
   DMZ_KERNEL_LINK_SYMBOL UInt32 string_to_uint32 (const String &Value);

   DMZ_KERNEL_LINK_SYMBOL Int64 string_to_int64 (const String &Value);
   DMZ_KERNEL_LINK_SYMBOL UInt64 string_to_uint64 (const String &Value);

   DMZ_KERNEL_LINK_SYMBOL Float32 string_to_float32 (const String &Value);
   DMZ_KERNEL_LINK_SYMBOL Float64 string_to_float64 (const String &Value);
//! @}
};

#endif // DMZ_TYPES_BASE_DOT_H
