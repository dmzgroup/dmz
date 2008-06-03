#include <dmzArchiveModule.h>
#include "dmzArchivePluginAutoSave.h"
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigRead.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzSystemFile.h>
#include <dmzSystemStreamFile.h>
#include <dmzXMLInterpreterConfig.h>
#include <dmzXMLParser.h>
#include <dmzXMLUtil.h>

dmz::ArchivePluginAutoSave::ArchivePluginAutoSave (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      Sync (Info, SyncTypeSystemTime, SyncModeRepeating, 5.0),
      UndoObserver (Info),
      _appState (Info),
      _archiveMod (0),
      _archiveHandle (0),
      _firstStart (True),
      _appStateDirty (False),
      _log (Info) {

   _init (local);
}


dmz::ArchivePluginAutoSave::~ArchivePluginAutoSave () {

}


// Plugin Interface
void
dmz::ArchivePluginAutoSave::discover_plugin (const Plugin *PluginPtr) {

   if (!_archiveMod) { _archiveMod = ArchiveModule::cast (PluginPtr); }
}


void
dmz::ArchivePluginAutoSave::start_plugin () {

   if (_firstStart && _saveFile && is_valid_path (_saveFile) && _archiveMod) {

      _log.info << "Restoring from auto save archive: " << _saveFile << endl;

      Config global ("global");
      XMLParser parser;
      XMLInterpreterConfig interpreter (global);
      parser.set_interpreter (&interpreter);

      FILE *file = open_file (_saveFile, "rb");

      if (file) {

         Boolean error (False);
         String buffer;

         while (read_file (file, 1024, buffer) && !error) {

            const Int32 Length = buffer.get_length ();
            const char *cbuf = buffer.get_buffer ();

            if (!parser.parse_buffer (cbuf, Length, Length < 1024)) {

               error = True;
               _log.error << "Unable to restore from auto save archive: " << _saveFile
                  << " : " << parser.get_error ();
            }
         }

         close_file (file);

         Config data;

         if (!error && global.lookup_all_config_merged ("dmz", data)) {

            _archiveMod->process_archive (_archiveHandle, data);
         }
      }
   }

   _firstStart = False;
}


void
dmz::ArchivePluginAutoSave::stop_plugin () {

}


void
dmz::ArchivePluginAutoSave::shutdown_plugin () {

   if (is_valid_path (_saveFile)) { remove_file (_saveFile); }
}


void
dmz::ArchivePluginAutoSave::remove_plugin (const Plugin *PluginPtr) {

   if (_archiveMod && (_archiveMod == ArchiveModule::cast (PluginPtr))) {

      _archiveMod = 0;
   }
}


// Sync Interface
void
dmz::ArchivePluginAutoSave::update_sync (const Float64 TimeDelta) {

   if (_appStateDirty && _archiveMod && _archiveHandle && _saveFile) {

      _appStateDirty = False;

      FILE *file = open_file (_saveFile, "wb");

      if (file) {

         StreamFile out (file);

         Config config = _archiveMod->create_archive (_archiveHandle);

         write_xml_header (out);
         format_config_to_xml (config, out);

         close_file (file);
      }
   }
}


// UndoObserver Interface
void
dmz::ArchivePluginAutoSave::update_recording_state (
      const UndoRecordingStateEnum RecordingState,
      const UndoRecordingTypeEnum RecordingType,
      const UndoTypeEnum UndoType) {

   if (RecordingState == UndoRecordingStateStart) { _appStateDirty = True; }
}


void
dmz::ArchivePluginAutoSave::_init (Config &local) {

   Definitions defs (get_plugin_runtime_context ());

   _saveFile = config_to_string ("save.file", local, _appState.get_autosave_file ());

   if (_saveFile) {

      set_sync_interval (config_to_float64 ("save.rate", local, get_sync_interval ()));

      _archiveHandle = defs.create_named_handle (
         config_to_string ("archive.name", local, ArchiveDefaultName));
   }
   else {

      _log.warn << "No auto save file specified. Auto save disabled." << endl;
      stop_sync ();
   }

}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzArchivePluginAutoSave (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::ArchivePluginAutoSave (Info, local);
}

};
