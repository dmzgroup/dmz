#include <dmzFoundationReaderWriterFile.h>
#include <dmzSystemFile.h>

namespace {

struct LocalShared {

   FILE *fd;
   dmz::String fileName;
   dmz::String error;

   void close () { if (fd) { dmz::close_file (fd); fd = 0; } fileName.flush (); }

   LocalShared () : fd (0) {;}
   ~LocalShared () { close (); }
};

};

/*!

\class dmz::ReaderFile
\ingroup Foundation
\brief Reads file from the file system.

*/


//! \cond
struct dmz::ReaderFile::State : public LocalShared {};
//! \endcond


//! Constructor.
dmz::ReaderFile::ReaderFile () : _state (*(new State)) {;}


//! Destructor.
dmz::ReaderFile::~ReaderFile () { delete &_state; }


dmz::Boolean
dmz::ReaderFile::open_file (const String &FileName) {

   Boolean result (False);

   _state.close ();

   _state.fd = dmz::open_file (FileName, "rb");

   if (_state.fd) {

      result = True;
      _state.fileName = FileName;
   }
   else { _state.error.flush () << "Failed opening file: " << FileName; }

   return result;
}


dmz::String
dmz::ReaderFile::get_file_name () const { return _state.fileName; }


dmz::UInt64
dmz::ReaderFile::get_file_size () const {

   return _state.fileName ? dmz::get_file_size (_state.fileName) : 0;
}


dmz::Int32
dmz::ReaderFile::read_file (void *buffer, const Int32 Size) {

   Int32 result (0);

   if (_state.fd) {

      result = dmz::read_file (_state.fd, Size, (char *)buffer);
   }
   else {

      result = -1;
      _state.error.flush () << "No file open for reading.";
   }

   return result;
}


dmz::Boolean
dmz::ReaderFile::close_file () {

   if (_state.fd) { _state.close (); }

   return True;
}


dmz::String
dmz::ReaderFile::get_error () const { return _state.error; }


//! \cond
struct dmz::WriterFile::State : public LocalShared {};
//! \endcond


/*!

\class dmz::WriterFile
\ingroup Foundation
\brief Writes files to the file system.

*/


//! Constructor.
dmz::WriterFile::WriterFile () : _state (*(new State))  {;}


//! Destructor.
dmz::WriterFile::~WriterFile () { delete &_state; }


dmz::Boolean
dmz::WriterFile::open_file (const String &FileName) {

   Boolean result (False);

   _state.close ();

   _state.fd = dmz::open_file (FileName, "wb");

   if (_state.fd) {

      result = True;
      _state.fileName = FileName;
   }
   else { _state.error.flush () << "Failed opening file: " << FileName; }

   return result;
}


dmz::String
dmz::WriterFile::get_file_name () const { return _state.fileName; }


dmz::Boolean
dmz::WriterFile::write_file (const void *Buffer, const Int32 Size) {

   Boolean result (False);

   if (_state.fd && Buffer && (Size > 0)) {

      result = (fwrite (Buffer, Size, sizeof (UInt8), _state.fd) == Size);
   }
   else { _state.error.flush () << "No file open for writing."; }

   return result;
}


dmz::Boolean
dmz::WriterFile::close_file () {

   _state.close ();

   return True;
}


dmz::String
dmz::WriterFile::get_error () const { return _state.error; }

