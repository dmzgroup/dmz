#include <dmzZip.h>

#include <errno.h>
#include <string.h>
#include <time.h>
#include <unzip.h>
#include <zip.h>

struct dmz::ZipWrite::State {

   zipFile zf;
   String zipFileName;
   String fileName;
   String error;

   State () : zf (0) {;}

   Boolean zip_error (const int Err) {

      Boolean result (Err == ZIP_OK ? True : False);

      if (!result) {

         if (Err == ZIP_ERRNO) { error.flush () << strerror (errno); }
         else if (Err == ZIP_PARAMERROR) { error.flush () << "Parameter Error."; }
         else if (Err == ZIP_BADZIPFILE) { error.flush () << "Bad Zip File."; }
         else if (Err == ZIP_INTERNALERROR) { error.flush () << "Internal Zip Error."; }
         else { error.flush () << "Unknown Zip Error."; }
      }

      return result;
   }
};

dmz::ZipWrite::ZipWrite () : _state (*(new State)) {;}


dmz::ZipWrite::~ZipWrite () {

   close_zip_file ();
   delete &_state;
}


// ZipWrite Interface
dmz::Boolean
dmz::ZipWrite::open_zip_file (const String &FileName, const UInt32 Mode) {

   Boolean result (False);

   close_zip_file ();

   _state.zf = zipOpen (FileName.get_buffer (), APPEND_STATUS_CREATE);

   if (_state.zf) {

      _state.zipFileName = FileName;
      result = True;
   }

   return result;
}


dmz::String
dmz::ZipWrite::get_zip_file_name () const { return _state.zipFileName; }


dmz::Boolean
dmz::ZipWrite::close_zip_file () {

   Boolean result (False);

   if (_state.zf) { 

      close_file ();
      result = _state.zip_error (zipClose (_state.zf, 0));
   }

   _state.zf = 0;
   _state.zipFileName.flush ();

   return result;
}


dmz::Boolean
dmz::ZipWrite::open_file (const String &FileName, const Int32 Level) {

   Boolean result (False);

   if (_state.zf) {

      close_file ();

      const int RealLevel ((Level < 0) ? 9 : ((Level > 9) ? 9 : Level));

      zip_fileinfo zinfo;

      zinfo.tmz_date.tm_sec = zinfo.tmz_date.tm_min = zinfo.tmz_date.tm_hour =
      zinfo.tmz_date.tm_mday = zinfo.tmz_date.tm_min = zinfo.tmz_date.tm_year = 0;
      zinfo.dosDate = 0;
      zinfo.internal_fa = 0;
      zinfo.external_fa = 0;

      time_t tm = time (0);

      struct tm *tms = localtime (&tm);

      if (tms) {

         zinfo.tmz_date.tm_sec  = tms->tm_sec;
         zinfo.tmz_date.tm_min  = tms->tm_min;
         zinfo.tmz_date.tm_hour = tms->tm_hour;
         zinfo.tmz_date.tm_mday = tms->tm_mday;
         zinfo.tmz_date.tm_mon  = tms->tm_mon;
         zinfo.tmz_date.tm_year = tms->tm_year + 1900;
      }

      result = _state.zip_error (zipOpenNewFileInZip (
         _state.zf,
         FileName.get_buffer (),
         &zinfo,
         0, 0, 0, 0, 0, // No comments
         Z_DEFLATED,
         RealLevel));

      if (result) { _state.fileName = FileName; }
   }
   else { _state.error.flush () << "Zip archive not open."; }

   return result;
}


dmz::String
dmz::ZipWrite::get_file_name () const { return _state.fileName; }
 

dmz::Boolean
dmz::ZipWrite::write_file (const void *Data, const Int32 Size) {

   Boolean result (False);

   if (_state.zf && Data && (Size > 0)) {

      result = _state.zip_error (zipWriteInFileInZip (
         _state.zf,
         Data,
         (unsigned)Size));
   }

   return result;
}


dmz::Boolean
dmz::ZipWrite::close_file () {

   Boolean result (False);

   if (_state.zf) {

      if (_state.fileName) { result = _state.zip_error (zipCloseFileInZip (_state.zf)); }
      else { result = True; }
   }
   else { _state.error.flush () << "Zip archive not open."; }

   _state.fileName.flush ();

   return result;
}


dmz::String
dmz::ZipWrite::get_error () const { return _state.error; }


struct dmz::ZipRead::State {

   unzFile zf;
   String zipFileName;
   String fileName;
   String error;

   State () : zf (0) {;}

   Boolean zip_error (const int Err) {

      Boolean result (Err == UNZ_OK ? True : False);

      if (!result) {

         if (Err == UNZ_ERRNO) { error.flush () << strerror (errno); }
         else if (Err == UNZ_PARAMERROR) { error.flush () << "Parameter Error."; }
         else if (Err == UNZ_BADZIPFILE) { error.flush () << "Bad Zip File."; }
         else if (Err == UNZ_INTERNALERROR) { error.flush () << "Internal Zip Error."; }
         else if (Err == UNZ_CRCERROR) { error.flush () << "CRC Zip Error."; }
         else { error.flush () << "Unknown Zip Error."; }
      }

      return result;
   }
};

dmz::ZipRead::ZipRead () : _state (*(new State)) {;}


dmz::ZipRead::~ZipRead () {

   close_zip_file ();
   delete &_state;
}


// ZipRead Interface
dmz::Boolean
dmz::ZipRead::open_zip_file (const String &FileName, const UInt32 Mode) {

   Boolean result (False);

   close_zip_file ();

   _state.zf = unzOpen (FileName.get_buffer ());

   if (_state.zf) {

      _state.zipFileName = FileName;
      result = True;
   }

   return result;
}


dmz::String
dmz::ZipRead::get_zip_file_name () const { return _state.zipFileName; }


dmz::Int32
dmz::ZipRead::get_file_size () const {

   Int32 result = 0;
   unz_file_info info;

   if (_state.zip_error (
         unzGetCurrentFileInfo (_state.zf, &info, 0, 0, 0, 0, 0, 0))) {

      result = Int32 (info.uncompressed_size);
   }

   return result;
}


dmz::Boolean
dmz::ZipRead::close_zip_file () {

   Boolean result (False);

   if (_state.zf) { 

      close_file ();

      result = _state.zip_error (unzClose (_state.zf));
   }

   _state.zf = 0;
   _state.zipFileName.flush ();

   return result;
}


dmz::Boolean
dmz::ZipRead::open_file (const String &FileName) {

   Boolean result (False);

   if (_state.zf) {

      close_file ();

      result = _state.zip_error (unzLocateFile (
         _state.zf,
         FileName.get_buffer (),
         1)); // Case sensitive

      if (result) { _state.fileName = FileName; }
   }
   else { _state.error.flush () << "Zip archive not open."; }

   return result;
}


dmz::String
dmz::ZipRead::get_file_name () const { return _state.fileName; }
 

dmz::Int32
dmz::ZipRead::read_file (char *buffer, const Int32 Size) {

   Int32 result (False);

   if (_state.zf && buffer && (Size > 0)) {

      result = unzReadCurrentFile (_state.zf, (void *)buffer, (unsigned)Size);
   }

   return result;
}


dmz::Boolean
dmz::ZipRead::close_file () {

   Boolean result (False);

   if (_state.zf) { result = _state.zip_error (unzCloseCurrentFile (_state.zf)); }

   _state.fileName.flush ();

   return result;
}


dmz::String
dmz::ZipRead::get_error () const { return _state.error; }

