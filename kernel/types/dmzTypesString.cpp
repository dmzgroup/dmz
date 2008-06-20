#include <dmzTypesBase.h>
#include <dmzTypesMask.h>
#include <dmzTypesString.h>
#include <dmzTypesVector.h>
#include <dmzTypesMatrix.h>
#include <dmzTypesQuaternion.h>
#include <dmzTypesUUID.h>

#include <ctype.h> // toupper tolower
#include <stdio.h> // snprintf
#include <string.h> // memcpy strncmp

/*!

\file dmzTypesString.h
\brief dmz::String interface.
\details File contains dmz::String class interface as well as basic dmz::String stream
operator function prototypes.

*/

/*!

\class dmz::String
\ingroup Types
\brief Character array container.
\details The dmz::String contains an 8 bit character array of arbitrary length.
A dmz::String has both Size
and Length. The Size is the size of the allocated character buffer and the
Length is the amount of the buffer currently being used to store a continuous
array of data. The NULL character is \b not a control character
as it is with traditional Standard C
character arrays. Since the dmz::String need not be NULL terminated,
it is possible to use it as a binary data container. Be aware that if
binary data is stored in a dmz::String, the length must also be specified. If
binary data is stored with one of the member functions that does not take a
length or size parameter the class will look for the first NULL character
in the buffer to determine size and length.

*/

/*!

\brief Base constructor.
\details No initial storage buffer is allocated.

*/
dmz::String::String () : _buffer (NULL), _length (0), _size (0) {;}


//! Copy constructor.
dmz::String::String (const String &Str) : _buffer (NULL), _length (0), _size (0) {

   *this = Str;
}


/*!

\brief Creates storage and copies buffer.
\details Uses String::set_buffer.
\param[in] Buffer character buffer to copy.
\param[in] Length number of characters to copy from \a Buffer.
\param[in] Size of internal buffer to allocate.

*/
dmz::String::String (const char *Buffer, const Int32 Length, const Int32 Size) :
      _buffer (NULL),
      _length (0),
      _size (0) {

   set_buffer (Buffer, Length, Size);
}


/*!

\brief Creates storage and copies buffer.
\details Calls String::set_buffer (\a Buffer, \a Length, \a Length)
\param[in] Buffer character buffer to copy.
\param[in] Length number of characters to copy from \a Buffer.

*/
dmz::String::String (const char *Buffer, const Int32 Length) :
      _buffer (NULL),
      _length (0),
      _size (0) {

   set_buffer (Buffer, Length, Length);
}


/*!

\brief Creates storage and copies buffer.
\details Calls String::set_buffer. Uses the Standard C function strlen to find \a Length
and \a Size. \note Make \b sure \a Buffer is NULL terminated.
\param[in] Buffer character buffer to copy.

*/
dmz::String::String (const char *Buffer) :
      _buffer (NULL),
      _length (0),
      _size (0) {

   const Int32 Length = Buffer ? strlen (Buffer) : 0;
   set_buffer (Buffer, Length, Length);
}


/*!

\brief Destructor
\details Deletes allocated char buffer if it exists.

*/
dmz::String::~String () { empty (); }


/*!

\brief Relational "equal to" operator.
\param[in] Buffer Right hand value.
\return Returns true if String's buffers are same length and contain identical data.

*/
dmz::Boolean
dmz::String::operator== (const String &Buffer) const {

   Boolean result (False);

   if (_length == Buffer._length) {

      if (!_buffer && !Buffer._buffer) { result = True; }
      else if (!strncmp (_buffer, Buffer._buffer, _length)) { result = True; }
   }

   return result;
}


/*!

\brief Relational "not equal to" operator.
\param[in] Buffer Right hand value.
\return Returns true if String's data is not identical.

*/
dmz::Boolean
dmz::String::operator!= (const String &Buffer) const {

   return !(*this == Buffer);
}


/*!

\brief Assignment operator.
\details Copies the contents of \a Buffer.
\param[in] Buffer Right hand value.
\return Returns reference to self.

*/
dmz::String &
dmz::String::operator= (const String &Buffer) {

   if (_buffer && !Buffer._buffer) { empty (); }
   else if (Buffer._buffer) {

      if ((!_buffer) || (_size <= Buffer._length)) {

         empty ();

         const Int32 NewSize (
            Buffer._size >= Buffer._length + 1 ? Buffer._size : Buffer._length + 1);

         _buffer = new char[NewSize];
         if (_buffer) { _size = NewSize; }
      }

      if (_buffer) {

         _length = Buffer._length;
         memcpy (_buffer, Buffer._buffer, _length);
         _buffer[_length] = '\0';
      }
   }

   return *this;
}


