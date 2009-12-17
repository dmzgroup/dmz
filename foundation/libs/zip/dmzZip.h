#ifndef DMZ_ZIP_DOT_H
#define DMZ_ZIP_DOT_H

#include <dmzTypesString.h>
#include <dmzZipExport.h>

namespace dmz {

class DMZ_ZIP_LINK_SYMBOL ZipWrite {

   public:
      ZipWrite ();
      ~ZipWrite ();

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
      ZipWrite (const ZipWrite &);
      const ZipWrite &operator= (const ZipWrite &);
};

class DMZ_ZIP_LINK_SYMBOL ZipRead {

   public:
      ZipRead ();
      ~ZipRead ();

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
      ZipRead (const ZipRead &);
      const ZipRead &operator= (const ZipRead &);
};

};

#endif // DMZ_ZIP_DOT_H
