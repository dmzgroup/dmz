#include <dmzSystem.h>
#include <dmzSystemMarshal.h>
#include <dmzTypesMatrix.h>
#include <dmzTypesString.h>
#include <dmzTypesUUID.h>
#include <dmzTypesVector.h>
#include <string.h> // for memcpy

/*!

\class dmz::Marshal
\ingroup System
\brief Marshals data in a given byte order.
\details This class is used to marshal data in to a desired byte order, either big or
little endian. The marshalled data created by this class can be used for writing to
binary files or transmitting data over the network.
\sa dmz::Unmarshal.

*/

struct dmz::Marshal::State {

   enum WriteEnum { RawWrite, NormalWrite };

   ByteOrderEnum order;

   Int32 size;
   Int32 length;
   Int32 place;
   Boolean swap;

   char *buffer;

   void set_byte_order (const ByteOrderEnum TheOrder) {

      order = TheOrder;
      if (order != dmz::get_byte_order ()) { swap = True; }
   }

   State (const ByteOrderEnum TheOrder) :
         order (TheOrder),
         size (0),
         length (0),
         place (0),
         buffer(0),
         swap (False) {

      set_byte_order (TheOrder);
   }

   ~State () { if (buffer) { delete []buffer; buffer = 0; } }

   void reset () {

      length = 0;
      place = 0;
   }

   void empty () {

      reset ();
      if (buffer) { delete []buffer; buffer = 0; }
      size = 0;
   }

   void grow (const Int32 GrowSize) {

      if (GrowSize > size) {

         char *growBuffer = new char[GrowSize];

         if (growBuffer) {

            if (buffer) {

               memcpy (growBuffer, buffer, size);
               delete []buffer; buffer = 0;
            }

            buffer = growBuffer;
            size = GrowSize;
         }
      }
   }

   State &operator= (const State &Value) {

      reset ();
      set_byte_order (Value.order);

      if (Value.buffer) {

         grow (Value.size);

         if (buffer && (Value.length <= size)) {

            memcpy (buffer, Value.buffer, Value.length);
            length = Value.length;
            place = Value.place;
         }
      }

      return *this;
   }

   Int32 grow_size (const Int32 ExtendSize) {

         const Int32 GrowSize (size + ((size + 1) >> 1));
         return GrowSize > ExtendSize ? GrowSize : ExtendSize;
   }

   void write (
         const Int32 ValueSize,
         char *value,
         const WriteEnum WriteMode = NormalWrite) {

      if ((place + ValueSize) > size) { grow (grow_size (place + ValueSize)); }

      if (buffer && ((place + ValueSize) <= size)) {

         if (!swap || (WriteMode == RawWrite)) {

            for (Int32 ix = 0; ix < ValueSize; ix++) {

               buffer[place] = value[ix];
               place++;
            }
         }
         else {

            for (Int32 ix = (ValueSize - 1); ix >= 0; ix--) {

               buffer[place] = value[ix];
               place++;
            }
         }

         if (place > length) { length = place; }
      }
   }
};


/*!

\brief Constructor.
\param[in] Order dmz::ByteOrderEnum specifies byte order in which data is marshaled.

*/
dmz::Marshal::Marshal (const ByteOrderEnum Order) : _state (*( new State (Order))) {;}


//! Copy constructor.
dmz::Marshal::Marshal (const Marshal &Value) :
      _state (*( new State (Value._state.order))) { _state = Value._state; }


//! Destructor.
dmz::Marshal::~Marshal () { delete &_state; }


//! Assignment operator.
dmz::Marshal &
dmz::Marshal::operator= (const Marshal &Value) {

   _state = Value._state;
   return *this;
}


/*!

\brief Sets place and length to zero.
\details Resets Marshal so that place and length are both zero. Storage is not deleted.

*/
void
dmz::Marshal::reset () { _state.reset (); }


