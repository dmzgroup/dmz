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
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level) {;}

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr) {;}
         
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
