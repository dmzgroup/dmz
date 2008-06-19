#include <dmzSystemFile.h>
#include <dmzTypesHashTableStringTemplate.h>
#include <stdio.h>

/*!

\file dmzSystemFile.h
\brief Contains basic file system functions.

\class dmz::PushDirectory
\ingroup System
\brief Changes current working directory while instance exists.
\details Changes the current working directory to the specified \a Path while the object
is in scope. When the instance is deleted, the previous working directory is restored.

*/

struct dmz::PathContainer::State {

   dmz::HashTableStringTemplate<String> paths;
   dmz::HashTableStringIterator it;
   ~State () { paths.empty (); }
};

/*!

\class dmz::PathContainer
\ingroup System
\brief Container class for storing Strings containing paths.

*/

//! Base constructor.
dmz::PathContainer::PathContainer () : _state (*(new State)) {;}


//! Copy constructor.
dmz::PathContainer::PathContainer (const PathContainer &Container) :
      _state (*(new State)) {

   *this = Container;
}


//! Destructor.
dmz::PathContainer::~PathContainer () { delete &_state; }


/*!

\brief Assignment operator.
\param[in] Container PathContainer to copy.
\return Returns reference to self.

*/
dmz::PathContainer &
dmz::PathContainer::operator= (const PathContainer &Container) {

   _state.paths.copy (Container._state.paths);

   return *this;
}


//! Empties container.
void
dmz::PathContainer::empty () { _state.paths.empty (); }


/*!

\brief Adds \a Path to list.
\details Appends path to end of the path list stored in the container.
\param[in] Path String containing path to be added.

*/
void
dmz::PathContainer::add_path (const String &Path) {

   if (!_state.paths.lookup (Path)) {

      _state.paths.store (Path, new String (Path));
   }
}


/*!

\brief Get number of paths stored in container.
\return Returns the number of paths stored in the container.
*/
dmz::Int32
dmz::PathContainer::get_count () const { return _state.paths.get_count (); }


/*!

\brief Gets first \a path stored in container.
\param[out] path String containing first path in the list.
\return Returns dmz::True if a value is stored in \a path.

*/
dmz::Boolean
dmz::PathContainer::get_first (String &path) const {

   Boolean result (False);
   String *ptr =  _state.paths.get_first (_state.it);
   if (ptr) { path.flush () << *ptr; result = True; }
   return result;
}


/*!

\brief Gets next \a path stored in container.
\param[out] path String containing next path in the list.
\return Returns dmz::True if a value is stored in \a path.

*/
dmz::Boolean
dmz::PathContainer::get_next (String &path) const {

   Boolean result (False);
   String *ptr =  _state.paths.get_next (_state.it);
   if (ptr) { path.flush () << *ptr; result = True; }
   return result;
}


//! \addtogroup System
//! @{

/*!

\brief Finds file on file system.
\details Defined in dmzSystemFile.h.
This function takes three steps to locate the file. First it attempts to validate the
file as it is passed in. If the file is not found. All leading path information is
stripped from the file and then each path in the PathContainer is used to try
and locate the file. If the file is not found, each path in the PathContainer is
searched with out the leading path information stripped from the \a FileName.
\param[in] Container PathContainer with list of paths to use in the file search.
\param[in] FileName String containing the name of the file to search for.
\param[out] foundFile String containing the absolute path to the found file.
\return Returns dmz::True if the file is found.

*/
dmz::Boolean
dmz::find_file (
      const PathContainer &Container,
      const String &FileName,
      String &foundFile) {

   Boolean result (False);

   if (is_valid_path (FileName)) { result = get_absolute_path (FileName, foundFile); }

   if (!result) {

      String path, file, ext;
      split_path_file_ext (FileName, path, file, ext);

      if (file) {

         file << ext;

         Boolean done (!Container.get_first (path));

         while (!done) {

            const String CleanPath (format_path (path + "/" + file));

            if (is_valid_path (CleanPath)) {

               result = get_absolute_path (CleanPath, foundFile);
               done = True;
            }
            else { done = !Container.get_next (path); }
         }
      }
   }

   if (!result) {

      String path;

      Boolean done (!Container.get_first (path));

      while (!done) {

         const String CleanPath (format_path (path + "/" + FileName));

         if (is_valid_path (CleanPath)) {

            result = get_absolute_path (CleanPath, foundFile);
            done = True;
         }
         else { done = !Container.get_next (path); }
      }
   }

   return result;
}