/*!

\brief Addition operator.
\param[in] Buffer Right hand value.
\return Returns a dmz::String with the contents of the two strings concatenated
together.

*/
dmz::String
dmz::String::operator+ (const String &Buffer) const {

   String result (*this);
   result.append (Buffer);
   return result;
}


/*!

\brief Assignment by sum operator
\details Appends content of \a Buffer.
\param[in] Buffer Right hand value.
\return Returns reference to self.

*/
dmz::String &
dmz::String::operator+= (const String &Buffer) {

   return append (Buffer);
}


/*!

\brief Logical not operator.
\return Returns dmz::True if the buffer is NULL or the buffer is an empty string.
(i.e. "\0")

*/
dmz::Boolean
dmz::String::operator! () const {

   Boolean result (False);
   if (!_buffer || _buffer[0] == '\0') { result = True; }
   return result;
}


/*!

\brief Changes length of character buffer.
\details This function will \b not make the buffer larger. It can only be used to
make the buffer length smaller. When the buffer is contracted, the part of the
buffer that falls out side of the new length is stored in \a remainder if
a valid pointer to a dmz::String is passed in. If \a remainder is NULL, it
is ignored.  @see dmz::String::resize
\param[in] Length specifies new length of buffer.
\param[out] remainder if valid, has the remainder of the buffer stored in it

*/
void
dmz::String::set_length (const Int32 Length, String *remainder) {

   if (Length < _size) {

      if (_buffer) {

         if (remainder) {

            if ((Length < _length)) {

               remainder->set_buffer (&(_buffer[Length]), _length - Length);
            }
            else { remainder->flush (); }
         }

         _buffer[Length] = '\0';
         _length = Length;
      }
   }
}


//! \brief Returns the length of the buffer.
dmz::Int32
dmz::String::get_length () const { return _length; }


//! \brief Returns the size of the buffer.
dmz::Int32
dmz::String::get_size () const { return _size; }


/*!

\brief Creates buffer of \a Size and copies \a Length bytes from \a Buffer
\details Function will only copy \a Buffer if \a Size \>= \a Length. If Size == Length
then the internal size of the buffer will be Size + 1 so that a NULL byte may be set
at the end. A NULL character is appended to the end of the buffer to facilitate
the use of the buffer in Standard C string functions and to prevent buffer
overruns. If the string already has a buffer allocated it will either delete
the existing buffer and allocate a new one if the existing one is too small or
use the existing buffer if it is the same size or larger than the buffer being copied.
If the preexisting buffer contains data, it will be overwritten with the data
contained in \a Buffer.
\param[in] Buffer character buffer to copy.
\param[in] Length number of characters to copy from \a Buffer.
\param[in] Size of internal buffer to allocate.

*/
void
dmz::String::set_buffer (const char *Buffer, const Int32 Length, const Int32 Size) {

   if ((Length >= 0) && (Length <= Size) && Buffer) {

      const Int32 RealSize ((Length == Size) ? Size + 1 :  Size);

      if (!_buffer || (RealSize > _size)) {

         empty ();
         _buffer = new char[RealSize];
         _size = RealSize;
      }

      if (_buffer) {

         flush ();
         _length = Length;
         memcpy (_buffer, Buffer, _length);
         _buffer[_length] = '\0';
      }
   }
   else if (!Buffer && (Size > 0)) {

      if (Size != _size) {

         empty ();
         _buffer = new char[Size];
         _size = Size;
      }

      flush ();
   }
}

/*!

\fn  dmz::String::set_buffer (const char *Buffer, const Int32 Length)
\brief Inline function that call set_buffer (Buffer, Length, Length);
\sa dmz::String::set_buffer (const char *Buffer, const Int32 Length, const Int32 Size)

*/

/*!

\brief Returns pointer to internal buffer and stores the buffer's Length in \a length.
\param[out] length used to return the length of the buffer.

*/
const char *
dmz::String::get_buffer (Int32 &length) const {

    length = _length;
    return _buffer;
}


//! \brief Returns pointer to internal buffer.
const char *
dmz::String::get_buffer () const { return _buffer; }


