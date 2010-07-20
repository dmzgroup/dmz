#ifndef DMZ_RUNTIME_RESOURCES_UPDATE_DOT_H
#define DMZ_RUNTIME_RESOURCES_UPDATE_DOT_H

#include <dmzKernelExport.h>
#include <dmzSystemFile.h>
#include <dmzTypesBase.h>
#include <dmzTypesString.h>

namespace dmz {

   class Config;
   class Log;
   class PluginInfo;
   class RuntimeContext;
   class RuntimeContextResources;
   class StringContainer;

   class DMZ_KERNEL_LINK_SYMBOL ResourcesUpdate {

      public:
         ResourcesUpdate (RuntimeContext *context);
         ResourcesUpdate (RuntimeContext *context, Log *log);
         ResourcesUpdate (const PluginInfo &Info);
         ResourcesUpdate (const PluginInfo &Info, Log *log);
         ~ResourcesUpdate ();

         Boolean update_resource (
            const String &ResourceName,
            const Config &Data);

         Boolean remove_resource (const String &ResourceName);

         Boolean update_resource_file (
            const String &ResourceName,
            const String &FileName);

         Boolean add_search_path (const String &Name, const String &Path);
         Boolean update_search_path (const String &Name, const StringContainer &Paths);

      protected:
         //! \cond
         RuntimeContextResources *_context;
         Log *_log;
         //! \endcond

      private:
         ResourcesUpdate ();
         ResourcesUpdate (const ResourcesUpdate &);
         ResourcesUpdate &operator= (const ResourcesUpdate &);
   };
};

#endif // DMZ_RUNTIME_RESOURCES_UPDATE_DOT_H
