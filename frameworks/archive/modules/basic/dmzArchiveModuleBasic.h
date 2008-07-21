#ifndef DMZ_ARCHIVE_MODULE_BASIC_DOT_H
#define DMZ_ARCHIVE_MODULE_BASIC_DOT_H

#include <dmzApplicationState.h>
#include <dmzArchiveModule.h>
#include <dmzArchiveObserver.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzTypesHashTableHandleTemplate.h>

namespace dmz {

   class ArchiveModuleBasic :
         public Plugin,
         public ArchiveModule {

      public:
         //! \cond
         ArchiveModuleBasic (const PluginInfo &Info, Config &local, Config &global);
         ~ArchiveModuleBasic ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level) {;}

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // ArchiveModule Interface
         virtual Boolean register_archive_observer (
            const Handle ArchiveHandle,
            ArchiveObserver &observer);

         virtual Boolean release_archive_observer (
            const Handle ArchvieHandle,
            ArchiveObserver &observer);

         virtual Config create_archive (const Handle ArchiveHandle);
         virtual void process_archive (const Handle ArchiveHandle, Config &archive);

      protected:
         struct ArchiveStruct {

            HashTableHandleTemplate<ArchiveObserver> table;

            ~ArchiveStruct () { table.clear (); }
         };

         void _init (Config &local);

         Config _global;
         ApplicationStateWrapper _appState;
         HashTableHandleTemplate<ArchiveStruct> _archiveTable;
         Log _log;
         //! \endcond

      private:
         ArchiveModuleBasic ();
         ArchiveModuleBasic (const ArchiveModuleBasic &);
         ArchiveModuleBasic &operator= (const ArchiveModuleBasic &);

   };
};

#endif // DMZ_ARCHIVE_MODULE_BASIC_DOT_H
