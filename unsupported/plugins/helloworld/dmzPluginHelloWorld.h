#ifndef DMZ_PLUGIN_HELLO_WORLD_DOT_H
#define DMZ_PLUGIN_HELLO_WORLD_DOT_H

#include <dmzModuleHelloWorld.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimeSync.h>
#include <dmzTypesBase.h>

namespace dmz {

   class PluginHelloWorld : public Plugin, public Sync {

      public:
         PluginHelloWorld (const PluginInfo &Info);
         virtual ~PluginHelloWorld ();

         virtual void discover_plugin (const Plugin *PluginPtr);
         virtual void start_plugin ();
         virtual void stop_plugin ();
         virtual void shutdown_plugin ();
         virtual void remove_plugin (const Plugin *PluginPtr);

         virtual void update_sync (const Float64 TimeDelta);
      protected:
         Log _log;
         ModuleHelloWorld *_mod;
   };
};

#endif // DMZ_PLUGIN_HELLO_WORLD_DOT_H
