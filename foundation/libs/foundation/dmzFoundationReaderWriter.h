#ifndef DMZ_FOUNDATION_READER_WRITER_DOT_H
#define DMZ_FOUNDATION_READER_WRITER_DOT_H

#include <dmzTypesString.h>

namespace dmz {

class Reader {

   public:
      virtual ~Reader () {;}

      virtual Boolean open_file (const String &FileName) = 0;
      virtual String get_file_name () const = 0;
      virtual UInt64 get_file_size () const = 0;
      virtual Int32 read_file (void *buffer, const Int32 Size) = 0;
      virtual Boolean close_file () = 0;

      virtual String get_error () const = 0;

   protected:
      Reader () {;}

   private:
      Reader (const Reader &);
      const Reader &operator= (const Reader &);
};

class Writer {

   public:
      virtual ~Writer () {;}

      virtual Boolean open_file (const String &FileName) = 0;
      virtual String get_file_name () const = 0;
      virtual Boolean write_file (const void *Buffer, const Int32 Size) = 0;
      virtual Boolean close_file () = 0;

      virtual String get_error () const = 0;

   protected:
      Writer () {;}

   private:
      Writer (const Writer &);
      const Writer &operator= (const Writer &);
};

};

#endif // DMZ_FOUNDATION_READER_WRITER_DOT_H
