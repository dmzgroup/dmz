#ifndef DMZ_RUNTIME_RESOURCES_DOT_H
#define DMZ_RUNTIME_RESOURCES_DOT_H

#include <dmzKernelExport.h>
#include <dmzTypesBase.h>
#include <dmzTypesString.h>

namespace dmz {

   class Config;
   class PathContainer;
   class PluginInfo;
   class RuntimeContext;
   class RuntimeContextResources;

   class DMZ_KERNEL_LINK_SYMBOL Resources {

      public:
         Resources (RuntimeContext *context);
         Resources (const PluginInfo &Info);
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
            const PathContainer &SearchPath);

         Boolean lookup_search_path (
            const String &SearchPathName,
            PathContainer &searchPath) const;

         Boolean remove_search_path (const String &SearchPathName);

      protected:
         RuntimeContextResources *_context;

      private:
         Resources ();
         Resources (const Resources &);
         Resources &operator= (const Resources &);
   };
};

#endif // DMZ_RUNTIME_RESOURCES_DOT_H