//! \brief Deletes internal storage.
void
dmz::Marshal::empty () { _state.empty (); }


/*!

\brief Returns byte order being used to marshal data.
\return Returns dmz::ByteOrderEnum that indicates the byte order being used to marshal
data.

*/
dmz::ByteOrderEnum
dmz::Marshal::get_byte_order () const { return _state.order; }


/*!

\brief Grows internal storage.
\details If \a Size is smaller than the current internal storage size, no action is taken.
\param[in] Size Desired internal storage size in bytes.
\return Returns dmz::True if internal storage grows to desired size.

*/
dmz::Boolean
dmz::Marshal::grow (const Int32 Size) {

   _state.grow (Size);

   return _state.size >= Size;
}


//! \brief Returns size of internal storage in bytes.
dmz::Int32
dmz::Marshal::get_size () const { return _state.size; }


/*!

\brief Sets current write place in Marshal.
\param[in] Place Desired write place in marshal.
\return Returns dmz::True if new place is successfully set.

*/
dmz::Boolean
dmz::Marshal::set_place (const Int32 Place) {

   Boolean result (False);

   if (Place <= _state.length) { _state.place = Place; result = True; }

   return result;
}


//! Returns current write place in internal storage.
dmz::Int32
dmz::Marshal::get_place () const { return _state.place; }


/*!

\brief Sets current length of written data.
\details If the desired new written length is larger than the current written length,
the new bytes of in the storage is set to zero.
\param[in] Length Desired written length.
\return Returns dmz::True if \a Length is successfully set.

*/
dmz::Boolean
dmz::Marshal::set_length (const Int32 Length) {

   Boolean result (False);

   if (Length > _state.length) {

      const Int32 StartPlace (_state.place);

      _state.place = _state.length;

      const Int32 Diff (Length - _state.length);

      for (Int32 ix = 0; ix < Diff; ix++) { set_next_uint8 (0); }

      _state.place = StartPlace;
   }

   if (Length <= _state.length) { result = True; }

   return result;
}


//! Returns number of written to the storage.
dmz::Int32
dmz::Marshal::get_length () const { return _state.length; }


//! Returns buffer containing marshalled data.
char *
dmz::Marshal::get_buffer () const { return _state.buffer; }


/*!

\brief Returns buffer containing marshalled data.
\param[out] length Contains number of bytes written in returned buffer.
\return Character buffer containing marshalled data. Returns NULL if empty.

*/
char *
dmz::Marshal::get_buffer (Int32 &length) const {

   length = _state.length;
   return _state.buffer;
}


//! Marshals a dmz::Int8 at the current place in the storage buffer.
void
dmz::Marshal::set_next_int8 (const Int8 Value) {

   _state.write (sizeof (Value), (char *)&Value);
}


//! Marshals a dmz::Int16 at the current place in the storage buffer.
void
dmz::Marshal::set_next_int16 (const Int16 Value) {

   _state.write (sizeof (Value), (char *)&Value);
}


//! Marshals a dmz::Int32 at the current place in the storage buffer.
void
dmz::Marshal::set_next_int32 (const Int32 Value) {

   _state.write (sizeof (Value), (char *)&Value);
}


//! Marshals a dmz::Int64 at the current place in the storage buffer.
void
dmz::Marshal::set_next_int64 (const Int64 Value) {

   _state.write (sizeof (Value), (char *)&Value);
}


//! Marshals a dmz::UInt8 at the current place in the storage buffer.
void
dmz::Marshal::set_next_uint8 (const UInt8 Value) {

   _state.write (sizeof (Value), (char *)&Value);
}


//! Marshals a dmz::UInt16 at the current place in the storage buffer.
void
dmz::Marshal::set_next_uint16 (const UInt16 Value) {

   _state.write (sizeof (Value), (char *)&Value);
}


//! Marshals a dmz::UInt32 at the current place in the storage buffer.
void
dmz::Marshal::set_next_uint32 (const UInt32 Value) {

   _state.write (sizeof (Value), (char *)&Value);
}


