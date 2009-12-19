#ifndef DMZ_FOUNDATION_READER_WRITER_FILE_DOT_H
#define DMZ_FOUNDATION_READER_WRITER_FILE_DOT_H

#include <dmzFoundationExport.h>
#include <dmzFoundationReaderWriter.h>
#include <dmzTypesString.h>

namespace dmz {

class DMZ_FOUNDATION_LINK_SYMBOL ReaderFile : public Reader {

   public:
      ReaderFile ();
      virtual ~ReaderFile ();

      virtual Boolean open_file (const String &FileName);
      virtual String get_file_name () const;
      virtual UInt64 get_file_size () const;
      virtual Int32 read_file (void *buffer, const Int32 Size);
      virtual Boolean close_file ();

      virtual String get_error () const;

   protected:
      struct State;
      State &_state;

   private:
      ReaderFile (const ReaderFile &);
      const ReaderFile &operator= (const ReaderFile &);
};

class DMZ_FOUNDATION_LINK_SYMBOL WriterFile : public Writer {

   public:
      WriterFile ();
      virtual ~WriterFile ();

      virtual Boolean open_file (const String &FileName);
      virtual String get_file_name () const;
      virtual Boolean write_file (const void *Buffer, const Int32 Size);
      virtual Boolean close_file ();

      virtual String get_error () const;

   protected:
      struct State;
      State &_state;

   private:
      WriterFile (const WriterFile &);
      const WriterFile &operator= (const WriterFile &);
};

};

#endif // DMZ_FOUNDATION_READER_WRITER_FILE_DOT_H
