#include "dmzArchiveModuleBasic.h"
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeConfigToNamedHandle.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>

/*!

\class dmz::ArchiveModuleBasic
\ingroup Archive
\brief Basic ArchiveModule implementation.
\details This provides a basic implementation of the ArchiveModule. At this time the
ArchiveModuleBasic has no configuration parameters.
\sa ArchiveModule

*/

//! \cond
dmz::ArchiveModuleBasic::ArchiveModuleBasic (
      const PluginInfo &Info,
      Config &local,
      Config &global) :
      Plugin (Info),
      ArchiveModule (Info),
      _log (Info),
      _defs (Info, &_log),
      _global (global),
      _appState (Info) {

   _init (local);
}


dmz::ArchiveModuleBasic::~ArchiveModuleBasic () {

   _archiveTable.empty ();
}


// Plugin Interface
void
dmz::ArchiveModuleBasic::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   ArchiveObserver *obs (ArchiveObserver::cast (PluginPtr));

   if (Mode == PluginDiscoverAdd) {

      if (obs) { obs->store_archive_module (get_plugin_name (), *this); }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (obs) { obs->remove_archive_module (get_plugin_name (), *this); }
   }
}


// ArchiveModule Interface
dmz::Boolean
dmz::ArchiveModuleBasic::register_archive_observer (
      const Handle ArchiveHandle,
      ArchiveObserver &observer) {

   Boolean result (False);

   if (ArchiveHandle) {

      ArchiveStruct *as (_archiveTable.lookup (ArchiveHandle));

      if (!as) {

         as = new ArchiveStruct (_defs.lookup_named_handle_name (ArchiveHandle), -1);

         if (as && !_archiveTable.store (ArchiveHandle, as)) { delete as; as = 0; }
      }

      if (as) {

         result = as->table.store (observer.get_archive_observer_handle (), &observer);
      }
   }

   return result;
}


dmz::Boolean
dmz::ArchiveModuleBasic::release_archive_observer (
      const Handle ArchiveHandle,
      ArchiveObserver &observer) {

   Boolean result (False);

   ArchiveStruct *as (_archiveTable.lookup (ArchiveHandle));

   if (as) {

      result = (as->table.remove (observer.get_archive_observer_handle ()) != 0);
   }

   return result;
}


dmz::Config
dmz::ArchiveModuleBasic::create_archive (const Handle ArchiveHandle) {

   _appState.push_mode (ApplicationModeSaving);

   Config result ("dmz");

   ArchiveStruct *as (_archiveTable.lookup (ArchiveHandle));

   if (as) {

      if (as->Version >= 0) {

         Config version ("archive-version");

         if (as->Name != ArchiveDefaultName) {

            version.store_attribute ("name", as->Name);
         }

         version.store_attribute ("version", String::number (as->Version));
         result.add_config (version);
      }

      HashTableHandleIterator it;

      ArchiveObserver *obs (as->table.get_first (it));

      while (obs) {

         obs->pre_create_archive (ArchiveHandle, as->Version);
         obs = as->table.get_next (it);
      }

      obs = as->table.get_first (it);

      while (obs) {

         StringContainer sc = obs->get_archive_scope (ArchiveHandle);
         String scopeName;
         if (!sc.get_first (scopeName)) { scopeName = obs->get_archive_observer_name (); }
         Config local (scopeName);
         obs->create_archive (ArchiveHandle, as->Version, local, result);
         if (!local.is_empty ()) { result.add_config (local); }
         obs = as->table.get_next (it);
      }

      obs = as->table.get_first (it);

      while (obs) {

         obs->post_create_archive (ArchiveHandle, as->Version);
         obs = as->table.get_next (it);
      }
   }
   else { result.set_config_context (0); }

   _appState.pop_mode ();

   return result;
}


void
dmz::ArchiveModuleBasic::process_archive (const Handle ArchiveHandle, Config &archive) {

   _appState.push_mode (ApplicationModeLoading);

   ArchiveStruct *as (_archiveTable.lookup (ArchiveHandle));

   if (as) {

      const Int32 Version = config_to_int32 ("archive-version.version", archive, -1);

      if (Version > as->Version) {

         _log.warn << "Archive version number: " << Version
            << " is greater than the supported version number: " << as->Version << endl;
      }

      HashTableHandleIterator it;

      ArchiveObserver *obs (as->table.get_first (it));

      while (obs) {

         obs->pre_process_archive (ArchiveHandle, Version);
         obs = as->table.get_next (it);
      }

      obs = as->table.get_first (it);

      while (obs) {

         Config local;
         StringContainer sc = obs->get_archive_scope (ArchiveHandle);
         String scopeName;
         Boolean found = sc.get_first (scopeName);

         while (found && !local) {

            archive.lookup_all_config_merged (scopeName, local);
            found = sc.get_next (scopeName);
         }

         obs->process_archive (ArchiveHandle, Version, local, archive);
         obs = as->table.get_next (it);
      }

      obs = as->table.get_first (it);

      while (obs) {

         obs->post_process_archive (ArchiveHandle, Version);
         obs = as->table.get_next (it);
      }
   }

   _appState.pop_mode ();
}


void
dmz::ArchiveModuleBasic::_init (Config &local) {

   RuntimeContext *context (0);

   Config list;

   if (local.lookup_all_config ("archive", list)) {

      ConfigIterator it;
      Config archive;

      while (list.get_next_config (it, archive)) {

         const String ArchiveName =
            config_to_string ("name", archive, ArchiveDefaultName);

         const Int32 Version = config_to_int32 ("version", archive, -1);

         if (ArchiveName) {

            ArchiveStruct *as = new ArchiveStruct (ArchiveName, Version);

            if (as &&
                  !_archiveTable.store (_defs.create_named_handle (ArchiveName), as)) {

               delete as; as = 0;
            }
         }
      }
   }
}
//! \endcond

extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzArchiveModuleBasic (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::ArchiveModuleBasic (Info, local, global);
}

};
