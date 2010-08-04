#include <dmzSystem.h>
#include <dmzSystemFile.h>
#include <dmzTypesBase.h>
#include <dmzTypesString.h>
#include <dmzTypesStringTokenizer.h>

#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <sys/param.h> // for MAXPATHLEN
#include <sys/stat.h> // for stat, mkdir
#include <sys/types.h> // for stat, mkdir
#include <unistd.h>

//! \addtogroup System
//! @{

/*!

\brief Gets a list of files found at the specified path.
\details Defined in dmzSystemFile.h.
\param[in] Path String containing path used to find the file list.
\param[out] container dmz::StringContainer used to store files found at the specified
\a Path
\return Returns dmz::True if the specified \a Path is valid.
\sa dmz::get_directory_list

*/
dmz::Boolean
dmz::get_file_list (
      const String &Path,
      StringContainer &container) {

   Boolean result (False);

   container.clear ();

   if (is_valid_path (Path)) {

      DIR *dir = opendir (Path.get_buffer ());

      if (dir) {

         result = True;

         struct dirent *current = readdir (dir);

         while (current) {

            if (current->d_type != DT_DIR) {

               container.add (current->d_name);
            }

            current = readdir (dir);
         }

         closedir (dir); dir = 0;
      }
   }

   return result;
}


/*!

\brief Gets a list of directories found at the specified path.
\details Defined in dmzSystemFile.h.
\param[in] Path String containing path used to find the directory list.
\param[out] container dmz::StringContainer used to store directories found at the specified
\a Path
\return Returns dmz::True if the specified \a Path is valid.
\sa dmz::get_file_list

*/
dmz::Boolean
dmz::get_directory_list (
      const String &Path,
      StringContainer &container) {

   Boolean result (False);

   container.clear ();

   if (is_valid_path (Path)) {

      DIR *dir = opendir (Path.get_buffer ());

      if (dir) {

         result = True;

         struct dirent *current = readdir (dir);

         while (current) {

            if (current->d_type == DT_DIR) {

               Boolean store (True);

               // Filter out . and .. directories
               if (current->d_name[0] == '.') {

                  if ((current->d_name[1] == '\0') ||
                     ((current->d_name[1] == '.') && (current->d_name[2] == '\0'))) {

                     store = False;
                  }
               }

               if (store) { container.add (current->d_name); }
            }

            current = readdir (dir);
         }

         closedir (dir); dir = 0;
      }
   }

   return result;
}


/*!

\brief Determines if a path is valid.
\details Defined in dmzSystemFile.h. Works with both files and directories.
\param[in] Path String containing path to validate.
\return Returns dmz::True if \a Path is valid.

*/
dmz::Boolean
dmz::is_valid_path (const String &Path) {

   struct stat s;
   return Path && !stat (Path.get_buffer (), &s);
}


/*!

\brief Determines if a path is a directory.
\details Defined in dmzSystemFile.h. Works with both files and directories.
\param[in] Path String containing path to validate.
\return Returns dmz::True if \a Path is a valid directory.

*/
dmz::Boolean
dmz::is_directory (const String &Path) {

   Boolean result (False);

   struct stat s;

   if (!stat (Path.get_buffer (), &s)) {

      if (S_ISDIR (s.st_mode)) { result = True; }
   }

   return result;
}


/*!

\brief Determines the size of a file.
\details Defined in dmzSystemFile.h.
\param[in] Path String containing path to the file.
\return Returns the size of the file in bytes.

*/
dmz::UInt64
dmz::get_file_size (const String &Path) {

   UInt64 result (0);

   struct stat s;

   if (!stat (Path.get_buffer (), &s)) {

      result = UInt64 (s.st_size > 0 ? s.st_size : 0);
   }

   return result;
}


/*!

\brief Gets current working directory.
\details Defined in dmzSystemFile.h.
\return Returns String containing current working directory.

*/
dmz::String
dmz::get_current_directory () {

   String result;

   char path[MAXPATHLEN];

   if (getcwd (path, MAXPATHLEN)) { result <<  path << "/"; }

   return format_path (result);
}


/*!

\brief Changes current working directory.
\details Defined in dmzSystemFile.h.
\param[in] Path String containing target directory.
\return Returns dmz::True if working directory was successfully changed.

*/
dmz::Boolean
dmz::change_directory (const String &Path) {

   return !chdir (Path.get_buffer ());
}


/*!

\brief Creates a directory.
\details Defined in dmzSystemFile.h. Function will create a path as deep as is
requested in \a Path. Function returns dmz::True if path already exists.
\param[in] Path String containing directory to create.
\return Returns dmz::True if directory was successfully created.

*/
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

            if (mkdir (dir.get_buffer (), S_IRUSR | S_IWUSR | S_IXUSR) != 0) {

               result = False;
            }
         }

         if (result) { part = st.get_next (); }
         else { part.flush (); }
      }
   }

   return result;
}


/*!

\brief Finds absolute path.
\details Defined in dmzSystemFile.h.
\param[in] Path String containing path used to find the absolute (a.k.a. canonical) path
\param[out] absPath String in which the found absolute path is stored.
\return Returns dmz::True if absolute path is found.

*/
dmz::Boolean
dmz::get_absolute_path (const String &Path, String &absPath) {

   Boolean result (False);

   if (is_valid_path (Path)) {

      if (is_directory (Path)) {

         const PushDirectory Push (Path);

         if (Push.is_valid ()) { result = True; absPath = get_current_directory (); }
      }
      else {

         String spath, file, ext;
         split_path_file_ext (Path, spath, file, ext);

         if (!spath) { spath = "."; }

         const PushDirectory Push (spath);

         if (Push.is_valid ()) {

            result = True;
            absPath.flush () <<  get_current_directory () << file << ext;
         }
      }

      absPath = format_path (absPath);
   }
   else { absPath.flush (); }

   return result;
}


/*!

\brief Removes specified file.
\details Defined in dmzSystemFile.h.
\param[in] Path String containing the path of the file to remove.
\return Returns dmz::True if the file was successfully removed.

*/
dmz::Boolean
dmz::remove_file (const String &Path) { return remove (Path.get_buffer ()) == 0; }


/*!

\brief Wrapper around Standard C fopen
\details Defined in dmzSystemFile.h.

*/
FILE *
dmz::open_file (const String &FileName, const String &Opts) {

   return fopen (FileName.get_buffer (), Opts.get_buffer ());
}


/*!

\brief Returns user's home directory.
\details Defined in dmzSystemFile.h.
\return Returns String containing users home directory.

*/
dmz::String
dmz::get_home_directory () {

   String result (get_env ("HOME"));

   if (!result) {

      const String StartDir (get_current_directory ());

      if (change_directory ("~")) { result = get_current_directory (); }

      change_directory (StartDir);
   }

   return result;
}

//! @}
