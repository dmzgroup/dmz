#include "dmzArchivePluginUndo.h"
#include <dmzRuntimeMessaging.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeConfigToNamedHandle.h>
#include <dmzRuntimeConfigWrite.h>
#include <dmzRuntimeData.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>

/*!

\class dmz::ArchivePluginUndo
\ingroup Archive
\brief Creates an archive of the runtime undo stack.

*/

//! \cond
dmz::ArchivePluginUndo::ArchivePluginUndo (const PluginInfo &Info, Config &local) :
      Plugin (Info),
      ArchiveObserverUtil (Info, local),
      _defs (Info),
      _undo (Info),
      _log (Info),
      _loadLimit (-1),
      _saveLimit (-1) {

   _init (local);
}


dmz::ArchivePluginUndo::~ArchivePluginUndo () {

}


// ArchiveObserver Interface.
void
dmz::ArchivePluginUndo::create_archive (
      const Handle ArchiveHandle,
      const Int32 Version,
      Config &local,
      Config &global) {

   _local = local;
   _undo.dump (UndoTypeUndo, *this);
   _step.set_config_context (0);
   _local.set_config_context (0);
}


void
dmz::ArchivePluginUndo::process_archive (
      const Handle ArchiveHandle,
      const Int32 Version,
      Config &local,
      Config &global) {

   Config undoList;

   if (local.lookup_all_config ("undo", undoList)) {

      RuntimeContext *context (get_plugin_runtime_context ());

      ConfigIterator it;
      Config undoStep;

      Boolean found (undoList.get_last_config (it, undoStep));

      while (found) {

         const String Name (config_to_string ("name", undoStep, "Unknown Action"));

         Config actionList;

         if (undoStep.lookup_all_config ("action", actionList)) {

            const Handle UndoHandle (_undo.start_record (Name));

            ConfigIterator actionIt;
            Config action;

            while (actionList.get_next_config (actionIt, action)) {

               const Message Type (
                  config_create_message ("message", action, "", context, &_log));

               const Handle Target (
                  config_to_named_handle ("target", action, "", context));

               Data value;
               const Boolean DataCreated (
                  config_to_data ("data", action, context, value, &_log));

               if (Type) {

                  _undo.store_action (Type, Target, DataCreated ? &value : 0);
               }
            }

            _undo.stop_record (UndoHandle);
         }

         found = undoList.get_prev_config (it, undoStep);
      }
   }
}


// UndoDump Interface.
void
dmz::ArchivePluginUndo::start_record (const Handle RecordHandle, const String &Name) {

   Config undoStep ("undo");
   undoStep.store_attribute ("name", Name);
   _local.add_config (undoStep);
   _step = undoStep;
}


void
dmz::ArchivePluginUndo::store_action (
      const Message &Type,
      const Handle Target,
      const Data *Value) {

   if (Type) {

      Config action ("action");

      action.store_attribute ("message", Type.get_name ());

      if (Target) {

         action.store_attribute ("target", _defs.lookup_named_handle_name (Target));
      }

      if (Value) {

         action.add_config (
            data_to_config (*Value, get_plugin_runtime_context (), &_log));
      }

      _step.add_config (action);
   }
}


void
dmz::ArchivePluginUndo::_init (Config &local) {

   init_archive (local);
   _loadLimit = config_to_int32 ("load-limit.value", local, _loadLimit);
   _saveLimit = config_to_int32 ("save-limit.value", local, _saveLimit);
}
//! \endcond


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzArchivePluginUndo (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::ArchivePluginUndo (Info, local);
}

};
