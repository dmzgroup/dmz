#ifndef DMZ_RUNTIME_PLUGIN_LOADER_DOT_H
#define DMZ_RUNTIME_PLUGIN_LOADER_DOT_H

#include <dmzKernelExport.h>
#include <dmzRuntimePluginContainer.h>

namespace dmz {

   class Config;
   class Log;
   class Plugin;
   class RuntimeContext;

   DMZ_KERNEL_LINK_SYMBOL Boolean
   load_plugins (
      RuntimeContext *context,
      Config &pluginList,
      Config &pluginInit,
      Config &global,
      PluginContainer &container,
      Log *log = 0);

};

#endif // DMZ_RUNTIME_PLUGIN_LOADER_DOT_H
