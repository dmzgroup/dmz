#ifndef DMZ_ZIP_DOT_H
#define DMZ_ZIP_DOT_H

#include <dmzFoundationExport.h>
#include <dmzTypesString.h>

namespace dmz {

DMZ_FOUNDATION_LINK_SYMBOL Boolean is_zip_file (const String &FileName);

class DMZ_FOUNDATION_LINK_SYMBOL ZipWriter {

   public:
      ZipWriter ();
      ~ZipWriter ();

      Boolean open_zip_file (const String &FileName, const UInt32 Mode = 0); 
      String get_zip_file_name () const;
      Boolean close_zip_file ();

      Boolean open_file (const String &FileName, const Int32 Level = -1);
      String get_file_name () const;
      Boolean write_file (const void *Data, const Int32 Size);
      Boolean close_file ();

      String get_error () const;

   protected:
      struct State;
      State &_state;

   private:
      ZipWriter (const ZipWriter &);
      const ZipWriter &operator= (const ZipWriter &);
};

class DMZ_FOUNDATION_LINK_SYMBOL ZipReader {

   public:
      ZipReader ();
      ~ZipReader ();

      Boolean open_zip_file (const String &FileName, const UInt32 Mode = 0); 
      String get_zip_file_name () const;
      Boolean close_zip_file ();

      Boolean open_file (const String &FileName);
      String get_file_name () const;
      Int32 get_file_size () const;
      Int32 read_file (char *buffer, const Int32 Size);
      Boolean close_file ();

      String get_error () const;

   protected:
      struct State;
      State &_state;

   private:
      ZipReader (const ZipReader &);
      const ZipReader &operator= (const ZipReader &);
};

};

#endif // DMZ_ZIP_DOT_H
