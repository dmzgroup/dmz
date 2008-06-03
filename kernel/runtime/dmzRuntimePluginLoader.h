#ifndef DMZ_RUNTIME_PLUGIN_LOADER_DOT_H
#define DMZ_RUNTIME_PLUGIN_LOADER_DOT_H

#include <dmzKernelExport.h>
#include <dmzRuntimePluginContainer.h>

namespace dmz {

   class Config;
   class Log;
   class Plugin;
   class RuntimeContext;

   class DMZ_KERNEL_LINK_SYMBOL PluginLoader {

      public:
         PluginLoader (RuntimeContext *context);
         ~PluginLoader ();

         Boolean load_plugins (
            Config &pluginList,
            Config &pluginInit,
            Config &global,
            PluginContainer &container,
            Log *log = 0);

      protected:
         struct State;
         State &_state; //!< Internal state.

      private:
         PluginLoader ();
         PluginLoader (const PluginLoader &);
         PluginLoader &operator= (const PluginLoader &);
   };
};

#endif // DMZ_RUNTIME_PLUGIN_LOADER_DOT_H
