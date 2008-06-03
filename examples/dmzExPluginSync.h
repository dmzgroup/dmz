#ifndef DMZ_EX_PLUGIN_SYNC_DOT_H
#define DMZ_EX_PLUGIN_SYNC_DOT_H

#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimeSync.h>


namespace dmz {

   class ExPluginSync :
         public Plugin,
         public Sync {

      public:
         ExPluginSync (const PluginInfo &Info, Config &local);
         ~ExPluginSync ();

         // Plugin Interface
         virtual void discover_plugin (const Plugin *PluginPtr);
         virtual void start_plugin ();
         virtual void stop_plugin ();
         virtual void shutdown_plugin ();
         virtual void remove_plugin (const Plugin *PluginPtr);
         
         // Sync Interface
         virtual void update_sync (const Float64 DeltaTime);
         
      protected:
         void _init (Config &local);

         Log _log;
         
      private:
         ExPluginSync ();
         ExPluginSync (const ExPluginSync &);
         ExPluginSync &operator= (const ExPluginSync &);

   };
};

#endif // DMZ_EX_PLUGIN_SYNC_DOT_H