/*!

\brief Sets buffer to \a Value at \a Index.
\details \a Index may be a positive or negative integer. The \a Index starts at zero
and goes to Length - 1. If \a Index is negative then -1 points to the end of
the buffer and -2 points to the second to last character in the buffer, etc.
If the \a Index is either to large or too small, no value is set and the return
value will be dmz::False.
\param[in] Index Index into the buffer may be positive or negative.
\param[in] Value Value to be set at the \a Index into the buffer.
\return dmz::True if \a Value is set, dmz::False if unable to set \a Value.

*/
dmz::Boolean
dmz::String::set_char (const Int32 Index, const char Value) {

   Boolean result (False);
   const Int32 RealIndex ((Index >= 0) ? Index : _length + Index);

   if ((RealIndex >= 0) && (RealIndex < _length)) {

      _buffer[RealIndex] = Value;
      result = True;
   }

   return result;
}


/*!

\brief Return the character value at \a Index
\details \a Index may be a positive or negative integer. The \a Index starts at zero
and goes to Length - 1. If \a Index is negative then -1 points to the end of
the buffer and -2 points to the second to last character in the buffer, etc.
If the \a Index is either to large or too small, The NULL character is returned.
\param[in] Index Index into the buffer may be positive or negative.
\return Character value in the buffer found at \a Index. If the \a Index is out of
range, the NULL character is returned.

*/
char
dmz::String::get_char (const Int32 Index) const {

   char result (0);
   const Int32 RealIndex ((Index >= 0) ? Index : _length + Index);

   if ((RealIndex >= 0) && (RealIndex < _length)) {

      result = _buffer[RealIndex];
   }

   return result;
}


//! \brief Deletes internal buffer.
dmz::String &
dmz::String::empty () {

   if (_buffer) { delete []_buffer; _buffer = NULL; _size = _length = 0; }
   return *this;
}


/*!

\brief Sets length to zero and clears the buffer.
\details Set all values in the buffer array to NULL. The function does \b not delete
the buffer.

*/
dmz::String &
dmz::String::flush () {

   _length = 0;

   if (_buffer) { memset (_buffer, '\0', _size); }

   return *this;
}


/*!

\brief Resized character buffer.
\details Safely resize internal character buffer. If the buffer contains data before
the resize, that data will be preserved after the resize if the new size is
Larger than the current buffer size. If the new size is smaller than the current
size then data that does not fit in the new buffer will be lost.
\param[in] Size Desired size of character buffer.
\return Returns reference to self.

*/
dmz::String &
dmz::String::resize (const Int32 Size) {

   if (Size == _size) {;} // do nothing
   else {

      char *buf (new char[Size + 1]);

      if (buf && _buffer) {

         _size = Size + 1;
         _length  = (Size > _length) ? _length : Size;
         memcpy (buf, _buffer, _length);
         buf[_length] = '\0';
         char *oldBuf (_buffer);
         _buffer = buf;
         delete []oldBuf;
      }
      else if (buf) { // must be !_buffer

         _buffer = buf;
         _size = Size + 1;
         _length = 0;
         flush ();
      }
   }

   return *this;
}


/*!

\brief Appends \a Value to end of current character buffer.
\details If current buffer is not large enough to append \a Value to the end, it will be
resized.
\param[in] Value buffer to append to end of current buffer.
\return Returns reference to self.

*/
dmz::String &
dmz::String::append (const String &Value) {

   if (Value._buffer) {

      if ((_length + Value._length) >= _size) {

         resize (_length + Value._length + 1);
      }

      if ((_length + Value._length) < _size) {

         memcpy (&(_buffer[_length]), Value._buffer, Value._length);
         _length = _length + Value._length;
         _buffer[_length] = '\0';
      }
   }

   return *this;
}


/*!

\brief Makes multiple copies of \a Buffer
\details Copies \a Buffer the number of times specified by \a Count.
If string has preexisting buffer, it is overwritten.
Calling str.repeat (" ", 5) would create an internal buffer of five spaces.
\param[in] Buffer value to copy
\param[in] Count number of times to copy
\return Returns reference to self.

*/
dmz::String &
dmz::String::repeat (const String &Buffer, const Int32 Count) {

   if (Buffer._length && Buffer._buffer && (Count >= 0)) {

      const Int32 NewSize = Count * Buffer._length + 1;

      if (NewSize > _size) { empty (); resize (NewSize); }

      if (_buffer) {

         for (Int32 ix = 0; ix < Count; ix++) {

            memcpy (&(_buffer[ix * Buffer._length]), Buffer._buffer, Buffer._length);
         }

         _length = NewSize - 1;
         _buffer[NewSize - 1] = '\0';
      }
   }

   return *this;
}


