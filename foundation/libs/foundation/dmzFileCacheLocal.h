#ifndef DMZ_FILE_CACHE_LOCAL_DOT_H
#define DMZ_FILE_CACHE_LOCAL_DOT_H

#include <dmzFileCache.h>
#include <dmzFoundationExport.h>

namespace dmz {

   class Config;

   class DMZ_FOUNDATION_LINK_SYMBOL FileCacheLocal : public FileCache {

      public:
         FileCacheLocal (RuntimeContext *context);
         ~FileCacheLocal ();

         void configure (Config &data);
         void add_path (const String &Path);

         virtual Boolean process_all_requests (const Float64 TimeOut);

         virtual void add_file_request (
            const String &FileLocation,
            const String &FileName,
            FileCacheAction &action);

      protected:
         struct State;
         State &_state; //!< Internal state.

      private:
         FileCacheLocal ();
         FileCacheLocal (const FileCacheLocal &);
         FileCacheLocal &operator= (const FileCacheLocal &);
   };
};

#endif // DMZ_FILE_CACHE_LOCAL_DOT_H
