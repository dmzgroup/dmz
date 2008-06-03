#ifndef DMZ_FILE_CACHE_DOT_H
#define DMZ_FILE_CACHE_DOT_H

#include <dmzRuntimeRTTINamed.h>
#include <dmzTypesString.h>

namespace dmz {

   //! FileCache interface name.
   const char FileCacheInterfaceName[] = "FileCacheInterface";

   enum FileCacheResultEnum {
      CacheFileFound = 1, //!< File found.
      UnknownFileLocation, //!< Unknown file location.
      FileNotFoundAtLocation, //!< File not found at given location.
   };

   class RuntimeContext;

   class FileCacheAction {

      public:
         virtual void process_file (
            const FileCacheResultEnum RequestResult,
            const String &LocalFilePath,
            const String &RequestedFileLocation,
            const String &RequestedFileName) = 0;

      protected:
         FileCacheAction () {;}
         ~FileCacheAction () {;}

      private:
         FileCacheAction (const FileCacheAction &);
         FileCacheAction &operator= (const FileCacheAction &);
   };

   class FileCache {

      public:
         static FileCache *get_interface (RuntimeContext *context);

         virtual Boolean process_all_requests (const Float64 TimeOut = -1.0) = 0;

         virtual void add_file_request (
            const String &FileLocation,
            const String &FileName,
            FileCacheAction &action) = 0;

      protected:
         FileCache (RuntimeContext *context);
         ~FileCache ();

      private:
         FileCache ();
         FileCache (const FileCache &);
         FileCache &operator= (const FileCache &);

         RuntimeContext *__context;
   };
};


/*!

\brief Returns interface to the dmz::FileCache.
\param[in] context Pointer to runtime context.
\return Returns a pointer to the FileCache. Will return NULL if a FileCache has not
been created.

*/
inline dmz::FileCache *
dmz::FileCache::get_interface (RuntimeContext *context) {

   return (FileCache *)lookup_rtti_named_interface (FileCacheInterfaceName, context);
}


//! Constructor.
inline
dmz::FileCache::FileCache (RuntimeContext *context) : __context (context) {

   store_rtti_named_interface (FileCacheInterfaceName, __context, (void *)this);
}


//! Destructor.
inline
dmz::FileCache::~FileCache () {

   remove_rtti_named_interface (FileCacheInterfaceName, __context, (void *)this);
   __context = 0;
}

#endif // DMZ_FILE_CACHE_DOT_H
