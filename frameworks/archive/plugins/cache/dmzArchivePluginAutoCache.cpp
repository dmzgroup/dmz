#include <dmzArchiveModule.h>
#include "dmzArchivePluginAutoCache.h"
#include <dmzFoundationInterpreterXMLConfig.h>
#include <dmzFoundationParserXML.h>
#include <dmzFoundationXMLUtil.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzSystemFile.h>
#include <dmzSystemStreamFile.h>


dmz::ArchivePluginAutoCache::ArchivePluginAutoCache (const PluginInfo &Info, Config &local) :
      Plugin (Info),
      TimeSlice (Info, TimeSliceTypeSystemTime, TimeSliceModeRepeating, 5.0),
      MessageObserver (Info),
      _appState (Info),
      _archiveMod (0),
      _archiveHandle (0),
      _firstStart (True),
      _appStateDirty (False),
      _log (Info) {

   _init (local);
}


dmz::ArchivePluginAutoCache::~ArchivePluginAutoCache () {

}


// Plugin Interface
void
dmz::ArchivePluginAutoCache::update_plugin_state (
      const PluginStateEnum State,
      const UInt32 Level) {

   if (State == PluginStateInit) {

   }
   else if (State == PluginStateStart) {

      if (_firstStart && _saveFile && is_valid_path (_saveFile) && _archiveMod) {

         _log.info << "Restoring from auto cached archive: " << _saveFile << endl;
_log.warn << "Restoring from auto cached archive: " << _saveFile << endl;

         Config global ("global");
         ParserXML parser;
         InterpreterXMLConfig interpreter (global);
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
                  _log.error << "Unable to restore from auto cached archive: " << _saveFile
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
   else if (State == PluginStateStop) {

      _cache_archive ();
   }
   else if (State == PluginStateShutdown) {

   }
}


void
dmz::ArchivePluginAutoCache::discover_plugin (
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
dmz::ArchivePluginAutoCache::update_time_slice (const Float64 TimeDelta) {

   _cache_archive ();
}


// Message Observer Interface
void
dmz::ArchivePluginAutoCache::receive_message (
      const Message &Type,
      const UInt32 MessageSendHandle,
      const Handle TargetObserverHandle,
      const Data *InData,
      Data *outData) {

   if (Type == _updateArchiveMessage) { _appStateDirty = True; }
}


// ArchivePluginAutoCache Interface
void
dmz::ArchivePluginAutoCache::_cache_archive () {

   if (_appStateDirty && _archiveMod && _archiveHandle && _saveFile) {

      _appStateDirty = False;

      FILE *file = open_file (_saveFile, "wb");

      if (file) {

         StreamFile out (file);

         Config config = _archiveMod->create_archive (_archiveHandle);

         format_config_to_xml (config, out, ConfigPrettyPrint);

         close_file (file);
         _log.warn << "Cached archive: " << _saveFile << endl;
      }
   }
}


dmz::Boolean
dmz::ArchivePluginAutoCache::_init_cache_dir () {

   Boolean retVal (False);
   _cacheDir = get_home_directory ();

   if (is_valid_path (_cacheDir)) {

#if defined (_WIN32)
      _cacheDir << "/Local Settings/Application Data/";
#elif defined (__APPLE__) || defined (MACOSX)
      _cacheDir << "/Library/Caches/";
#else
      _cacheDir << "/.";
#endif

      _cacheDir = format_path (_cacheDir + "dmz/" + _appState.get_app_name ());

      // create cache directory if it doesn't exists already
      if (!is_valid_path (_cacheDir)) {

         create_directory (_cacheDir);
         _log.info << "Created application cache dir: " << _cacheDir << endl;
      }

      retVal = True;
   }

   return retVal;
}


void
dmz::ArchivePluginAutoCache::_init (Config &local) {

   RuntimeContext *context (get_plugin_runtime_context ());
   Definitions defs (context);

   _updateArchiveMessage = config_create_message (
      "archive-updated-message.name",
      local,
      "Archive_Updated_Message",
       context);

   subscribe_to_message (_updateArchiveMessage);

   if (_init_cache_dir ()) {

      _saveFile = config_to_string ("save.file", local, "archive.xml");

      if (_saveFile) {

         String path, file, ext;
         split_path_file_ext (_saveFile, path, file, ext);

         _saveFile = format_path (_cacheDir + "/" + file + ext);

         _log.info << "Auto cache archive to file: " << _saveFile << endl;

         set_time_slice_interval (
            config_to_float64 ("save.rate", local, get_time_slice_interval ()));

         _archiveHandle = defs.create_named_handle (
            config_to_string ("archive.name", local, ArchiveDefaultName));
      }
      else {

         _log.warn << "No auto save file specified. Auto cache archive disabled." << endl;
         stop_time_slice ();
      }
   }
   else {

      _log.warn << "Failed to crate cache directory. Auto cache archive disabled." << endl;
      stop_time_slice ();
   }
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzArchivePluginAutoCache (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::ArchivePluginAutoCache (Info, local);
}

};