/*!

\brief Shift string by specified number of characters.
\details \a ShiftValue can be either positive or negative. A positive value shifts the
buffer away from zero and a negative value shifts the buffer towards zero. When shifting
the buffer away from zero, the new character values in the buffer are filled with
\a FillChar. Buffers that are shifted towards zero do not use the \a FillChar and
instead the buffer length is contracted with the buffer.
\param[in] ShiftValue Number of characters to shift buffer.
\param[in] FillChar Character to use to fill space created by shifted buffer.
\return Returns reference to self.

*/
dmz::String &
dmz::String::shift (const Int32 ShiftValue, const char FillChar) {

   if (ShiftValue < 0) {

      const Int32 RealShiftValue = -ShiftValue;

      if (RealShiftValue < _length) {

         for (Int32 ix = RealShiftValue; ix <= _length; ix++) {

            { _buffer[ix - RealShiftValue] = _buffer[ix]; }
         }

         _length = _length + ShiftValue;
      }
      else { flush (); }
   }
   else {

      if ((_length + ShiftValue) >= _size) { resize (_length + ShiftValue + 1); }

      for (Int32 ix = _length + ShiftValue + 1; ix >= 0; ix--) {

         if (0 <= (ix - ShiftValue)) { _buffer[ix] = _buffer[ix - ShiftValue]; }
         else { _buffer[ix] = FillChar; }
      }

      _length = _length + ShiftValue;
   }

   return *this;
}


/*!

\brief Tests if buffer is NULL
\return Returns dmz::True if the buffer is NULL

*/
dmz::Boolean
dmz::String::is_null () const {

   Boolean result (False);
   if (!_buffer) { result = True; }
   return result;
}


/*!

\brief Converts all lowercase letters to uppercase.
\return Returns reference to self.

*/
dmz::String &
dmz::String::to_upper () {

   if (_buffer && _length) {

      for (Int32 ix = 0; ix < _length; ix++) { _buffer[ix] = toupper (_buffer[ix]); }
   }

   return *this;
}


/*!

\brief Converts all uppercase letters to lowercase.
\return Returns reference to self.

*/
dmz::String &
dmz::String::to_lower () {

   if (_buffer && _length) {

      for (Int32 ix = 0; ix < _length; ix++) { _buffer[ix] = tolower (_buffer[ix]); }
   }

   return *this;
}


/*!

\brief Creates copy with  all lowercase letters converted to uppercase.
\return Returns copy of buffer with all lowercase letters converted to uppercase.

*/
dmz::String
dmz::String::get_upper () const {

   String result (*this);
   return result.to_upper ();
}


/*!

\brief Creates copy with  all uppercase letters converted to lowercase.
\return Returns copy of buffer with all uppercase letters converted to lowercase.

*/
dmz::String
dmz::String::get_lower () const {

   String result (*this);
   return result.to_lower ();
}


/*!

\brief Finds index of substring in buffer.
\param[in] Sub Substring to be found.
\param[out] index Index where substring starts in buffer.
\param[in] Start Index in buffer where to start substring search.
\return Returns dmz::True if substring is found.

*/
dmz::Boolean
dmz::String::find_sub (
      const String &Sub,
      Int32 &index,
      const Int32 Start) const {

   Boolean result (False);

   index = -1;

   const Int32 RealStart ((Start >= 0) ? Start : _length + Start);

   if ((RealStart >= 0) && ((_length - RealStart) >= Sub._length)) {

      if (_buffer && Sub._buffer) {

         Int32 place = RealStart;
         Boolean done = False;

         while (!done) {

            if ((_length - place) >= Sub._length) {

               if (!strncmp (&(_buffer[place]), Sub._buffer, Sub._length)) {

                  index = place;
                  done = True;
                  result = True;
               }
               else { place++; }
            }
            else { done = True; }
         }
      }
   }

   return result;
}


/*!

\brief Returns substring.
\details \a Start and \a End may be either negative or positive. \a End must point
to an index in the buffer that is greater than the \a Start index location.
Both \a Start and \a End must point to valid indices within the buffer.
A negative value is interpreted as starting at the end of the buffer
and moving towards the front. Thus str.get_sub (5, -1) will return
the substring that starts and index 5 and goes to the end of the buffer.
\param[in] Start Index in buffer where to substring starts.
\param[in] End Index in buffer where to substring Ends.
\return Returns the substring.

*/
dmz::String
dmz::String::get_sub (const Int32 Start, const Int32 End) const {

   String result;

   if (_buffer) {

      const Int32 RealStart ((Start >= 0) ? Start : _length + Start);
      const Int32 RealEnd ((End >= 0) ? End : _length + End);

      if ((RealStart >= 0) && (RealStart <= RealEnd) && (RealEnd < _length)) {

         const Int32 Size (RealEnd - RealStart + 1);
         result.set_buffer (&(_buffer[RealStart]), Size, Size);
      }
   }

   return result;
}


