#ifndef DMZ_FOUNDATION_ZIP_DOT_H
#define DMZ_FOUNDATION_ZIP_DOT_H

#include <dmzFoundationExport.h>
#include <dmzFoundationReaderWriter.h>
#include <dmzTypesString.h>

namespace dmz {

class StringContainer;

DMZ_FOUNDATION_LINK_SYMBOL Boolean is_zip_file (const String &FileName);

class DMZ_FOUNDATION_LINK_SYMBOL ReaderZip : public Reader{

   public:
      ReaderZip ();
      virtual ~ReaderZip ();

      Boolean open_zip_file (const String &FileName, const UInt32 Mode = 0); 
      String get_zip_file_name () const;
      Boolean get_file_list (StringContainer &container) const;
      Boolean close_zip_file ();

      // Reader Interface
      virtual Boolean open_file (const String &FileName);
      virtual String get_file_name () const;
      virtual UInt64 get_file_size () const;
      virtual Int32 read_file (void *buffer, const Int32 Size);
      virtual Boolean close_file ();

      virtual String get_error () const;

   protected:
      //! \cond
      struct State;
      State &_state;
      //! \endcond

   private:
      ReaderZip (const ReaderZip &);
      const ReaderZip &operator= (const ReaderZip &);
};

class DMZ_FOUNDATION_LINK_SYMBOL WriterZip : public Writer {

   public:
      WriterZip ();
      virtual ~WriterZip ();

      Boolean open_zip_file (const String &FileName, const UInt32 Mode = 0); 
      String get_zip_file_name () const;
      Boolean close_zip_file ();

      Int32 set_level (const Int32 Level);
      Int32 get_level () const;

      // Writer Interface
      virtual Boolean open_file (const String &FileName);
      virtual String get_file_name () const;
      virtual Boolean write_file (const void *Buffer, const Int32 Size);
      virtual Boolean close_file ();

      virtual String get_error () const;

   protected:
      //! \cond
      struct State;
      State &_state;
      //! \endcond

   private:
      WriterZip (const WriterZip &);
      const WriterZip &operator= (const WriterZip &);
};

};

#endif // DMZ_FOUNDATION_ZIP_DOT_H
