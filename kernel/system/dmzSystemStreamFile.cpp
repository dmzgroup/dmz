#include <dmzSystemStreamFile.h>
#include <dmzTypesString.h>

/*!

\class dmz::StreamFile
\ingroup System
\brief Writes to a Standard C FILE stream.

\class dmz::StreamFileOut
\ingroup System
\brief Writes to stdout stream.

\class dmz::StreamFileErr
\ingroup System
\brief Writes to stderr stream.

*/

struct dmz::StreamFile::State {

   FILE *file;

   State (FILE *theFile) : file (theFile) {;}
};


/*!

\brief Default Constructor.
\details Class is created with internal file pointer set to NULL.

*/
dmz::StreamFile::StreamFile () : _state (*(new State (0))) {;}


//! Constructor take Standard C FILE pointer.
dmz::StreamFile::StreamFile (FILE *handle) : _state (*(new State (handle))) {;}


//! Destructor.
dmz::StreamFile::~StreamFile () { delete &_state; }


/*!

\brief Gets current Standard C FILE pointer.
\return Returns current Standard C FILE pointer being used to stream data out.

*/
FILE *
dmz::StreamFile::get_file_handle () { return _state.file; }


/*!

\brief Logical not operator.
\return Returns dmz::True if a valid Standard C FILE pointer has been set.

*/
dmz::Boolean
dmz::StreamFile::operator! () const { return _state.file != 0; }


/*!

\brief Writes raw binary data to the C FILE stream.
\return Returns reference to self.

*/
dmz::Stream &
dmz::StreamFile::write_raw_data (const UInt8 *Data, const Int32 Size) {

   if (_state.file) {

      fwrite ((void *)Data, Size, sizeof (UInt8), _state.file);
   }

   return *this;
}


/*!

\brief Flushes Standard C FILE stream.
\return Returns reference to self.

*/
dmz::Stream &
dmz::StreamFile::flush () {

   if (_state.file) { fflush (_state.file); }
   return *this;
}


/*!

\brief Writes a new line to the Standard C FILE stream.
\return Returns reference to self.

*/
dmz::Stream &
dmz::StreamFile::newline () {

   if (_state.file) { fprintf (_state.file, "\n"); }
   return *this;
}


dmz::Stream &
dmz::StreamFile::operator<< (const UInt16 Value) {

   if (_state.file) {

      String out; out << Value; fprintf (_state.file, "%s", out.get_buffer ());
   }

   return *this;
}


dmz::Stream &
dmz::StreamFile::operator<< (const UInt32 Value) {

   if (_state.file) {

      String out; out << Value; fprintf (_state.file, "%s", out.get_buffer ());
   }

   return *this;
}


dmz::Stream &
dmz::StreamFile::operator<< (const UInt64 Value) {

   if (_state.file) {

      String out; out << Value; fprintf (_state.file, "%s", out.get_buffer ());
   }

   return *this;
}


dmz::Stream &
dmz::StreamFile::operator<< (const Int16 Value) {

   if (_state.file) {

      String out; out << Value; fprintf (_state.file, "%s", out.get_buffer ());
   }

   return *this;
}


dmz::Stream &
dmz::StreamFile::operator<< (const Int32 Value) {

   if (_state.file) {

      String out; out << Value; fprintf (_state.file, "%s", out.get_buffer ());
   }

   return *this;
}


dmz::Stream &
dmz::StreamFile::operator<< (const Int64 Value) {

   if (_state.file) {

      String out; out << Value; fprintf (_state.file, "%s", out.get_buffer ());
   }

   return *this;
}


dmz::Stream &
dmz::StreamFile::operator<< (const Float32 Value) {

   if (_state.file) {

      String out; out << Value; fprintf (_state.file, "%s", out.get_buffer ());
   }

   return *this;
}


dmz::Stream &
dmz::StreamFile::operator<< (const Float64 Value) {

   if (_state.file) {

      String out; out << Value; fprintf (_state.file, "%s", out.get_buffer ());
   }

   return *this;
}


dmz::Stream &
dmz::StreamFile::operator<< (const String &Value) {

   if (_state.file) {

      String out; out << Value; fprintf (_state.file, "%s", out.get_buffer ());
   }

   return *this;
}


dmz::Stream &
dmz::StreamFile::operator<< (const char Value) {

   if (_state.file) {

      String out; out << Value; fprintf (_state.file, "%s", out.get_buffer ());
   }

   return *this;
}


dmz::Stream &
dmz::StreamFile::operator<< (const char *Value) {

   if (_state.file) {

      String out; out << Value; fprintf (_state.file, "%s", out.get_buffer ());
   }

   return *this;
}


dmz::Stream &
dmz::StreamFile::operator<< (const void *Value) {

   if (_state.file) {

      String out; out << Value; fprintf (_state.file, "%s", out.get_buffer ());
   }

   return *this;
}


dmz::Stream &
dmz::StreamFile::operator<< (stream_operator_func func) { return func (*this); }


//! Creates dmz::Stream with Standard C FILE pointer set to stdout.
dmz::StreamFileOut::StreamFileOut () : StreamFile (stdout) {;}


//! Destructor.
dmz::StreamFileOut::~StreamFileOut () {;}


//! Creates dmz::Stream with Standard C FILE pointer set to stderr.
dmz::StreamFileErr::StreamFileErr () : StreamFile (stderr) {;}


//! Destructor.
dmz::StreamFileErr::~StreamFileErr () {;}

