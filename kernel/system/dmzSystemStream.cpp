#include <dmzSystemStream.h>
#include <dmzTypesString.h>
#include <dmzTypesStringContainer.h>
#include <dmzTypesUUID.h>

/*!

\file dmzSystemStream.h
\brief Contains Stream interfaces.
\details Pure virtual interface for Stream class as well a some stream operators
for basic DMZ Types (i.e. Vector, Matrix, UUID etc.)

\class dmz::Stream
\ingroup System
\brief Stream pure virtual interface.
\details The Stream class provides an interface for writing data. Derived class
write the data to various destinations depending on the actual implementation.
\sa dmz::StreamFile \n dmz::StreamString \n dmz::StreamLog

*/

//! \addtogroup System
//! @{

/*!

\brief Calls dmz::Stream::flush.
\details Defined in dmzSystemStream.h.
\param[in] stream Stream object.
\return Returns reference to \a stream.

*/
dmz::Stream &
dmz::flush (Stream &stream) {

   return stream.flush ();
}


/*!

\brief Calls dmz::Stream::newline.
\details Defined in dmzSystemStream.h.
\param[in] stream Stream object.
\return Returns reference to \a stream.

*/
dmz::Stream &
dmz::endl (Stream &stream) {

   return stream.newline ();
}


//! Converts dmz::Mask to a dmz::String and appends it to the \a stream.
dmz::Stream &
operator<< (dmz::Stream &stream, const dmz::Mask &Value) {

   dmz::String result;
   stream << (result << Value);
   return stream;
}


//! Converts dmz::Matrix to a dmz::String and appends it to the \a stream.
dmz::Stream &
operator<< (dmz::Stream &stream, const dmz::Matrix &Value) {

   dmz::String result;
   stream << (result << Value);
   return stream;
}


//! Converts dmz::Quaternion to a dmz::String and appends it to the \a stream.
dmz::Stream &
operator<< (dmz::Stream &stream, const dmz::Quaternion &Value) {

   dmz::String result;
   stream << (result << Value);
   return stream;
}


//! Converts dmz::StingContainer to a dmz::String and appends it to the \a stream.
dmz::Stream &
operator<< (dmz::Stream &stream, const dmz::StringContainer &Value) {

   dmz::String result;
   stream << (result << Value);
   return stream;
}

//! Converts dmz::UUID to a dmz::String and appends it to the \a stream.
dmz::Stream &
operator<< (dmz::Stream &stream, const dmz::UUID &Value) {

   stream << Value.to_string ();
   return stream;
}


//! Converts dmz::Vector to a dmz::String and appends it to the \a stream.
dmz::Stream &
operator<< (dmz::Stream &stream, const dmz::Vector &Value) {

   dmz::String result;
   stream << (result << Value);
   return stream;
}

//! @}
