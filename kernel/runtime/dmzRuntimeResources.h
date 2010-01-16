#ifndef DMZ_RUNTIME_RESOURCES_DOT_H
#define DMZ_RUNTIME_RESOURCES_DOT_H

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

   class DMZ_KERNEL_LINK_SYMBOL Resources {

      public:
         Resources (RuntimeContext *context);
         Resources (RuntimeContext *context, Log *log);
         Resources (const PluginInfo &Info);
         Resources (const PluginInfo &Info, Log *log);
         ~Resources ();

         String find_file (const String &ResourceName) const;

         Boolean store_resource_config (
            const String &ResourceName,
            const Config &Resource);

         Boolean lookup_resource_config (
            const String &ResourceName,
            Config &resource) const;
 
         Boolean remove_resource_config (const String &ResourceName);

         Boolean store_search_path (
            const String &SearchPathName,
            const StringContainer &SearchPath);

         Boolean lookup_search_path (
            const String &SearchPathName,
            StringContainer &searchPath) const;

         Boolean remove_search_path (const String &SearchPathName);

      protected:
         //! \cond
         RuntimeContextResources *_context;
         Log *_log;
         //! \endcond

      private:
         Resources ();
         Resources (const Resources &);
         Resources &operator= (const Resources &);
   };
};

#endif // DMZ_RUNTIME_RESOURCES_DOT_H
