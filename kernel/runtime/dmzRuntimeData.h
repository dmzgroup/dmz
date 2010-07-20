#ifndef DMZ_RUNTIME_DATA_DOT_H
#define DMZ_RUNTIME_DATA_DOT_H

#include <dmzKernelExport.h>
#include <dmzSystemStream.h>
#include <dmzTypesBase.h>
#include <dmzTypesBooleanOperator.h>
#include <dmzTypesString.h>

namespace dmz {

   class Config;
   class Log;
   class Mask;
   class Matrix;
   class RuntimeContext;
   class RuntimeIterator;
   class Stream;
   class UUID;
   class Vector;

   class DMZ_KERNEL_LINK_SYMBOL Data {

      public:
         Data (RuntimeContext *context = 0);
         Data (const Data &Value);
         ~Data ();

         Data &operator= (const Data &Value);
         Boolean operator== (const Data &Value) const;
         Boolean operator!= (const Data &Value) const;
         Boolean operator! () const;
         DMZ_BOOLEAN_OPERATOR;

         void set_runtime_context (RuntimeContext *context);
         RuntimeContext *get_runtime_context () const;

         void clear ();

         Handle get_first_attribute (RuntimeIterator &it) const;
         Handle get_next_attribute (RuntimeIterator &it) const;

         Int32 get_attribute_count () const;
         Int32 lookup_attribute_element_count (const Handle AttrHandle) const;
         BaseTypeEnum lookup_attribute_base_type_enum (const Handle AttrHandle) const;

         Boolean store_boolean (
            const Handle AttrHandle,
            const Int32 Element,
            const Boolean Value);

         Boolean lookup_boolean (
            const Handle AttrHandle,
            const Int32 Element,
            Boolean &value) const;

         Boolean store_uint32 (
            const Handle AttrHandle,
            const Int32 Element,
            const UInt32 Value);

         Boolean lookup_uint32 (
            const Handle AttrHandle,
            const Int32 Element,
            UInt32 &value) const;

         Boolean store_uint64 (
            const Handle AttrHandle,
            const Int32 Element,
            const UInt64 Value);

         Boolean lookup_uint64 (
            const Handle AttrHandle,
            const Int32 Element,
            UInt64 &value) const;

         Boolean store_int32 (
            const Handle AttrHandle,
            const Int32 Element,
            const Int32 Value);

         Boolean lookup_int32 (
            const Handle AttrHandle,
            const Int32 Element,
            Int32 &value) const;

         Boolean store_int64 (
            const Handle AttrHandle,
            const Int32 Element,
            const Int64 Value);

         Boolean lookup_int64 (
            const Handle AttrHandle,
            const Int32 Element,
            Int64 &value) const;

         Boolean store_float32 (
            const Handle AttrHandle,
            const Int32 Element,
            const Float32 Value);

         Boolean lookup_float32 (
            const Handle AttrHandle,
            const Int32 Element,
            Float32 &value) const;

         Boolean store_float64 (
            const Handle AttrHandle,
            const Int32 Element,
            const Float64 Value);

         Boolean lookup_float64 (
            const Handle AttrHandle,
            const Int32 Element,
            Float64 &value) const;

         Boolean store_string (
            const Handle AttrHandle,
            const Int32 Element,
            const String &Value);

         Boolean store_string (
            const Handle AttrHandle,
            const Int32 Element,
            const BaseTypeEnum Type,
            const String &Value);

         Boolean lookup_string (
            const Handle AttrHandle,
            const Int32 Element,
            String &value) const;

         Boolean store_vector (
            const Handle AttrHandle,
            const Int32 Element,
            const Vector &Value);

         Boolean lookup_vector (
            const Handle AttrHandle,
            const Int32 Element,
            Vector &value) const;

         Boolean store_matrix (
            const Handle AttrHandle,
            const Int32 Element,
            const Matrix &Value);

         Boolean lookup_matrix (
            const Handle AttrHandle,
            const Int32 Element,
            Matrix &value) const;

         Boolean store_uuid (
            const Handle AttrHandle,
            const Int32 Element,
            const UUID &Value);

         Boolean lookup_uuid (
            const Handle AttrHandle,
            const Int32 Element,
            UUID &value) const;

         Boolean store_handle (
            const Handle AttrHandle,
            const Int32 Element,
            const Handle &Value);

         Boolean lookup_handle (
            const Handle AttrHandle,
            const Int32 Element,
            Handle &value) const;

         Boolean store_mask (const Handle AttrHandle, const Mask &Value);
         Boolean lookup_mask (const Handle AttrHandle, Mask &value) const;

      protected:
         struct State;
         State &_state; //!< Internal state.
   };

   DMZ_KERNEL_LINK_SYMBOL Boolean config_to_data (
      const String &Name,
      const Config &Source,
      RuntimeContext *context,
      Data &target,
      Log *log = 0);

   Boolean config_to_data (
      const Config &Source,
      RuntimeContext *context,
      Data &target,
      Log *log = 0);
};


inline dmz::Boolean
dmz::config_to_data (
      const Config &Source,
      RuntimeContext *context,
      Data &target,
      Log *log) { return config_to_data ("", Source, context, target, log); }


DMZ_KERNEL_LINK_SYMBOL dmz::Stream &
operator<< (dmz::Stream &stream, const dmz::Data &Data);

#endif //  DMZ_RUNTIME_DATA_DOT_H
