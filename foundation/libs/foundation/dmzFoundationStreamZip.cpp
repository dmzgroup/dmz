#include <dmzFoundationStreamZip.h>

#include <errno.h>
#include <string.h>
#include <time.h>

/*!

\class dmz::StreamZip
\ingroup Foundation
\brief Allows streaming of data to a zip archive.

*/


//! Constructor
dmz::StreamZip::StreamZip () {;}


//! Destructor.
dmz::StreamZip::~StreamZip () {;}


// Stream Interface
dmz::Stream &
dmz::StreamZip::write_raw_data (const UInt8 *Data, const Int32 Size) {

   write_file ((const void *)Data, Size);
   return *this;
}


dmz::Stream &
dmz::StreamZip::flush () {

   // Do nothing
   return *this;
}


dmz::Stream &
dmz::StreamZip::newline () {

//   static const UInt8 Data[3] = { 0x0D, 0x0A, 0 };
//   return write_raw_data (Data, 2);

   static const UInt8 Data[3] = { 0x0A, 0 };
   return write_raw_data (Data, 1);
}


dmz::Stream &
dmz::StreamZip::operator<< (const UInt16 Value) {

   String out; out << Value;
   
   return *this << out;
}


dmz::Stream &
dmz::StreamZip::operator<< (const UInt32 Value) {

   String out; out << Value;
   
   return *this << out;
}


dmz::Stream &
dmz::StreamZip::operator<< (const UInt64 Value) {

   String out; out << Value;
   
   return *this << out;
}


dmz::Stream &
dmz::StreamZip::operator<< (const Int16 Value) {

   String out; out << Value;
   
   return *this << out;
}


dmz::Stream &
dmz::StreamZip::operator<< (const Int32 Value) {

   String out; out << Value;
   
   return *this << out;
}


dmz::Stream &
dmz::StreamZip::operator<< (const Int64 Value) {

   String out; out << Value;
   
   return *this << out;
}


dmz::Stream &
dmz::StreamZip::operator<< (const Float32 Value) {

   String out; out << Value;
   
   return *this << out;
}


dmz::Stream &
dmz::StreamZip::operator<< (const Float64 Value) {

   String out; out << Value;
   
   return *this << out;
}


dmz::Stream &
dmz::StreamZip::operator<< (const String &Value) {

   write_file ((const void *)Value.get_buffer (), Value.get_length ());
   return *this;
}


dmz::Stream &
dmz::StreamZip::operator<< (const char Value) {

   char data[2] = {0, 0};
   data[0] = Value;
   String out; out << data;
   return *this << out;
}


dmz::Stream &
dmz::StreamZip::operator<< (const char *Data) {

   String out; out << Data;

   return *this << out;
}


dmz::Stream &
dmz::StreamZip::operator<< (const void *Value) {

   String out; out << Value;

   return *this << out;
}


dmz::Stream &
dmz::StreamZip::operator<< (stream_operator_func func) { return func (*this); }



