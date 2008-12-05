#include <dmzArchiveModule.h>
#include <dmzArchiveObserverUtil.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimeLog.h>
#include <dmzTypesHashTableStringTemplate.h>
#include <dmzTypesHandleContainer.h>

/*!

\class dmz::ArchiveObserverUtil
\ingroup Archive
\brief Archive observer utility class.

*/

struct dmz::ArchiveObserverUtil::State {

   const String ArchiveModuleName;
   Log log;
   Definitions defs;
   ArchiveModule *module;
   HashTableStringTemplate<Handle> handleTable;
   HandleContainer validHandles;

   State (const PluginInfo &Info, const Config &Init) :
         ArchiveModuleName (config_to_string ("module.archive.name", Init)),
         log (Info.get_name () + ".ArchiveObserverUtil", Info.get_context ()),
         defs (Info, &log),
         module (0) {

      if (ArchiveModuleName) {

         log.info << "Looking for Archive Module: " << ArchiveModuleName << endl;
      }
   }

   ~State () { handleTable.empty (); }
};


/*!

\brief Constructor
\details The ArchiveObserverUtil uses the Config archive passed in to specify which
ArchiveModule to register with. This is done by giving the name of the ArchiveModule. The
format is as follows:
\code
<module>
   <archive name="Archive Module Name"/>
</module>
\endcode
If the ArchiveModule name is not specified, the observer will register with the first
ArchiveModule that discovers it.
\param[in] Info PluginInfo used to initialize the Plugin that is derived from this class.
\param[in] Init Config used to initialize the class. This is most often the Config archive
passed in as local to the Plugin.

*/
dmz::ArchiveObserverUtil::ArchiveObserverUtil (
      const PluginInfo &Info,
      const Config &Init) :
      ArchiveObserver (Info),
      __state (*(new State (Info, Init))) {;}


dmz::ArchiveObserverUtil::~ArchiveObserverUtil () {

   if (__state.module) { remove_archive_module ("", *(__state.module)); }
   delete &__state;
}


dmz::Boolean
dmz::ArchiveObserverUtil::is_active_archive_handle (const Handle ArchiveHandle) {

   return __state.validHandles.contains (ArchiveHandle);
}


void
dmz::ArchiveObserverUtil::init_archive (const Config &Init, Log *log) {

   Config list;

   if (Init.lookup_all_config ("archive", list)) {

      ConfigIterator it;
      Config archive;

      while (list.get_next_config (it, archive)) {

         activate_archive (config_to_string ("name", archive, ArchiveDefaultName));
      }
   }
   else { activate_default_archive (); }
}


/*!

\brief Joins default archive group.
\return Returns the Handle of the default archive group.

*/
dmz::Handle
dmz::ArchiveObserverUtil::activate_default_archive () {

   return activate_archive (ArchiveDefaultName);
}


/*!

\brief Joins an archive group.
\param[in] ArchiveName String containing the name of the archive group.
\return Returns the Handle of the archive group.

*/
dmz::Handle
dmz::ArchiveObserverUtil::activate_archive (const String &ArchiveName) {

   Handle result (__state.defs.create_named_handle (ArchiveName));

   if (result) {

      __state.validHandles.add_handle (result);

      Handle *ptr (new Handle (result));

      if (ptr) {

         if (__state.handleTable.store (ArchiveName, ptr)) {

            if (__state.module) {

               __state.module->register_archive_observer (*ptr, *this);
            }
         }
         else { delete ptr; ptr = 0; }
      }
   }

   return result;
}


/*!

\brief Leaves the default archive group.

*/
void
dmz::ArchiveObserverUtil::deactivate_default_archive () {

   deactivate_archive (ArchiveDefaultName);
}


/*!

\brief Leave an archive group.
\param[in] ArchiveName String containing the name of the archive group.

*/
void
dmz::ArchiveObserverUtil::deactivate_archive (const String &ArchiveName) {

   Handle *ptr (__state.handleTable.remove (ArchiveName));

   if (ptr) {

      __state.validHandles.remove_handle (*ptr);

      if (__state.module) { __state.module->release_archive_observer (*ptr, *this); }

      delete ptr; ptr = 0;
   }
}


/*!

\brief Leave an archive group.
\param[in] ArchiveHandle Handle of the archive group.

*/
void
dmz::ArchiveObserverUtil::deactivate_archive (const Handle ArchiveHandle) {

   const String Name (__state.defs.lookup_named_handle_name (ArchiveHandle));

   if (Name) { deactivate_archive (Name); }
}


//! Leaves all joined archive groups.
void
dmz::ArchiveObserverUtil::deactivate_archive_all () {

   if (__state.module) {

      HashTableStringIterator it;

      Handle *ptr (__state.handleTable.get_first (it));

      while (ptr) {

         __state.validHandles.remove_handle (*ptr);
         __state.module->release_archive_observer (*ptr, *this);
         ptr = __state.handleTable.get_next (it);
      }
   }
}


/*!

\brief Gets archive module with which the observer is registered.
\return Returns a pointer to the ArchiveModule with which the observer is registered.
Returns NULL if the observer is not registered with any ArchiveModule.

*/
dmz::ArchiveModule *
dmz::ArchiveObserverUtil::get_archive_module () { return __state.module; }


// ArchiveObserer Interface.
dmz::StringContainer
dmz::ArchiveObserverUtil::get_archive_scope (const Handle ArchiveHandle) {

   StringContainer result;

   result.add_string (get_archive_observer_name ());

   return result;
}


void
dmz::ArchiveObserverUtil::store_archive_module (
      const String &Name,
      ArchiveModule &module) {

   if (!__state.ArchiveModuleName || (Name == __state.ArchiveModuleName)) {

      if (!__state.module) {

         __state.module = &module;

         HashTableStringIterator it;

         Handle *ptr (__state.handleTable.get_first (it));

         while (ptr) {

            module.register_archive_observer (*ptr, *this);
            ptr = __state.handleTable.get_next (it);
         }
      }
   }
}


void
dmz::ArchiveObserverUtil::remove_archive_module (
      const String &Name,
      ArchiveModule &module) {

   if (__state.module == &module) {

      deactivate_archive_all ();
      __state.module = 0;
   }
}