/*!

\brief Replaces substring in buffer.
\details This function works recursively when replacing substrings. Thus the string
"aaaa" processed with the call repalce_sub ("aa", "a") would result in a string
of "a".
\param[in] Sub Substring to be found.
\param[in] Target Replacement substring.
\param[in] Start Index in buffer where to start substring search.
\return Returns number of substrings that were replaced.
\note Since this function works recursively, it is not very fast.

*/
dmz::Int32
dmz::String::replace_sub (
      const String &Sub,
      const String &Target,
      const Int32 Start) {

   struct listStruct {

      listStruct *next;
      Int32 start, end;
      listStruct (const Int32 Start, const Int32 End) :
            next (0),
            start (Start),
            end (End) {;}
      ~listStruct () { if (next) { delete next; next = 0; } }
   };

   listStruct *list (0);
   listStruct *cur (0);
   Int32 count (0);
   Int32 place (Start < 0 ? _length + Start : Start);
   Boolean done (False);
   const Int32 SubLen (Sub.get_length ());
   const Int32 TargetLen (Target.get_length ());

   while (!done) {

      Int32 index (-1);
      if (find_sub (Sub, index, place)) {

          count++;
          place = index + SubLen +  1;
          listStruct *tmp = new listStruct (index, index + SubLen);
          if (!list) { list = cur = tmp; }
          else { cur->next = tmp; cur = tmp; }
      }
      else { done = True; }
   }

   if (list) {

      const Int32 NewLen (_length + (count * (TargetLen - SubLen)));
      char *buf (new char[NewLen + 1]);

      if (buf) {

         Int32 newPlace (0);
         Int32 oldPlace (0);
         cur = list;

         while (cur) {

            const Int32 Diff (cur->start - oldPlace);

            if (Diff > 0)  {

               memcpy (&(buf[newPlace]), &(_buffer[oldPlace]), Diff);
               newPlace += Diff;
               oldPlace += Diff;
            }

            Int32 len (-1);

            memcpy (&(buf[newPlace]), Target.get_buffer (len), TargetLen);
            newPlace += TargetLen;
            oldPlace += SubLen;

            cur = cur->next;
         }

         if (oldPlace < _length) {

            memcpy (&(buf[newPlace]), &(_buffer[oldPlace]), _length - oldPlace);
         }

         empty ();
         _buffer = buf;
         _length = NewLen;
         _size = NewLen + 1;
         _buffer[_length] = '\0';
      }

      delete list; list = 0;
   }

   return count ? count + replace_sub (Sub, Target) : 0;
}


dmz::String &
operator<< (dmz::String &str, const dmz::String &Value) {

   str.append (Value);
   return str;
}


dmz::String &
operator<< (dmz::String &str, const char *value) {

   const dmz::Int32 Len = value ? strlen (value) : 0;
   dmz::String tmp (value, Len);
   str.append (tmp);
   return str;
}


#ifdef _WIN32
#   if _MSC_VER >= 1400
#      define sprintf_n sprintf_s
#   else
#      define sprintf_n _snprintf
#   endif
#else // Unix
#   define sprintf_n snprintf
#endif


dmz::String &
operator<< (dmz::String &str, const unsigned short Value) {

   char buf[sizeof (Value) * 4];
   const dmz::Int32 Len = sprintf_n (buf, sizeof (buf), "%u", Value);
   dmz::String tmp (buf, Len);
   str.append (tmp);
   return str;
}


dmz::String &
operator<< (dmz::String &str, const unsigned int Value) {

   char buf[sizeof (Value) * 4];
   const dmz::Int32 Len = sprintf_n (buf, sizeof (buf), "%u", Value);
   dmz::String tmp (buf, Len);
   str.append (tmp);
   return str;
}


dmz::String &
operator<< (dmz::String &str, const unsigned long Value) {

   char buf[sizeof (Value) * 4];
   const dmz::Int32 Len = sprintf_n (buf, sizeof (buf), "%u", Value);
   dmz::String tmp (buf, Len);
   str.append (tmp);
   return str;
}

