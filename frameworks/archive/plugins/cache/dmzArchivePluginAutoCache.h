#ifndef DMZ_ARCHIVE_PLUGIN_AUTO_CACHE_DOT_H
#define DMZ_ARCHIVE_PLUGIN_AUTO_CACHE_DOT_H

#include <dmzApplicationState.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimeMessaging.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimeTimeSlice.h>


namespace dmz {

   class ArchiveModule;

   class ArchivePluginAutoCache :
         public Plugin,
         public TimeSlice,
         public MessageObserver {

      public:
         ArchivePluginAutoCache (const PluginInfo &Info, Config &local);
         ~ArchivePluginAutoCache ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level);

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // TimeSlice Interface
         virtual void update_time_slice (const Float64 TimeDelta);

         // Message Observer Interface
         virtual void receive_message (
            const Message &Type,
            const UInt32 MessageSendHandle,
            const Handle TargetObserverHandle,
            const Data *InData,
            Data *outData);

      protected:
         void _cache_archive ();
         Boolean _init_cache_dir ();
         void _load_cache ();
         void _init (Config &local);

         ApplicationState _appState;
         ArchiveModule *_archiveMod;
         Handle _archiveHandle;
         Handle _versionHandle;
         UInt32 _version;
         UInt32 _versionDelta;
         String _cacheDir;
         String _saveFile;
         String _dbName;
         Boolean _autoRestore;
         Boolean _appStateDirty;
         Boolean _haveLoadedCache;
         Message _updateArchiveMessage;
         Handle _dbHandle;
         Message _loadMessage;
         Message _dbMessage;
         Message _skippedMessage;
         Log _log;

      private:
         ArchivePluginAutoCache ();
         ArchivePluginAutoCache (const ArchivePluginAutoCache &);
         ArchivePluginAutoCache &operator= (const ArchivePluginAutoCache &);
   };
};

#endif // DMZ_ARCHIVE_PLUGIN_AUTO_CACHE_DOT_H
