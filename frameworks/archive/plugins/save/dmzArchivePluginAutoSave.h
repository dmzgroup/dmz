#ifndef DMZ_ARCHIVE_PLUGIN_AUTO_SAVE_DOT_H
#define DMZ_ARCHIVE_PLUGIN_AUTO_SAVE_DOT_H

#include <dmzApplicationState.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimeTimeSlice.h>
#include <dmzRuntimeUndo.h>

namespace dmz {

   class ArchiveModule;

   class ArchivePluginAutoSave :
         public Plugin,
         public TimeSlice,
         public UndoObserver {

      //! \cond
      public:
         ArchivePluginAutoSave (const PluginInfo &Info, Config &local);
         ~ArchivePluginAutoSave ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level);

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // TimeSlice Interface
         virtual void update_time_slice (const Float64 TimeDelta);

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
         //! \endcond

      private:
         ArchivePluginAutoSave ();
         ArchivePluginAutoSave (const ArchivePluginAutoSave &);
         ArchivePluginAutoSave &operator= (const ArchivePluginAutoSave &);

   };
};

#endif // DMZ_ARCHIVE_PLUGIN_AUTO_SAVE_DOT_H
