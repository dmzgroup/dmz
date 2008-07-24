#include <dmzSystem.h>
#include <dmzSystemUnmarshal.h>
#include <dmzTypesBase.h>
#include <dmzTypesMatrix.h>
#include <dmzTypesQuaternion.h>
#include <dmzTypesString.h>
#include <dmzTypesUUID.h>
#include <dmzTypesVector.h>

/*!

\class dmz::Unmarshal
\ingroup System
\brief Unmarshals data from a buffer in a given byte order.
\details This class is used to unmarshal data in to a desired byte order, either big or
little endian.
\sa dmz::Marshal.

*/

struct dmz::Unmarshal::State {

   enum ReadEnum { RawRead, NormalRead };

   ByteOrderEnum order;
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
         length (0),
         place (0),
         swap (False),
         buffer (0) { set_byte_order (TheOrder); }

   void reset () { place = 0; }

   void clear () {

      reset ();
      length = 0;
      buffer = 0;
   }

   State &operator= (const State &Value) {

      order = Value.order;
      length = Value.length;
      place = Value.place;
      swap = Value.swap;
      buffer= Value.buffer;

      return *this;
   }

   void read (const Int32 ValueSize, char *value, const ReadEnum ReadMode = NormalRead) {

      if (buffer && (place + ValueSize) <= length) {

         if (!swap || (ReadMode == RawRead)) {

            for (Int32 ix = 0; ix < ValueSize; ix++) {

               value[ix] = buffer[place];
               place++;
            }
         }
         else {

            for (Int32 ix = (ValueSize - 1); ix >= 0; ix--) {

               value[ix] = buffer[place];
               place++;
            }
         }
      }
   }
};



/*!

\brief Constructor.
\param[in] Order dmz::ByteOrderEnum specifies byte order in which data is unmarshaled.

*/
dmz::Unmarshal::Unmarshal (const ByteOrderEnum Order) : _state (*(new State (Order))) {;}


//! Copy constructor.
dmz::Unmarshal::Unmarshal (const Unmarshal &Value) :
      _state (*(new State (Value._state.order))) {

   _state = Value._state;
}


/*!

\brief Destructor.
\note If Unmarshal contains a buffer at the time of destruction, that buffer is \b not
deleted.

*/
dmz::Unmarshal::~Unmarshal () { delete &_state; }


//! Assignment operator.
dmz::Unmarshal &
dmz::Unmarshal::operator= (const Unmarshal &Value) {

   _state = Value._state;
   return *this;
}


//! Sets place to zero.
void
dmz::Unmarshal::reset () { _state.reset (); }


//! Sets place and length to zero and sets the internal buffer to NULL.
void
dmz::Unmarshal::clear () { _state.clear (); }


/*!

\brief Returns byte order being used to unmarshal data.
\return Returns dmz::ByteOrderEnum that indicates the byte order being used to unmarshal
data.

*/
dmz::ByteOrderEnum
dmz::Unmarshal::get_byte_order () const { return _state.order; }


//! Returns length of buffer being unmarshalled.
dmz::Int32
dmz::Unmarshal::get_length () const { return _state.length; }


/*!

\brief Sets place for next read.
\param[in] Place Location of next read in buffer.
\return Returns dmz::True if new read place falls with in the length of the buffer.

*/
dmz::Boolean
dmz::Unmarshal::set_place (const Int32 Place) {

   Boolean result (False);

   if (Place <= _state.length) { _state.place = Place; result = True; }

   return result;
}


//! Returns current read place.
dmz::Int32
dmz::Unmarshal::get_place () const { return _state.place; }


/*!

\brief sets current buffer.
\param[in] Size Number bytes contained in \a buffer.
\param[in] buffer Array containing data to be unmarshalled.

*/
void
dmz::Unmarshal::set_buffer (const Int32 Size, char *buffer) {

   reset ();
   _state.length = Size;
   _state.buffer = buffer;
}


//! Returns buffer being unmarshalled.
char *
dmz::Unmarshal::get_buffer () const { return _state.buffer; }


/*!

\brief Returns buffer being unmarshalled as well as its length.
\param[out] length Size of the buffer.
\return Buffer being unmarshalled.

*/
char *
dmz::Unmarshal::get_buffer (Int32 &length) const {

   length = _state.length;
   return _state.buffer;
}


//! Unmarshals next dmz::Int8 in the buffer.
dmz::Int8
dmz::Unmarshal::get_next_int8 () {

   Int8 value (0);
   _state.read (sizeof (value), (char *)&value);
   return value;
}


//! Unmarshals next dmz::Int16 in the buffer.
dmz::Int16
dmz::Unmarshal::get_next_int16 () {

   Int16 value (0);
   _state.read (sizeof (value), (char *)&value);
   return value;
}


//! Unmarshals next dmz::Int32 in the buffer.
dmz::Int32
dmz::Unmarshal::get_next_int32 () {

   Int32 value (0);
   _state.read (sizeof (value), (char *)&value);
   return value;
}


//! Unmarshals next dmz::Int64 in the buffer.
dmz::Int64
dmz::Unmarshal::get_next_int64 () {

   Int64 value (0);
   _state.read (sizeof (value), (char *)&value);
   return value;
}


