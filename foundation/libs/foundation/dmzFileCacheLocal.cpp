#include "dmzFileCacheLocal.h"
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzSystemFile.h>

/*!

\class dmz::FileCacheLocal
\ingroup Foundation
\brief Finds files in local cache.
\details This class finds files only on the local file system.
This class should not be used directly. To access the FileCache interface call
dmz::FileCache::get_interface().
\sa dmz::FileCache

*/


struct dmz::FileCacheLocal::State {

   PathContainer paths;
};


/*!

\brief Constructor.
\param[in] context Pointer to the runtime context.

*/
dmz::FileCacheLocal::FileCacheLocal (RuntimeContext *context) :
      FileCache (context),
      _state (*(new State)) {;}


/*!

\brief Configures the file cache.
\details Parses the config context tree and finds the paths for the local file cache.
The file cache paths may be specified as follows:
\code
<dmz>
<dmzFileCacheLocal>
   <path value="file/path/"/>
   <path value="another/file/path/"/>
</dmzFileCacheLocal>
</dmz>
\endcode

*/
void
dmz::FileCacheLocal::configure (Config &data) {

   Config pathList;

   if (data.lookup_all_config ("dmz.dmzFileCacheLocal.path", pathList)) {

      Config path;
      ConfigIterator it;

      Boolean found (pathList.get_first_config (it, path));

      while (found) {

         _state.paths.add_path (config_to_string (path));
         found = pathList.get_next_config (it, path);
      }
   }
}


dmz::FileCacheLocal::~FileCacheLocal () { delete &_state; }


//! Adds a path to the local file cache search path.
void
dmz::FileCacheLocal::add_path (const String &Path) {

   _state.paths.add_path (Path);
}


dmz::Boolean
dmz::FileCacheLocal::process_all_requests (const Float64 TimeOut) {

   // Nothing to do here since all files are loaded immediately.
   return True;
}


void
dmz::FileCacheLocal::add_file_request (
      const String &FileLocation,
      const String &FileName,
      FileCacheAction &action) {

   String foundFile;

   if (is_valid_path (FileName)) { get_absolute_path (FileName, foundFile); }

   if (!foundFile && FileLocation) {

      PathContainer pc;
      pc.add_path (FileLocation);

      find_file (pc, FileName, foundFile);
   }

   if (!foundFile) { find_file (_state.paths, FileName, foundFile); }

   FileCacheResultEnum result (CacheFileFound);

   if (!foundFile) { result = FileNotFoundAtLocation; }

   action.process_file (result, foundFile, FileLocation, FileName);
}

