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


struct dmz::ReaderFile::State : public LocalShared {

};


dmz::ReaderFile::ReaderFile () : _state (*(new State)) {;}


dmz::ReaderFile::~ReaderFile () { delete &_state; }


dmz::Boolean
dmz::ReaderFile::open_file (const String &FileName) {

   Boolean result (False);

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

   return result;
}


dmz::Boolean
dmz::ReaderFile::close_file () {

   Boolean result (False);

   return result;
}


dmz::String
dmz::ReaderFile::get_error () const { return _state.error; }


struct dmz::WriterFile::State : public LocalShared {

};


dmz::WriterFile::WriterFile () : _state (*(new State))  {;}


dmz::WriterFile::~WriterFile () { delete &_state; }


dmz::Boolean
dmz::WriterFile::open_file (const String &FileName) {

   Boolean result (False);

   return result;
}


dmz::String
dmz::WriterFile::get_file_name () const { return _state.fileName; }


dmz::Boolean
dmz::WriterFile::write_file (const void *Buffer, const Int32 Size) {

   Boolean result (False);

   return result;
}


dmz::Boolean
dmz::WriterFile::close_file () {

   Boolean result (False);

   return result;
}


dmz::String
dmz::WriterFile::get_error () const { return _state.error; }

