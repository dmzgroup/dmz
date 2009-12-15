#include <dmzArchiveModule.h>
#include "dmzArchivePluginAutoSave.h"
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzSystemFile.h>
#include <dmzSystemStreamFile.h>
#include <dmzXMLInterpreterConfig.h>
#include <dmzXMLParser.h>
#include <dmzXMLUtil.h>

/*!

\class dmz::ArchivePluginAutoSave
\ingroup Archive
\brief Creates an auto save file whenever a new undo event is create.
\details Creates archives while the application is running. If the application
abnormally terminates and is restarted, it will restore from the auto saved archive.

*/

//! \cond
dmz::ArchivePluginAutoSave::ArchivePluginAutoSave (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      TimeSlice (Info, TimeSliceTypeSystemTime, TimeSliceModeRepeating, 5.0),
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
dmz::ArchivePluginAutoSave::update_plugin_state (
      const PluginStateEnum State,
      const UInt32 Level) {

   if (State == PluginStateStart) {

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
   else if (State == PluginStateShutdown) {

      if (is_valid_path (_saveFile)) { remove_file (_saveFile); }
   }
}


void
dmz::ArchivePluginAutoSave::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      if (!_archiveMod) { _archiveMod = ArchiveModule::cast (PluginPtr); }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_archiveMod && (_archiveMod == ArchiveModule::cast (PluginPtr))) {

         _archiveMod = 0;
      }
   }
}


// TimeSlice Interface
void
dmz::ArchivePluginAutoSave::update_time_slice (const Float64 TimeDelta) {

   if (_appStateDirty && _archiveMod && _archiveHandle && _saveFile) {

      _appStateDirty = False;

      FILE *file = open_file (_saveFile, "wb");

      if (file) {

         StreamFile out (file);

         Config config = _archiveMod->create_archive (_archiveHandle);

         write_xml_header (out);
         format_config_to_xml (config, out, XMLPrettyPrint);

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

      set_time_slice_interval (
         config_to_float64 ("save.rate", local, get_time_slice_interval ()));

      _archiveHandle = defs.create_named_handle (
         config_to_string ("archive.name", local, ArchiveDefaultName));
   }
   else {

      _log.warn << "No auto save file specified. Auto save disabled." << endl;
      stop_time_slice ();
   }

}
//! \endcond


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzArchivePluginAutoSave (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::ArchivePluginAutoSave (Info, local);
}

};