//! Marshals a dmz::UInt64 at the current place in the storage buffer.
void
dmz::Marshal::set_next_uint64 (const UInt64 Value) {

   _state.write (sizeof (Value), (char *)&Value);
}


//! Marshals a dmz::Float32 at the current place in the storage buffer.
void
dmz::Marshal::set_next_float32 (const Float32 Value) {

   _state.write (sizeof (Value), (char *)&Value);
}


//! Marshals a dmz::Float64 at the current place in the storage buffer.
void
dmz::Marshal::set_next_float64 (const Float64 Value) {

   _state.write (sizeof (Value), (char *)&Value);
}


/*!

\brief Marshals a dmz::String at the current place in the storage buffer.
\details Writes the entire string including the NULL terminator.
\param[in] Value String to be marshalled.

*/
void
dmz::Marshal::set_next_string (const String &Value) {

   // + 1 so we get the \0 as well as the string
   set_next_fixed_string (Value, Value.get_length () + 1);
}


/*!

\brief Marshals a fixed sized dmz::String at the current place in the storage buffer.
\param[in] Value String to be marshalled.
\param[in] Size Number of bytes of the string to be written.
\note This function does \b not use the C Strings NULL terminator. It will always write
the number of byte specified by \a Size. If \a Value does not contain enough data
to write out the number of bytes requested then the NULL character is used to pad out the
remaining bytes.

*/
void
dmz::Marshal::set_next_fixed_string (const String &Value, const Int32 Size) {

   const char *buffer = Value.get_buffer ();

   if (buffer) {

      if ((_state.place + Size) > _state.size) {

         _state.grow (_state.grow_size (_state.place + Size));
      }

      if (_state.buffer && ((_state.place + Size) < _state.size)) {

         memcpy (&(_state.buffer[_state.place]), buffer, Size);

         _state.place += Size;
         if (_state.place > _state.length) { _state.length = _state.place; }
      }
   }
   else { for (Int32 ix = 0; ix < Size; ix++) { set_next_uint8 (0); } }
}


//! Marshals a dmz::UUID at the current place in the storage buffer.
void
dmz::Marshal::set_next_uuid (const UUID &Value) {

   UInt8 array[16] = {
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
   };

   Value.to_array (array);
   _state.write (16, (char *)array, State::RawWrite);
}


/*!

\brief Marshals a dmz::Vector at the current place in the storage buffer.
\details This function takes the three 64 bit values and marshals them as three 32 bit
values.
\param[in] Value Vector to be marshalled.

*/
void
dmz::Marshal::set_next_vector32 (const Vector &Value) {

   set_next_float32 ((Float32)Value.get_x ());
   set_next_float32 ((Float32)Value.get_y ());
   set_next_float32 ((Float32)Value.get_z ());
}


/*!

\brief Marshals a dmz::Matrix at the current place in the storage buffer.
\details This function takes the nine 64 bit values and marshals them as nine 32 bit
values.
\param[in] Value Matrix to be marshalled.

*/
void
dmz::Marshal::set_next_matrix32 (const Matrix &Value) {

   Float32 data[9];
   Value.to_array32 (data);
   for (Int32 ix = 0; ix < 9; ix++) { set_next_float32 (data[ix]); }
}


//! Marshals a dmz::Vector at the current place in the storage buffer.
void
dmz::Marshal::set_next_vector (const Vector &Value) {

   set_next_float64 (Value.get_x ());
   set_next_float64 (Value.get_y ());
   set_next_float64 (Value.get_z ());
}


//! Marshals a dmz::Matrix at the current place in the storage buffer.
void
dmz::Marshal::set_next_matrix (const Matrix &Value) {

   Float64 data[9];
   Value.to_array (data);
   for (Int32 ix = 0; ix < 9; ix++) { set_next_float64 (data[ix]); }
}

