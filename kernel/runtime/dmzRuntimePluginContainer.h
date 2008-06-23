#ifndef DMZ_RUNTIME_PLUGIN_CONTAINER_DOT_H
#define DMZ_RUNTIME_PLUGIN_CONTAINER_DOT_H

#include <dmzKernelExport.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzTypesBase.h>
#include <dmzTypesString.h>

namespace dmz {

   class Log;
   class Plugin;

   class DMZ_KERNEL_LINK_SYMBOL PluginIterator {

      public:
         PluginIterator ();
         ~PluginIterator ();

         struct State;
         State &state; //!< Internal state.

      private:
         PluginIterator (const PluginIterator &);
         PluginIterator &operator= (const PluginIterator &);
   };

   class DMZ_KERNEL_LINK_SYMBOL PluginContainer {

      public:
         PluginContainer (Log *log = 0);
         ~PluginContainer ();

         Boolean add_plugin (PluginInfo *Info, Plugin *plugin);
         PluginInfo *lookup_plugin_info (const Handle PluginHandle);
         Plugin *lookup_plugin (const Handle PluginHandle);
         Boolean remove_plugin (const Handle PluginHandle);

         void discover_plugins ();
         void init_plugins ();
         void start_plugins ();
         void stop_plugins ();
         void shutdown_plugins ();
         void remove_plugins ();
         void delete_plugins ();

         Plugin *get_first (PluginIterator &it) const;
         Plugin *get_next (PluginIterator &it) const;

         void discover_external_plugin (const Plugin *PluginPtr);
         void remove_external_plugin (const Plugin *PluginPtr);

      protected:
         struct State;
         State &_state; //!< Internal state.

      private:
         PluginContainer (const PluginContainer &Container);
         PluginContainer &operator= (const PluginContainer &Container);
   };
};

#endif // DMZ_RUNTIME_PLUGIN_CONTAINER_DOT_H