//! Unmarshals next dmz::UInt8 in the buffer.
dmz::UInt8
dmz::Unmarshal::get_next_uint8 () {

   UInt8 value (0);
   _state.read (sizeof (value), (char *)&value);
   return value;
}


//! Unmarshals next dmz::UInt16 in the buffer.
dmz::UInt16
dmz::Unmarshal::get_next_uint16 () {

   UInt16 value (0);
   _state.read (sizeof (value), (char *)&value);
   return value;
}


//! Unmarshals next dmz::UInt32 in the buffer.
dmz::UInt32
dmz::Unmarshal::get_next_uint32 () {

   UInt32 value (0);
   _state.read (sizeof (value), (char *)&value);
   return value;
}


//! Unmarshals next dmz::UInt64 in the buffer.
dmz::UInt64
dmz::Unmarshal::get_next_uint64 () {

   UInt64 value (0);
   _state.read (sizeof (value), (char *)&value);
   return value;
}


//! Unmarshals next dmz::Float32 in the buffer.
dmz::Float32
dmz::Unmarshal::get_next_float32 () {

   Float32 value (0);
   _state.read (sizeof (value), (char *)&value);
   return value;
}


//! Unmarshals next dmz::Float64 in the buffer.
dmz::Float64
dmz::Unmarshal::get_next_float64 () {

   Float64 value (0);
   _state.read (sizeof (value), (char *)&value);
   return value;
}


/*!

\brief Unmarshals a dmz::String from the buffer.
\details Read bytes from the buffer until an 8 bit NULL is encountered or the end of
the buffer which ever comes first.
\param[out] value String containing the string unmarshalled from the buffer.

*/
void
dmz::Unmarshal::get_next_string (String &value) {

   if (_state.buffer && (_state.place < _state.length)) {

      Int32 current (_state.place);

      while (_state.buffer[current] && (current < _state.length)) {

         current++;
      }

      value.set_buffer (&(_state.buffer[_state.place]), current - _state.place);

      _state.place = current + 1;
   }
}


/*!

\brief Unmarshals a fixed dmz::String from the buffer.
\details Read \a Size number of bytes from the buffer or until  the end of the buffer
is encountered.
\param[in] Size Number of bytes to read from the buffer.
\param[out] value String containing the string unmarshalled from the buffer.

*/
void
dmz::Unmarshal::get_next_fixed_string (const Int32 Size, String &value) {

   if (_state.buffer) {

      Int32 realSize (Size);

      if ((_state.place + Size) >= _state.length) {

         realSize = _state.length - _state.place;
      }

      if (realSize > 0) {

         value.set_buffer (&(_state.buffer[_state.place]), realSize);

         _state.place += realSize;
      }
   }
}


//! Unmarshals next dmz::UUID in the buffer.
void
dmz::Unmarshal::get_next_uuid (UUID &value) {

   UInt8 array[16] = {
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
   };

   _state.read (16, (char *)array, State::RawRead);
   value.from_array (array);
}


/*!

\brief Unmarshals a dmz::Vector from the current place in the storage buffer.
\details This function unmarshals the three 32 bit values and store them in a dmz::Vector.
\param[out] value Vector to store unmarshalled data.

*/
void
dmz::Unmarshal::get_next_vector32 (Vector &value) {

   value.set_x (get_next_float32 ());
   value.set_y (get_next_float32 ());
   value.set_z (get_next_float32 ());
}


/*!

\brief Unmarshals a dmz::Matrix from the current place in the storage buffer.
\details This function unmarshals the nine 32 bit values and store them in a dmz::Matrix.
\param[out] value Matrix to store unmarshalled data.

*/
void
dmz::Unmarshal::get_next_matrix32 (Matrix &value) {

   Float32 data[9];
   for (Int32 ix = 0; ix < 9; ix++) { data[ix] = get_next_float32 (); }
   value.from_array32 (data);
}


/*!

\brief Unmarshals a dmz::Quaternion from the current place in the storage buffer.
\details This function unmarshals the four 32 bit values and store them in a
dmz::Quaternion.
\param[out] value Quaternion to store unmarshalled data.

*/
void
dmz::Unmarshal::get_next_quaternion32 (Quaternion &value) {

   value.set_x (get_next_float32 ());
   value.set_y (get_next_float32 ());
   value.set_z (get_next_float32 ());
   value.set_w (get_next_float32 ());
}


//! Unmarshals next dmz::Vector in the buffer.
void
dmz::Unmarshal::get_next_vector (Vector &value) {

   value.set_x (get_next_float64 ());
   value.set_y (get_next_float64 ());
   value.set_z (get_next_float64 ());
}


//! Unmarshals next dmz::Matrix in the buffer.
void
dmz::Unmarshal::get_next_matrix (Matrix &value) {

   Float64 data[9];
   for (Int32 ix = 0; ix < 9; ix++) { data[ix] = get_next_float64 (); }
   value.from_array (data);
}


//! Unmarshals next dmz::Quaternion in the buffer.
void
dmz::Unmarshal::get_next_quaternion (Quaternion &value) {

   value.set_x (get_next_float64 ());
   value.set_y (get_next_float64 ());
   value.set_z (get_next_float64 ());
   value.set_w (get_next_float64 ());
}

