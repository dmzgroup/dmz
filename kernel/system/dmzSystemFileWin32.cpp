#include <dmzSystem.h>
#include <dmzSystemFile.h>
#include <dmzTypesBase.h>
#include <dmzTypesString.h>
#include <dmzTypesStringTokenizer.h>
#include <stdio.h>
#include <windows.h>


dmz::Boolean
dmz::get_file_list (
      const String &Path,
      PathContainer &container) {

   Boolean result (False);

   container.empty ();

   if (is_valid_path (Path)) {

      result = True;

      WIN32_FIND_DATA data;
      HANDLE handle = FindFirstFile ((Path + "/*").get_buffer (), &data);

      if (handle != INVALID_HANDLE_VALUE) {

         do {

            if (!(data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {

               container.add_path (data.cFileName);
            }
         } while (FindNextFile (handle, &data));

         FindClose (handle);
      }
   }

   return result;
}


dmz::Boolean
dmz::get_directory_list (
      const String &Path,
      PathContainer &container) {

   Boolean result (False);

   container.empty ();

   if (is_valid_path (Path)) {

      result = True;

      WIN32_FIND_DATA data;
      HANDLE handle = FindFirstFile ((Path + "/*").get_buffer (), &data);

      if (handle != INVALID_HANDLE_VALUE) {

         do {

            if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {

               Boolean store (True);

               if (data.cFileName[0] == '.') {

                  if ((data.cFileName[1] == '\0') ||
                        ((data.cFileName[1] == '.') && (data.cFileName[2] == '\0'))) {

                     store = False;
                  }
               }

               if (store) { container.add_path (data.cFileName); }
            }
         } while (FindNextFile (handle, &data));
      }

      FindClose (handle);
   }

   return result;
}


dmz::Boolean
dmz::is_valid_path (const String &Path) {

   WIN32_FILE_ATTRIBUTE_DATA data;
   memset ((void *) &data, '\0', sizeof (WIN32_FILE_ATTRIBUTE_DATA));
   return GetFileAttributesEx (Path.get_buffer (), GetFileExInfoStandard, &data) != 0;
}


dmz::Boolean
dmz::is_directory (const String &Path) {

   Boolean result (False);

   WIN32_FILE_ATTRIBUTE_DATA data;
   memset ((void *) &data, '\0', sizeof (WIN32_FILE_ATTRIBUTE_DATA));

   if (GetFileAttributesEx (Path.get_buffer (), GetFileExInfoStandard, &data)) {

      if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) { result = True; }
   }

   return result;
}


dmz::String
dmz::get_current_directory () {

   String result;
   const DWORD Size = GetCurrentDirectory (0, 0);
   char *ptr = (char *)malloc ((Size + 1) * sizeof (char));
   if (ptr && GetCurrentDirectory (Size + 1, ptr)) { result << ptr << "/"; }
   if (ptr) { free (ptr); ptr = 0; }

   return format_path (result);
}


dmz::Boolean
dmz::change_directory (const String &Path) {

   Boolean result (False);
   if (SetCurrentDirectory (Path.get_buffer ())) { result = True; }
   return result;
}


dmz::Boolean
dmz::create_directory (const String &Path) {

   Boolean result (False);

   if (Path) {

      result = True;

      const String FormattedPath (format_path (Path));

      StringTokenizer st (FormattedPath, '/');

      String dir;

      if (FormattedPath.get_char (0) == '/') { dir << "/"; }

      String part (st.get_next ());

      while (part) {

         if (dir) { dir << "/" << part; }
         else { dir = part; }

         if (!is_valid_path (dir)) {

            if (!CreateDirectory (dir.get_buffer (), 0)) { result = False; }
         }

         if (result) { part = st.get_next (); }
         else { part.flush (); }
      }
   }

   return result;
}


dmz::Boolean
dmz::get_absolute_path (const String &Path, String &absPath) {

   Boolean result (False);

   const DWORD Size = GetFullPathName (Path.get_buffer (), 0, 0, 0);
   char *ptr = (char *)malloc ((Size + 1) * sizeof (char));
   if (ptr && GetFullPathName (Path.get_buffer (), Size + 1, ptr, 0)) {

      absPath.flush () << ptr;
      if (is_directory (absPath)) { absPath << "/"; }
      absPath = format_path (absPath);
      result = True;
   }
   if (ptr) { free (ptr); ptr = 0; }

   return result;
}


dmz::Boolean
dmz::remove_file (const String &FileName) {

   Boolean result (False);

   if (DeleteFile (FileName.get_buffer ())) { result = True; }

   return result;
}


FILE *
dmz::open_file (const String &FileName, const String &Opts) {

   FILE *file (0);

#if _MSC_VER >= 1400
   fopen_s (&file, FileName.get_buffer (), Opts.get_buffer ());
#else
   file = fopen (FileName.get_buffer (), Opts.get_buffer ());
#endif

   return file;
}


dmz::String
dmz::get_home_directory () {

   String result (get_env ("USERPROFILE"));

   if (!result) {

      result = get_env ("HOMEPATH");

      if (!result) { result = get_env ("HOMEDRIVE"); }
   }

   return result;
}