/*!

\brief Converts back slashes to forward slashes.
\details Defined in dmzSystemFile.h.
This function converts all back slashes to forward slashes and then converts
multiple forward slashes into a single forward slash. The one exception is that
on win32 systems, if the path starts with two leading back slashes, these are left
unchanged in the result. The leading backslashes on a win32 system specifies a
network mounted path and the two back slashes must be present for the path to be
valid.
\param[in] Path String containing path to format.
\return Returns string containing formatted path.

*/
dmz::String
dmz::format_path (const String &Path) {

   String result (Path);

   Int32 start (0);

#if defined (_WIN32)
   if ((Path.get_char (0) == '\\') && (Path.get_char (1) == '\\')) { start = 2; }
#endif

   result.replace_sub ("\\", "/", start);
   result.replace_sub ("//", "/");

   return result;
}


/*!

\brief Splits a path into the directory, file name, and file extension.
\details Defined in dmzSystemFile.h.
The \a FullPath is formatted before processing. The file extension is returned with
the leading period. If either a path or extension is not found in the \a FullPath,
the corresponding return value is an empty string.
\param[in] FullPath String containing full path to be split up.
\param[out] path String containing the directory component of the \a FullPath.
\param[out] file String containing the file component of the \a FullPath.
\param[out] ext String containing the extension of the \a FullPath.

*/
void
dmz::split_path_file_ext (
      const String &FullPath,
      String &path,
      String &file,
      String &ext) {

   path = format_path (FullPath);
   file.flush ();
   ext.flush ();

   const Int32 Length = path.get_length ();

   Int32 slashFound = -1;
   Int32 index = Length - 1;

   Boolean done (False);

   while (!done) {

      if (path.get_char (index) == '/') { slashFound = index; done = True; }
      else { index--; if (index < 0) { done = True; } }
   }

   if (slashFound >= 0) {

      file = path.get_sub (slashFound + 1);
      path = path.get_sub (0, slashFound);
   }
   else {

      file = path;
      path.flush ();
   }

   if (file) {

      const Int32 FileLength = file.get_length ();
      Int32 dotFound = -1;

      index = FileLength - 1;
      done = False;

      while (!done) {

         if (file.get_char (index) == '.') { dotFound = index; done = True; }
         else { index--; if (index < 0) { done = True; } }
      }

      if (dotFound >= 0) {

         ext = file.get_sub (dotFound);
         file = file.get_sub (0, dotFound - 1);
      }
   }
}


/*!

\brief Read a block of data and store it in a String.
\details Defined in dmzSystemFile.h.
Attempts to read \a Size number of bytes from \a File and store them in \a buffer.
\param[in] File C FILE * to read from.
\param[in] Size Size of data to attempt to read from \a File.
\param[out] buffer String to store read data in.
\return Returns dmz::True if any data was read.

*/
dmz::Int32
dmz::read_file (const FILE *File, const Int32 Size, String &buffer) {

   Int32 result (0);

   if (Size > 0) {

      buffer.set_buffer (0, Size + 1);

      if (buffer.get_size () >= Size) {

         result = fread ((void *)buffer.get_buffer (), sizeof (char), Size, (FILE *)File);
         buffer.set_length (result);
      }
   }

   return result;
}


/*!

\brief Wrapper around Standard C fclose function.
\details Defined in dmzSystemFile.h.

*/
dmz::Boolean
dmz::close_file (const FILE *File) { return !fclose ((FILE *)File); }

//! @}
