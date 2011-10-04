#ifndef DMZ_ARCHIVE_MODULE_BASIC_DOT_H
#define DMZ_ARCHIVE_MODULE_BASIC_DOT_H

#include <dmzApplicationState.h>
#include <dmzArchiveModule.h>
#include <dmzArchiveObserver.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimeMessaging.h>
#include <dmzRuntimePlugin.h>
#include <dmzTypesHashTableHandleTemplate.h>

namespace dmz {


   class ArchiveModuleBasic :
         public Plugin,
         public MessageObserver,
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

         // Message Observer Interface
         virtual void receive_message (
            const Message &Type,
            const UInt32 MessageSendHandle,
            const Handle TargetObserverHandle,
            const Data *InData,
            Data *outData);

         // ArchiveModule Interface
         virtual Boolean register_archive_observer (
            const Handle ArchiveHandle,
            ArchiveObserver &observer);

         virtual Boolean release_archive_observer (
            const Handle ArchvieHandle,
            ArchiveObserver &observer);

         virtual Config create_archive (const Handle ArchiveHandle);
         virtual void process_archive (const Handle ArchiveHandle, Config &archive);

         virtual void set_database_name (String name) { _databaseName = name; }
         virtual String get_database_name () { return _databaseName; }

      protected:
         struct ArchiveStruct {

            const String Name;
            const Int32 Version;
            const Boolean ClearOldArchives;
            HashTableHandleTemplate<ArchiveObserver> table;

            ArchiveStruct (const String &TheName, const Int32 TheVersion, const Boolean Clear) :
                  Name (TheName),
                  Version (TheVersion),
                  ClearOldArchives (Clear) {;}

            ~ArchiveStruct () { table.clear (); }
         };

         void _init (Config &local);

         Log _log;
         Definitions _defs;
         Config _global;
         ApplicationState _appState;
         HashTableHandleTemplate<ArchiveStruct> _archiveTable;
         String _databaseName;
         Message _clearArchiveMessage;
         Boolean _clearArchive;
         //! \endcond

      private:
         ArchiveModuleBasic ();
         ArchiveModuleBasic (const ArchiveModuleBasic &);
         ArchiveModuleBasic &operator= (const ArchiveModuleBasic &);

   };
};

#endif // DMZ_ARCHIVE_MODULE_BASIC_DOT_H
