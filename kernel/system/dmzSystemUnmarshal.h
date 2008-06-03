#ifndef DMZ_SYSTEM_UNMARSHAL_DOT_H
#define DMZ_SYSTEM_UNMARSHAL_DOT_H

#include <dmzSystem.h>
#include <dmzKernelExport.h>
#include <dmzTypesBase.h>

namespace dmz {

   class Matrix;
   class Quaternion;
   class String;
   class UUID;
   class Vector;

   class DMZ_KERNEL_LINK_SYMBOL Unmarshal {

      public:
         Unmarshal (const ByteOrderEnum Order);
         Unmarshal (const Unmarshal &Value);
         ~Unmarshal ();

         Unmarshal &operator= (const Unmarshal &Value);

         void reset ();
         void clear ();

         ByteOrderEnum get_byte_order () const;
         Int32 get_length () const;

         Boolean set_place (const Int32 Place);
         Int32 get_place () const;

         void set_buffer (const Int32 Size, char *buffer);
         char *get_buffer () const;
         char *get_buffer (Int32 &length) const;

         Int8 get_next_int8 ();
         Int16 get_next_int16 ();
         Int32 get_next_int32 ();
         Int64 get_next_int64 ();

         UInt8 get_next_uint8 ();
         UInt16 get_next_uint16 ();
         UInt32 get_next_uint32 ();
         UInt64 get_next_uint64 ();

         Float32 get_next_float32 ();
         Float64 get_next_float64 ();

         void get_next_string (String &value);
         void get_next_fixed_string (const Int32 Size, String &value);

         void get_next_uuid (UUID &value);

         // Unmarshals data as 32 bit floats
         void get_next_vector32 (Vector &value);
         void get_next_matrix32 (Matrix &value);
         void get_next_quaternion32 (Quaternion &value);

         // Unmarshals data as native 64 bit floats
         void get_next_vector (Vector &value);
         void get_next_matrix (Matrix &value);
         void get_next_quaternion (Quaternion &value);

      protected:
         struct State;
         State &_state; //!< Internal state.

      private:
         Unmarshal ();
   };

   //! Use this interface in templates only.
   class UnmarshalWrap {

      protected:
         Unmarshal &_data; //!< Unmarshal object being wrapped.

      public:
         UnmarshalWrap (Unmarshal &data) : _data (data) {;} //!< Constructor
         ~UnmarshalWrap () {;} //!< Destructor.

         //! Wraps dmz::Unmarshal::get_next_int8.
         void get_next (Int8 &value) { value = _data.get_next_int8 (); }
         //! Wraps dmz::Unmarshal::get_next_int16.
         void get_next (Int16 &value) { value = _data.get_next_int16 (); }
         //! Wraps dmz::Unmarshal::get_next_int32.
         void get_next (Int32 &value) { value = _data.get_next_int32 (); }
         //! Wraps dmz::Unmarshal::get_next_int64.
         void get_next (Int64 &value) { value = _data.get_next_int64 (); }
         //! Wraps dmz::Unmarshal::get_next_uint8.
         void get_next (UInt8 &value) { value = _data.get_next_uint8 (); }
         //! Wraps dmz::Unmarshal::get_next_uint16.
         void get_next (UInt16 &value) { value = _data.get_next_uint16 (); }
         //! Wraps dmz::Unmarshal::get_next_uint32.
         void get_next (UInt32 &value) { value = _data.get_next_uint32 (); }
         //! Wraps dmz::Unmarshal::get_next_uint64.
         void get_next (UInt64 &value) { value = _data.get_next_uint64 (); }
         //! Wraps dmz::Unmarshal::get_next_float32.
         void get_next (Float32 &value) { value = _data.get_next_float32 (); }
         //! Wraps dmz::Unmarshal::get_next_float64.
         void get_next (Float64 &value) { value = _data.get_next_float64 (); }
         //! Wraps dmz::Unmarshal::get_next_string.
         void get_next (String &value) { _data.get_next_string (value); }
         //! Wraps dmz::Unmarshal::get_next_uuid.
         void get_next (UUID &value) { _data.get_next_uuid (value); }
         //! Wraps dmz::Unmarshal::get_next_vector.
         void get_next (Vector &value) { _data.get_next_vector (value); }
         //! Wraps dmz::Unmarshal::get_next_matrix.
         void get_next (Matrix &value) { _data.get_next_matrix (value); }
         //! Wraps dmz::Unmarshal::get_next_quaternion.
         void get_next (Quaternion &value) { _data.get_next_quaternion (value); }

      private:
         UnmarshalWrap ();
         UnmarshalWrap (const UnmarshalWrap &);
         UnmarshalWrap &operator= (const UnmarshalWrap &);
   };
};

#endif // DMZ_SYSTEM_UNMARSHAL_DOT_H
