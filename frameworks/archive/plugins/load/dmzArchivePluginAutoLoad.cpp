#include <dmzArchiveModule.h>
#include "dmzArchivePluginAutoLoad.h"
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigRead.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>

/*!

\class dmz::ArchivePluginAutoLoad
\ingroup Archive
\brief Auto loads an archive.
\details The archive comes from the Config passed to Plugins at creation time.
The archive is processed in the Plugin::start_plugin() callback.

*/

//! \cond
dmz::ArchivePluginAutoLoad::ArchivePluginAutoLoad (
      const PluginInfo &Info,
      Config &local,
      Config &global) :
      Plugin (Info),
      _archiveMod (0),
      _global (global),
      _archive (0),
      _log (Info) {

   _init (local);
}


dmz::ArchivePluginAutoLoad::~ArchivePluginAutoLoad () {

}


// Plugin Interface
void
dmz::ArchivePluginAutoLoad::discover_plugin (const Plugin *PluginPtr) {

   if (!_archiveMod) { _archiveMod = ArchiveModule::cast (PluginPtr); }
}


void
dmz::ArchivePluginAutoLoad::start_plugin () {

   if (_archiveMod && _archive) {

      Config data;
      _global.lookup_all_config_merged ("dmz", data);
      _archiveMod->process_archive (_archive, data);
   }
}


void
dmz::ArchivePluginAutoLoad::remove_plugin (const Plugin *PluginPtr) {

   if (_archiveMod && (_archiveMod == ArchiveModule::cast (PluginPtr))) {

      _archiveMod = 0;
   }
}


void
dmz::ArchivePluginAutoLoad::_init (Config &local) {

   Definitions defs (get_plugin_runtime_context ());

   _archive = defs.create_named_handle (
      config_to_string ("archive.name", local, ArchiveDefaultName));
}
//! \endcond


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzArchivePluginAutoLoad (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::ArchivePluginAutoLoad (Info, local, global);
}

};
