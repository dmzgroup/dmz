#include <dmzArchiveModule.h>
#include "dmzArchivePluginQuickSave.h"
#include <dmzInputEventKey.h>
#include <dmzInputEventMasks.h>
#include <dmzRuntimeConfigRead.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzSystemFile.h>
#include <dmzSystemStreamFile.h>
#include <dmzXMLUtil.h>

/*!

\class dmz::ArchivePluginQuickSave
\ingroup Archive
\brief Creates archive and saves to XML file.
\details When the specified key is pressed, an archive is created and saved
to the specified file. The default key is 'T'. The default file is called "dump.xml".
If an archive group is not specified, the default archive group is used.
\code
<dmz>
<dmzArchivePluginQuickSave>
   <file name="File Name"
   <archive name="Archive Group Name"/>
   <key value="Key Value"/>
</dmzArchivePluginQuickSave>
</dmz>
\endcode
*/

//! \cond
dmz::ArchivePluginQuickSave::ArchivePluginQuickSave (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      InputObserverUtil (Info, local),
      _dumpFile ("dump.xml"),
      _archiveMod (0),
      _defaultHandle (0),
      _key (0),
      _log (Info) {

   _init (local);
}


dmz::ArchivePluginQuickSave::~ArchivePluginQuickSave () {

}


// Plugin Interface
void
dmz::ArchivePluginQuickSave::discover_plugin (
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


// Input Observer Interface
void
dmz::ArchivePluginQuickSave::receive_key_event (
      const Handle Channel,
      const InputEventKey &Value) {

   if (_archiveMod && (Value.get_key () == _key) && Value.get_key_state ()) {

      FILE *file = open_file (_dumpFile, "wb");

      if (file) {

         StreamFile out (file);

         Config config = _archiveMod->create_archive (_defaultHandle);

         write_xml_header (out);
         format_config_to_xml (config, out);

         _log.info << "Created object dump file: " << _dumpFile << endl;

         close_file (file);
      }
   }
}


void
dmz::ArchivePluginQuickSave::_init (Config &local) {

   Definitions defs (get_plugin_runtime_context ());

   _defaultHandle = defs.create_named_handle (
      config_to_string ("archive.name", local, ArchiveDefaultName));

   _key = config_to_key_value ("key.value", local,  UInt32 ('T'), &_log);

   _dumpFile = config_to_string ("file.name", local, _dumpFile);

   init_input_channels (local, InputEventKeyMask, &_log);
}
//! \endcond


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzArchivePluginQuickSave (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::ArchivePluginQuickSave (Info, local);
}

};
