#ifndef DMZ_RUNTIME_PLUGIN_CONTAINER_DOT_H
#define DMZ_RUNTIME_PLUGIN_CONTAINER_DOT_H

#include <dmzKernelExport.h>
#include <dmzRuntimeIterator.h>
#include <dmzTypesBase.h>
#include <dmzTypesString.h>

namespace dmz {

   class HandleContainer;
   class Log;
   class Plugin;
   class PluginInfo;
   class RuntimeContext;

   class DMZ_KERNEL_LINK_SYMBOL PluginContainer {

      public:
         PluginContainer (RuntimeContext *context, Log *log = 0);
         ~PluginContainer ();

         Handle get_handle () const;
         void get_plugin_list (HandleContainer &container);
         Boolean add_plugin (PluginInfo *Info, Plugin *plugin);
         PluginInfo *lookup_plugin_info (const Handle PluginHandle);
         Plugin *lookup_plugin (const Handle PluginHandle);
         Boolean remove_plugin (const Handle PluginHandle);
         Boolean release_plugin (const Handle PluginHandle);

         void discover_plugins ();
         void init_plugins ();
         void start_plugins ();
         void stop_plugins ();
         void shutdown_plugins ();
         void remove_plugins ();
         void delete_plugins ();

         Plugin *get_first (RuntimeIterator &it) const;
         Plugin *get_next (RuntimeIterator &it) const;

         void discover_external_plugin (const Plugin *PluginPtr);
         void remove_external_plugin (const Plugin *PluginPtr);

      protected:
         //! \cond
         struct State;
         State &_state;
         //! \endcond

      private:
         PluginContainer (const PluginContainer &Container);
         PluginContainer &operator= (const PluginContainer &Container);
   };
};

#endif // DMZ_RUNTIME_PLUGIN_CONTAINER_DOT_H
