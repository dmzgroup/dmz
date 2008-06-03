#include "dmzArchiveModuleBasic.h"
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
      _global (global),
      _appState (Info),
      _log (Info) {

   _init (local);
}


dmz::ArchiveModuleBasic::~ArchiveModuleBasic () {

   _archiveTable.empty ();
}


// Plugin Interface
void
dmz::ArchiveModuleBasic::discover_plugin (const Plugin *PluginPtr) {

   ArchiveObserver *obs (ArchiveObserver::cast (PluginPtr));

   if (obs) { obs->store_archive_module (get_plugin_name (), *this); }
}


void
dmz::ArchiveModuleBasic::start_plugin () {

}


void
dmz::ArchiveModuleBasic::stop_plugin () {

}


void
dmz::ArchiveModuleBasic::shutdown_plugin () {

}


void
dmz::ArchiveModuleBasic::remove_plugin (const Plugin *PluginPtr) {

   ArchiveObserver *obs (ArchiveObserver::cast (PluginPtr));

   if (obs) { obs->remove_archive_module (get_plugin_name (), *this); }
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

         as = new ArchiveStruct;

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

      HashTableHandleIterator it;

      ArchiveObserver *obs (as->table.get_first (it));

      while (obs) {

         obs->pre_create_archive (ArchiveHandle);
         obs = as->table.get_next (it);
      }

      obs = as->table.get_first (it);

      while (obs) {

         Config local (obs->get_archive_observer_name ());
         obs->create_archive (ArchiveHandle, local, result);
         if (!local.is_empty ()) { result.add_config (local); }
         obs = as->table.get_next (it);
      }

      obs = as->table.get_first (it);

      while (obs) {

         obs->post_create_archive (ArchiveHandle);
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

      HashTableHandleIterator it;

      ArchiveObserver *obs (as->table.get_first (it));

      while (obs) {

         obs->pre_process_archive (ArchiveHandle);
         obs = as->table.get_next (it);
      }

      obs = as->table.get_first (it);

      while (obs) {

         Config local;
         archive.lookup_all_config_merged (obs->get_archive_observer_name (), local);
         obs->process_archive (ArchiveHandle, local, archive);
         obs = as->table.get_next (it);
      }

      obs = as->table.get_first (it);

      while (obs) {

         obs->post_process_archive (ArchiveHandle);
         obs = as->table.get_next (it);
      }
   }

   _appState.pop_mode ();
}


void
dmz::ArchiveModuleBasic::_init (Config &local) {

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
