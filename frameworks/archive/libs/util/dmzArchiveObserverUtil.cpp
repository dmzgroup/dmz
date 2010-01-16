#include <dmzArchiveModule.h>
#include <dmzArchiveObserverUtil.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigToStringContainer.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimeLog.h>
#include <dmzTypesHashTableStringTemplate.h>
#include <dmzTypesHandleContainer.h>
#include <dmzTypesStringContainer.h>

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
   StringContainer scope;
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

   if (__state.module) {

      remove_archive_module (
         __state.module->get_archive_module_name (),
         *(__state.module));
   }

   delete &__state;
}


/*!

\brief Tests if the observer is attached to the specified archive group.
\param[in] ArchiveHandle Handle containing the archive group to test.
\return Returns dmz::True if the observer is attached the specified group.

*/
dmz::Boolean
dmz::ArchiveObserverUtil::is_active_archive_handle (const Handle ArchiveHandle) {

   return __state.validHandles.contains (ArchiveHandle);
}


/*!

\brief Attaches observer to specified archive groups.
\details The \a Init Config should contain a list of Config objects named
"archive". These objects should also contain an attribute called "name" which
contains the name of the archive group. If the name attribute is not set, the default
archive group name is used. Also calls init_archive_scope().
\code
<dmz>
<dmzFooPluginArchive>
   <archive/> <!-- attaches to default archive group -->
   <archive name="Extra Archive Group"/>
   <archive name="Some Other Archive Group"/>
</dmzFooPluginArchive>
</dmz>
\endcode
\param[in] Init Config containing a list of the archive groups.
\param[in] log Pointer to a Log used for logging.
\sa dmz::ArchiveObserverUtil::init_archive_scope()
*/
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

   init_archive_scope (Init);
}


/*!

\brief Creates a list of supported archive scopes.
\details The \a Init Config should contain a list of Config object called "string"
contained in a Config object called "archive-scope". Each "string" Config
should contain an attribute called "value" that specifies a scope name. When
processing an archive, the Archive module will first look for a Config object with
the first scope name specified in the scope list. It will continue down the list
until it find a matching Config object. When creating an archive, the archive module
will always use the first scope name in the list when creating the Config object
to be used to store the archive data.
\code
<dmz>
<dmzFooPluginArchive>
   <archive-scope>
      <string value="First Scope Name"/>
      <string value="Next Scope Name"/>
   </archive-scope>
</dmzFooPluginArchive>
</dmz>
\endcode
\param[in] Init Config containing the archive scope list.

*/
void
dmz::ArchiveObserverUtil::init_archive_scope (const Config &Init) {

   __state.scope = config_to_string_container ("archive-scope.string", Init);
}


/*!

\brief Adds a scope name to the archive's scope list.
\param[in] Scope String containing name of scope to add to list.

*/
void
dmz::ArchiveObserverUtil::add_archive_scope (const String &Scope) {

   __state.scope.append (Scope);
}


/*!

\brief Tests if the scope name list is empty.
\return Returns dmz::True if the list is empty. dmz::False is returned if the list
contains any scope names.

*/
dmz::Boolean
dmz::ArchiveObserverUtil::is_archive_scope_empty () {

   return __state.scope.get_count () == 0;
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

   if (__state.scope.get_count () != 0) { result = __state.scope; }
   else { result.append (get_archive_observer_name ()); }

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

