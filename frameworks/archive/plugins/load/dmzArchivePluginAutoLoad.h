#ifndef DMZ_ARCHIVE_PLUGIN_AUTO_LOAD_DOT_H
#define DMZ_ARCHIVE_PLUGIN_AUTO_LOAD_DOT_H

#include <dmzRuntimeConfig.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>

namespace dmz {

   class ArchiveModule;

   class ArchivePluginAutoLoad :
         public Plugin {

      public:
         //! \cond
         ArchivePluginAutoLoad (const PluginInfo &Info, Config &local, Config &global);
         ~ArchivePluginAutoLoad ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level);

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

      protected:
         void _init (Config &local);

         ArchiveModule *_archiveMod;
         Config _global;
         Handle _archive;

         Log _log;
         //! \endcond
      private:
         ArchivePluginAutoLoad ();
         ArchivePluginAutoLoad (const ArchivePluginAutoLoad &);
         ArchivePluginAutoLoad &operator= (const ArchivePluginAutoLoad &);
   };
};

#endif // DMZ_ARCHIVE_PLUGIN_AUTO_LOAD_DOT_H