#ifdef _WIN32
static const char localUInt64Format[] = "%I64u";
#else
static const char localUInt64Format[] = "%llu";
#endif

dmz::String &
operator<< (dmz::String &str, const dmz::UInt64 Value) {

   char buf[sizeof (Value) * 4];
   const dmz::Int32 Len = sprintf_n (buf, sizeof (buf), localUInt64Format, Value);
   dmz::String tmp (buf, Len);
   str.append (tmp);
   return str;
}


dmz::String &
operator<< (dmz::String &str, const short Value) {

   char buf[sizeof (Value) * 4];
   const dmz::Int32 Len = sprintf_n (buf, sizeof (buf), "%d", Value);
   dmz::String tmp (buf, Len);
   str.append (tmp);
   return str;
}


dmz::String &
operator<< (dmz::String &str, const int Value) {

   char buf[sizeof (Value) * 4];
   const dmz::Int32 Len = sprintf_n (buf, sizeof (buf), "%d", Value);
   dmz::String tmp (buf, Len);
   str.append (tmp);
   return str;
}


dmz::String &
operator<< (dmz::String &str, const long Value) {

   char buf[sizeof (Value) * 4];
   const dmz::Int32 Len = sprintf_n (buf, sizeof (buf), "%d", Value);
   dmz::String tmp (buf, Len);
   str.append (tmp);
   return str;
}


#ifdef _WIN32
static const char localInt64Format[] = "%I64d";
#else
static const char localInt64Format[] = "%lld";
#endif


dmz::String &
operator<< (dmz::String &str, const dmz::Int64 Value) {

   char buf[sizeof (Value) * 4];
   const dmz::Int32 Len = sprintf_n (buf, sizeof (buf), localInt64Format, Value);
   dmz::String tmp (buf, Len);
   str.append (tmp);
   return str;
}


dmz::String &
operator<< (dmz::String &str, const dmz::Float32 Value) {

   char buf[128];
   const dmz::Int32 Len = sprintf_n (buf, sizeof (buf), "%f", Value);
   dmz::String tmp (buf, Len);
   str.append (tmp);
   return str;
}


dmz::String &
operator<< (dmz::String &str, const dmz::Float64 Value) {

   char buf[128];
   const dmz::Int32 Len = sprintf_n (buf, sizeof (buf), "%f", Value);
   dmz::String tmp (buf, Len);
   str.append (tmp);
   return str;
}


dmz::String &
operator<< (dmz::String &str, const void *Value) {

   char buf[sizeof (Value) * 4];
   const dmz::Int32 Len = sprintf_n (buf, sizeof (buf), "%p", Value);
   dmz::String tmp (buf, Len);
   str.append (tmp);
   return str;
}


dmz::String &
operator<< (dmz::String &str, const dmz::Vector &Value) {

   str << "["
      << Value.get_x () << ", "
      << Value.get_y () << ", "
      << Value.get_z () << "]";

   return str;
}


dmz::String &
operator<< (dmz::String &str, const dmz::Mask &Value) {

   const dmz::Int32 Size ((Value.get_size () * 32) - 1);

   str << "[";

   for (dmz::Int32 place = Size; place >= 0; place--) {

      if (place && !((place + 1) % 32) && (str.get_length ()!= 1)) { str << " | "; }
      else if (place && !((place + 1) % 4) && (str.get_length () != 1)) { str << " "; }
      str << (Value.get_bit (place) ? "1" : "0");
   }

   return str << "]";
}


dmz::String &
operator<< (dmz::String &str, const dmz::Matrix &Value) {

   dmz::Float64 matrixData[9];
   Value.to_array (matrixData);

   str << "("
      << 0 << "["
      << matrixData[0] << ", " << matrixData[1] << ", " << matrixData[2]
      << "] "
      << 1 << "["
      << matrixData[3] << ", " << matrixData[4] << ", " << matrixData[5]
      << "] "
      << 2 << "["
      << matrixData[6] << ", " << matrixData[7] << ", " << matrixData[8]
      << "])";

   return str;
}


dmz::String &
operator<< (dmz::String &str, const dmz::Quaternion &Value) {

   str << "["
      << Value.get_x () << ", "
      << Value.get_y () << ", "
      << Value.get_z () << ", "
      << Value.get_w () << "]";

   return str;
}


dmz::String &
operator<< (dmz::String &str, const dmz::UUID &Value) {

   str << Value.to_string ();
   return str;
}
