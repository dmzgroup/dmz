#include <dmzSystemStreamString.h>

/*!

\class dmz::StreamString
\brief Writes to a dmz::String

*/

struct dmz::StreamString::State {

   String *str;

   State (String *theStr) : str (theStr) {;}
};


//! Default constructor.
dmz::StreamString::StreamString () : _state (*(new State (0))) {;}


//! String Constructor.
dmz::StreamString::StreamString (String &value) : _state (*(new State (&value))) {;}


/*!

\brief Destructor.
\note If the StreamString contains a String for writing, that String
is not deleted when the StringStream is destroyed.

*/
dmz::StreamString::~StreamString () { delete &_state; }


/*!

\brief Sets current String for writing.
\param[in] value String to be written to.

*/
void
dmz::StreamString::set_string (String *value) { _state.str = value; }


/*!

\brief Return String currently being written to.
\return Returns pointer to String being used for writing.

*/
dmz::String *
dmz::StreamString::get_string () { return _state.str; }


/*!

\brief Logical not operator.
\return Returns dmz::True if the stream contains a valid pointer to a String.

*/
dmz::Boolean
dmz::StreamString::operator! () const { return _state.str != 0; }


/*!

\brief Writes raw binary data to the String.
\return Returns reference to self.

*/
dmz::Stream &
dmz::StreamString::write_raw_data (const UInt8 *Data, const Int32 Size) {

   if (_state.str) {

      const String Value ((const char *)Data, Size, Size + 1);
      *(_state.str) << Value;
   }

   return *this;
}

//! Currently is a no-op when streaming to a string.
dmz::Stream &
dmz::StreamString::flush () {

   return *this;
}


//! Appends a new line to the String.
dmz::Stream &
dmz::StreamString::newline () {

   if (_state.str) { *(_state.str) << "\n"; }

   return *this;
}


dmz::Stream &
dmz::StreamString::operator<< (const UInt16 Value) {

   if (_state.str) { *(_state.str) << Value; }

   return *this;
}


dmz::Stream &
dmz::StreamString::operator<< (const UInt32 Value) {

   if (_state.str) { *(_state.str) << Value; }

   return *this;
}


dmz::Stream &
dmz::StreamString::operator<< (const UInt64 Value) {

   if (_state.str) { *(_state.str) << Value; }

   return *this;
}


dmz::Stream &
dmz::StreamString::operator<< (const Int16 Value) {

   if (_state.str) { *(_state.str) << Value; }

   return *this;
}


dmz::Stream &
dmz::StreamString::operator<< (const Int32 Value) {

   if (_state.str) { *(_state.str) << Value; }

   return *this;
}


dmz::Stream &
dmz::StreamString::operator<< (const Int64 Value) {

   if (_state.str) { *(_state.str) << Value; }

   return *this;
}


dmz::Stream &
dmz::StreamString::operator<< (const Float32 Value) {

   if (_state.str) { *(_state.str) << Value; }

   return *this;
}


dmz::Stream &
dmz::StreamString::operator<< (const Float64 Value) {

   if (_state.str) { *(_state.str) << Value; }

   return *this;
}


dmz::Stream &
dmz::StreamString::operator<< (const String &Value) {

   if (_state.str) { *(_state.str) << Value; }

   return *this;
}


dmz::Stream &
dmz::StreamString::operator<< (const char Value) {

   if (_state.str) { *(_state.str) << Value; }

   return *this;
}


dmz::Stream &
dmz::StreamString::operator<< (const char *Value) {

   if (_state.str) { *(_state.str) << Value; }

   return *this;
}


dmz::Stream &
dmz::StreamString::operator<< (const void *Value) {

   if (_state.str) { *(_state.str) << Value; }

   return *this;
}


dmz::Stream &
dmz::StreamString::operator<< (stream_operator_func func) { return func (*this); }

