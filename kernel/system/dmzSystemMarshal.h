#ifndef DMZ_SYSTEM_MARSHAL_DOT_H
#define DMZ_SYSTEM_MARSHAL_DOT_H

#include <dmzSystem.h>
#include <dmzKernelExport.h>
#include <dmzTypesBase.h>

namespace dmz {

   class Matrix;
   class Quaternion;
   class String;
   class UUID;
   class Vector;

   class DMZ_KERNEL_LINK_SYMBOL Marshal {

      public:
         Marshal (const ByteOrderEnum Order);
         Marshal (const Marshal &Value);
         ~Marshal ();

         Marshal &operator= (const Marshal &Value);

         void reset ();
         void empty ();

         ByteOrderEnum get_byte_order () const;

         Boolean grow (const Int32 Size);
         Int32 get_size () const;

         Boolean set_place (const Int32 Place);
         Int32 get_place () const;

         Boolean set_length (const Int32 Length);
         Int32 get_length () const;
         char *get_buffer () const;
         char *get_buffer (Int32 &length) const;

         void set_next_int8 (const Int8 Value);
         void set_next_int16 (const Int16 Value);
         void set_next_int32 (const Int32 Value);
         void set_next_int64 (const Int64 Value);

         void set_next_uint8 (const UInt8 Value);
         void set_next_uint16 (const UInt16 Value);
         void set_next_uint32 (const UInt32 Value);
         void set_next_uint64 (const UInt64 Value);

         void set_next_float32 (const Float32 Value);
         void set_next_float64 (const Float64 Value);

         void set_next_string (const String &Value);
         void set_next_fixed_string (const String &Value, const Int32 Size);

         void set_next_uuid (const UUID &Value);

         // Marshals data as 32 bit floats
         void set_next_vector32 (const Vector &Value);
         void set_next_matrix32 (const Matrix &Value);
         void set_next_quaternion32 (const Quaternion &Value);

         // Marshals data as native 64 bit floats
         void set_next_vector (const Vector &Value);
         void set_next_matrix (const Matrix &Value);
         void set_next_quaternion (const Quaternion &Value);

      protected:
         struct State;
         State &_state; //!< Internal state.

      private:
         Marshal ();
   };

   //! \brief Use this interface in templates only.
   //! \ingroup System
   class MarshalWrap {

      protected:
         Marshal &_data; //!< Marshal object being wrapped.

      public:
         MarshalWrap (Marshal &data) : _data (data) {;} //!< Constructor.
         ~MarshalWrap () {;} //!< Destructor

         //! Wraps dmz::Marshal::set_next_int8.
         void set_next (const Int8 Value) { _data.set_next_int8 (Value); }
         //! Wraps dmz::Marshal::set_next_int16.
         void set_next (const Int16 Value) { _data.set_next_int16 (Value); }
         //! Wraps dmz::Marshal::set_next_int32.
         void set_next (const Int32 Value) { _data.set_next_int32 (Value); }
         //! Wraps dmz::Marshal::set_next_int64.
         void set_next (const Int64 Value) { _data.set_next_int64 (Value); }
         //! Wraps dmz::Marshal::set_next_uint8.
         void set_next (const UInt8 Value) { _data.set_next_uint8 (Value); }
         //! Wraps dmz::Marshal::set_next_uint16.
         void set_next (const UInt16 Value) { _data.set_next_uint16 (Value); }
         //! Wraps dmz::Marshal::set_next_uint32.
         void set_next (const UInt32 Value) { _data.set_next_uint32 (Value); }
         //! Wraps dmz::Marshal::set_next_uint64.
         void set_next (const UInt64 Value) { _data.set_next_uint64 (Value); }
         //! Wraps dmz::Marshal::set_next_float32.
         void set_next (const Float32 Value) { _data.set_next_float32 (Value); }
         //! Wraps dmz::Marshal::set_next_float64.
         void set_next (const Float64 Value) { _data.set_next_float64 (Value); }
         //! Wraps dmz::Marshal::set_next_string.
         void set_next (const String &Value) { _data.set_next_string (Value); }
         //! Wraps dmz::Marshal::set_next_uuid.
         void set_next (const UUID &Value) { _data.set_next_uuid (Value); }
         //! Wraps dmz::Marshal::set_next_vector.
         void set_next (const Vector &Value) { _data.set_next_vector (Value); }
         //! Wraps dmz::Marshal::set_next_matrix.
         void set_next (const Matrix &Value) { _data.set_next_matrix (Value); }
         //! Wraps dmz::Marshal::set_next_quaternion.
         void set_next (const Quaternion &Value) { _data.set_next_quaternion (Value); }

      private:
         MarshalWrap ();
         MarshalWrap (const MarshalWrap &);
         MarshalWrap& operator= (const MarshalWrap &);
   };
};

#endif // DMZ_SYSTEM_MARSHAL_DOT_H
