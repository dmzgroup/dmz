#ifndef DMZ_ARCHIVE_PLUGIN_AUTO_SAVE_DOT_H
#define DMZ_ARCHIVE_PLUGIN_AUTO_SAVE_DOT_H

#include <dmzApplicationState.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimeSync.h>
#include <dmzRuntimeUndo.h>

namespace dmz {

   class ArchiveModule;

   class ArchivePluginAutoSave :
         public Plugin,
         public Sync,
         public UndoObserver {

      public:
         ArchivePluginAutoSave (const PluginInfo &Info, Config &local);
         ~ArchivePluginAutoSave ();

         // Plugin Interface
         virtual void discover_plugin (const Plugin *PluginPtr);
         virtual void start_plugin ();
         virtual void stop_plugin ();
         virtual void shutdown_plugin ();
         virtual void remove_plugin (const Plugin *PluginPtr);

         // Sync Interface
         virtual void update_sync (const Float64 TimeDelta);

         // UndoObserver Interface
         virtual void update_recording_state (
            const UndoRecordingStateEnum RecordingState,
            const UndoRecordingTypeEnum RecordingType,
            const UndoTypeEnum UndoType);

         virtual void update_current_undo_names (
            const String *NextUndoName,
            const String *NextRedoName) {;}


      protected:
         void _init (Config &local);

         ApplicationStateWrapper _appState;
         ArchiveModule *_archiveMod;
         Handle _archiveHandle;
         String _saveFile;
         Boolean _firstStart;
         Boolean _appStateDirty;

         Log _log;

      private:
         ArchivePluginAutoSave ();
         ArchivePluginAutoSave (const ArchivePluginAutoSave &);
         ArchivePluginAutoSave &operator= (const ArchivePluginAutoSave &);

   };
};

#endif // DMZ_ARCHIVE_PLUGIN_AUTO_SAVE_DOT_H
