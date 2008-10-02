/*

NOTE: This file only contains the documentation for the inline functions.
The non-inline functions are defined in dmzRuntimeConfigUtil.cpp.

*/

//! \addtogroup Runtime
//! @{

/*!

\fn dmz::Vector dmz::config_to_vector (const Config &Source)
\brief Converts Config to Vector.
\details Defined in dmzRuntimeConfigToTypesBase.h.
\overload
\sa dmz::config_to_vector(const String &Name, const Config &Source, const Vector &DefaultValue)

\fn dmz::Vector dmz::config_to_vector (const String &Name, const Config &Source)
\brief Converts Config to Vector.
\details Defined in dmzRuntimeConfigToTypesBase.h.
\overload
\sa dmz::config_to_vector(const String &Name, const Config &Source, const Vector &DefaultValue)

\fn dmz::Vector dmz::config_to_vector (const Config &Source, const Vector &Value)
\brief Converts Config to Vector.
\details Defined in dmzRuntimeConfigToTypesBase.h.
\overload
\sa dmz::config_to_vector(const String &Name, const Config &Source, const Vector &DefaultValue)

\fn dmz::Matrix dmz::config_to_matrix (const Config &Source)
\brief Converts Config to Matrix.
\details Defined in dmzRuntimeConfigToTypesBase.h.
\overload
\sa config_to_matrix(const String &Name, const Config &Source, const Matrix &DefaultValue)

\fn dmz::Matrix dmz::config_to_matrix (const String &Name, const Config &Source)
\brief Converts Config to Matrix.
\details Defined in dmzRuntimeConfigToTypesBase.h.
\overload
\sa config_to_matrix(const String &Name, const Config &Source, const Matrix &DefaultValue)

\fn dmz::BaseTypeEnum dmz::config_to_base_type_enum (const Config &Source)
\brief Converts Config to BaseTypeEnum.
\details Defined in dmzRuntimeConfigToTypesBase.h.
\overload
\sa config_to_base_type_enum(const String &Name, const Config &Source, const BaseTypeEnum DefaultValue)

\fn dmz::BaseTypeEnum dmz::config_to_base_type_enum (
const String &Name,
const Config &Source)
\brief Converts Config to BaseTypeEnum.
\details Defined in dmzRuntimeConfigToTypesBase.h.
\overload
\sa config_to_base_type_enum(const String &Name, const Config &Source, const BaseTypeEnum DefaultValue)

\fn dmz::Boolean dmz::config_to_boolean (const Config &Source)
\brief Converts Config to Boolean.
\details Defined in dmzRuntimeConfigToTypesBase.h.
\overload
\note This function assumes the attribute name is "value".
\sa config_to_boolean(const String &Name, const Config &Source, const Boolean DefaultValue)

\fn dmz::Boolean dmz::config_to_boolean (const String &Name, const Config &Source)
\brief Converts Config to Boolean.
\details Defined in dmzRuntimeConfigToTypesBase.h.
\overload
\sa config_to_boolean(const String &Name, const Config &Source, const Boolean DefaultValue)

\fn dmz::Int32 dmz::config_to_int32 (const Config &Source)
\brief Converts Config to Int32.
\details Defined in dmzRuntimeConfigToTypesBase.h.
\overload
\note This function assumes the attribute name is "value".
\sa config_to_int32(const String &Name, const Config &Source, const Int32 DefaultValue)

\fn dmz::Int32 dmz::config_to_int32 (const String &Name, const Config &Source)
\brief Converts Config to Int32.
\details Defined in dmzRuntimeConfigToTypesBase.h.
\overload
\sa config_to_int32(const String &Name, const Config &Source, const Int32 DefaultValue)

\fn dmz::UInt32 dmz::config_to_uint32 (const Config &Source)
\brief Converts Config to UInt32.
\details Defined in dmzRuntimeConfigToTypesBase.h.
\overload
\note This function assumes the attribute name is "value".
\sa config_to_uint32(const String &Name, const Config &Source, const UInt32 DefaultValue)

\fn dmz::UInt32 dmz::config_to_uint32 (const String &Name, const Config &Source)
\brief Converts Config to UInt32.
\details Defined in dmzRuntimeConfigToTypesBase.h.
\overload
\sa config_to_uint32(const String &Name, const Config &Source, const UInt32 DefaultValue)

\fn dmz::Int64 dmz::config_to_int64 (const Config &Source)
\brief Converts Config to Int64.
\details Defined in dmzRuntimeConfigToTypesBase.h.
\overload
\note This function assumes the attribute name is "value".
\sa config_to_int64(const String &Name, const Config &Source, const Int64 DefaultValue)

\fn dmz::Int64 dmz::config_to_int64 (const String &Name, const Config &Source)
\brief Converts Config to Int64.
\details Defined in dmzRuntimeConfigToTypesBase.h.
\overload
\sa config_to_int64(const String &Name, const Config &Source, const Int64 DefaultValue)

\fn dmz::UInt64 dmz::config_to_uint64 (const Config &Source)
\brief Converts Config to UInt64.
\details Defined in dmzRuntimeConfigToTypesBase.h.
\overload
\note This function assumes the attribute name is "value".
\sa config_to_uint64(const String &Name, const Config &Source, const UInt64 DefaultValue)

\fn dmz::UInt64 dmz::config_to_uint64 (const String &Name, const Config &Source)
\brief Converts Config to UInt64.
\details Defined in dmzRuntimeConfigToTypesBase.h.
\overload
\sa config_to_uint64(const String &Name, const Config &Source, const UInt64 DefaultValue)

\fn dmz::Float32 dmz::config_to_float32 (const Config &Source)
\brief Converts Config to Float32.
\details Defined in dmzRuntimeConfigToTypesBase.h.
\overload
\note This function assumes the attribute name is "value".
\sa config_to_float32(const String &Name, const Config &Source, const Float32 DefaultValue)

\fn dmz::Float32 dmz::config_to_float32 (const String &Name, const Config &Source)
\brief Converts Config to Float32.
\details Defined in dmzRuntimeConfigToTypesBase.h.
\overload
\sa config_to_float32(const String &Name, const Config &Source, const Float32 DefaultValue)

\fn dmz::Float64 dmz::config_to_float64 (const Config &Source)
\brief Converts Config to Float64.
\details Defined in dmzRuntimeConfigToTypesBase.h.
\overload
\note This function assumes the attribute name is "value".
\sa config_to_float64(const String &Name, const Config &Source, const Float64 DefaultValue)

\fn dmz::Float64 dmz::config_to_float64 (const String &Name, const Config &Source)
\brief Converts Config to Float64.
\details Defined in dmzRuntimeConfigToTypesBase.h.
\overload
\sa config_to_float64(const String &Name, const Config &Source, const Float64 DefaultValue)

\fn dmz::String dmz::config_to_string (const Config &Source)
\brief Converts Config to String.
\details Defined in dmzRuntimeConfigToTypesBase.h.
\overload
\note This function assumes the attribute name is "value".
\sa config_to_string(const String &Name, const Config &Source, const String &DefaultValue)

\fn dmz::String dmz::config_to_string (const String &Name, const Config &Source)
\brief Converts Config to String.
\details Defined in dmzRuntimeConfigToTypesBase.h.
\overload
\sa config_to_string(const String &Name, const Config &Source, const String &DefaultValue)

\fn dma::Handle dmz::config_to_named_handle (
const String &Name,
const Config &Source,
RuntimeContext *context)
\brief Converts Config to a named Handle.
\details Defined in dmzRuntimeConfigToTypesBase.h.
\overload
\sa  dmz::config_to_named_handle (
const String &Name,
const Config &Source,
const String &DefaultValue,
RuntimeContext *context)

\fn dmz::Boolean dmz::config_to_data (
      const Config &Source,
      RuntimeContext *context,
      Data &target,
      Log *log)
\brief Converts Config to Data.
\details Defined in dmzRuntimeConfigToTypesBase.h.
\overload
\sa config_to_data(const String &Name,
      const Config &Source,
      RuntimeContext *context,
      Data &target,
      Log *log)

*/

//! @}
