#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimePluginContainer.h>
#include <dmzRuntimeRTTI.h>
#include <dmzTypesHashTableHandleTemplate.h>

/*!

\class dmz::PluginIterator
\ingroup Runtime
\brief Class for iterating over Plugin instances stored in a PluginContainer.
\sa dmz::PluginContainer::get_first \n dmz::PluginContainer::get_next

*/

struct dmz::PluginIterator::State {

   HashTableHandleIterator it;
};


//! Base constructor.
dmz::PluginIterator::PluginIterator () : state (*(new State)) {;}


//! Destructor.
dmz::PluginIterator::~PluginIterator () { delete &state; }


namespace {

   struct pluginStruct {

      dmz::Boolean HasInterface;
      dmz::PluginInfo &info;
      dmz::Plugin &plugin;
      dmz::Log *log;

      pluginStruct (
            dmz::PluginInfo &theInfo,
            dmz::Plugin &thePlugin,
            dmz::Log *theLog) :
            HasInterface (dmz::has_rtti_interface (&thePlugin)),
            info (theInfo),
            plugin (thePlugin),
            log (theLog) {;}

      ~pluginStruct () {

         // Info must be delete AFTER plugin so the lib is unloaded After the
         // plugin's destructor is called.
         if (dmz::PluginDeleteModeDelete == info.get_delete_mode ()) {

            if (log) {

               log->info << "Deleting plugin: " << info.get_name () << dmz::endl;
            }

            delete &plugin;
            delete &info;
         }
         else if (log) {

            log->info << "Not Deleting plugin: " << info.get_name () << dmz::endl;
         }
      }
   };
};


/*!

\class dmz::PluginContainer
\ingroup Runtime
\brief Class for storing Plugin instances.
\details Container class for storing and manipulating a collection of Plugins.
This class provide the necessary functionality to manage extensions.
\sa dmz::PluginLoader

*/

struct dmz::PluginContainer::State {

   Boolean discovered;
   Boolean started;
   HashTableHandleIterator handleIt;
   HashTableHandleTemplate<pluginStruct> interfaceTable;
   HashTableHandleTemplate<pluginStruct> pluginTable;
   HashTableHandleTemplate<const Plugin> externTable;
   Log *log;

   void delete_plugins () {

      discovered = False;
      started = False;
      interfaceTable.clear ();
      pluginTable.empty ();
      externTable.clear ();
   }

   State (Log *theLog) : discovered (False), started (False), log (theLog) {;}
   ~State () { delete_plugins (); }

   void discover_plugin (const Plugin *PluginPtr) {

      for (
            pluginStruct *current = pluginTable.get_first (handleIt);
            current;
            current = pluginTable.get_next (handleIt)) {

         current->plugin.discover_plugin (PluginPtr);
      }
   }

   void remove_plugin (const Plugin *PluginPtr) {

      for (
            pluginStruct *current = pluginTable.get_last (handleIt);
            current;
            current = pluginTable.get_prev (handleIt)) {

         current->plugin.remove_plugin (PluginPtr);
      }
   }

   void discover_all_plugins (Plugin *pluginPtr) {

      if (pluginPtr) {

         for (
               pluginStruct *current = interfaceTable.get_first (handleIt);
               current;
               current = interfaceTable.get_next (handleIt)) {

            pluginPtr->discover_plugin (&(current->plugin));
         }

         HashTableHandleIterator it;

         for (
               const Plugin *ExPtr = externTable.get_first (it);
               ExPtr;
               ExPtr = externTable.get_next (it)) {

            pluginPtr->discover_plugin (ExPtr);
         }
      }
   }

   void remove_all_plugins (Plugin *pluginPtr) {

      if (pluginPtr) {

         for (
               pluginStruct *current = interfaceTable.get_last (handleIt);
               current;
               current = interfaceTable.get_prev (handleIt)) {

            pluginPtr->remove_plugin (&(current->plugin));
         }

         HashTableHandleIterator it;

         for (
               const Plugin *ExPtr = externTable.get_last (it);
               ExPtr;
               ExPtr = externTable.get_prev (it)) {

            pluginPtr->remove_plugin (ExPtr);
         }
      }
   }
};


/*!

\brief Constructor.
\param[in] log Pointer to dmz::Log used for log messaging. If the pointer is NULL
The container will not create any log messages.

*/
dmz::PluginContainer::PluginContainer (Log *log) : _state (*(new State (log))) {;}


/*!

\brief Destructor.

All Plugin instances and their corresponding libraries will be delete unless otherwise
specified by the PluginInfo.

*/
dmz::PluginContainer::~PluginContainer () { delete &_state; }


/*!

\brief Add a Plugin to the container.
\details If dmz::PluginContainer::discover_plugins has already been invoked then the
newly added Plugin will discover all Plugin instances in the container. If the
Plugins have been started then the added Plugin will also be started.
\param[in] info Pointer to a PluginInfo object.
\param[in] plugin Pointer to a Plugin object to added.
\return Returns dmz::True if the Plugin was successfully added.

*/
dmz::Boolean
dmz::PluginContainer::add_plugin (PluginInfo *info, Plugin *plugin) {

   Boolean result (False);

   if (info && plugin) {

      pluginStruct *ps (new pluginStruct (*info, *plugin, _state.log));

      if (ps) {

         if (_state.pluginTable.store (ps->info.get_handle (), ps)) {

            if (ps->HasInterface) {

               _state.interfaceTable.store (ps->info.get_handle (), ps);
            }

            result = True;
         }
         else { delete ps; ps = 0; }

         if (_state.discovered) { _state.discover_all_plugins (plugin); }
         if (_state.started) { plugin->start_plugin (); }
      }
   }

   return result;
}


/*!

\brief Looks up PluginInfo associated with the unique handle.
\param[in] PluginHandle Unique Plugin handle.
\return Returns pointer to dmz::PluginInfo. Returns NULL pointer if \a PluginHandle
is invalid.

*/
dmz::PluginInfo *
dmz::PluginContainer::lookup_plugin_info (const Handle PluginHandle) {

   PluginInfo *result (0);

   pluginStruct *ps = _state.pluginTable.lookup (PluginHandle);

   if (ps) { result = &(ps->info); }

   return result;
}


/*!

\brief Looks up Plugin associated with the unique handle.
\param[in] PluginHandle Unique Plugin handle.
\return Returns pointer to dmz::Plugin. Returns NULL pointer if \a PluginHandle
is invalid.

*/
dmz::Plugin *
dmz::PluginContainer::lookup_plugin (const Handle PluginHandle) {

   Plugin *result (0);

   pluginStruct *ps = _state.pluginTable.lookup (PluginHandle);

   if (ps) { result = &(ps->plugin); }

   return result;
}


/*!

\brief Remove Plugin.
\details The Plugin is stopped and shutdown. It is also be removed from all
Plugins still in the container. The Plugin will \b not be deleted if
specified by the PluginInfo.
\param[in] PluginHandle Unique Plugin handle.
\return Returns dmz::True if the Plugin was removed from the container.

*/
dmz::Boolean
dmz::PluginContainer::remove_plugin (const Handle PluginHandle) {

   Boolean result (False);

   pluginStruct *ps (_state.pluginTable.remove (PluginHandle));

   if (ps) {

      if (_state.started) { ps->plugin.stop_plugin (); ps->plugin.shutdown_plugin (); }

      if (_state.discovered) {

         _state.remove_all_plugins (&(ps->plugin));

         if (ps->HasInterface) {

            _state.remove_plugin (&(ps->plugin));
            _state.interfaceTable.remove (PluginHandle);
         }
      }

      delete ps; ps = 0;
      result = True;
   }

   return result;
}


/*!

\brief Performs Plugin discovery.
\details Every Plugin is allowed to discover every other Plugin in the container.

*/
void
dmz::PluginContainer::discover_plugins () {

   _state.discovered = True;

   HashTableHandleIterator it;

   if (_state.log) { _state.log->info << "Discovering Plugins" << endl; }

   for (
         pluginStruct *ps = _state.pluginTable.get_first (it);
         ps;
         ps = _state.pluginTable.get_next (it)) {

      _state.discover_all_plugins (&(ps->plugin));
   }
}


//! All Plugins are started.
void
dmz::PluginContainer::start_plugins () {

   _state.started = True;

   if (_state.log) { _state.log->info << "Starting Plugins" << endl; }

   for (
         pluginStruct *current = _state.pluginTable.get_first (_state.handleIt);
         current;
         current = _state.pluginTable.get_next (_state.handleIt)) {

      current->plugin.start_plugin ();
   }
}


//! All Plugins are stopped.
void
dmz::PluginContainer::stop_plugins () {

   _state.started = False;

   if (_state.log) { _state.log->info << "Stopping Plugins" << endl; }

   for (
         pluginStruct *current = _state.pluginTable.get_last (_state.handleIt);
         current;
         current = _state.pluginTable.get_prev (_state.handleIt)) {

      current->plugin.stop_plugin ();
   }
}


//! All Plugins are shutdown.
void
dmz::PluginContainer::shutdown_plugins () {

   _state.started = False;

   if (_state.log) { _state.log->info << "Shutting Down Plugins" << endl; }

   for (
         pluginStruct *current = _state.pluginTable.get_last (_state.handleIt);
         current;
         current = _state.pluginTable.get_prev (_state.handleIt)) {

      current->plugin.shutdown_plugin ();
   }
}


/*!

\brief Performs Plugin removal.
\details Every Plugin is removed from every other Plugin in the container.

*/
void
dmz::PluginContainer::remove_plugins () {

   _state.discovered = False;

   if (_state.log) { _state.log->info << "Removing Plugins" << endl; }

   HashTableHandleIterator it;

   for (
         pluginStruct *ps = _state.pluginTable.get_last (it);
         ps;
         ps = _state.pluginTable.get_prev (it)) {

      _state.remove_all_plugins (&(ps->plugin));
   }
}


/*!

\brief All Plugins and their corresponding PluginInfos are deleted unless specified
otherwise.

*/
void
dmz::PluginContainer::delete_plugins () {

   if (_state.log) { _state.log->info << "Deleting Plugins" << endl; }
   _state.delete_plugins ();
}


/*!

\brief Gets first Plugin in the container.
\param[in] it PluginIterator.
\return Returns pointer to first Plugin in the container. Returns NULL if container is
empty.

*/
dmz::Plugin *
dmz::PluginContainer::get_first (PluginIterator &it) const {

   pluginStruct *current (_state.pluginTable.get_first (it.state.it));
   return current ? &(current->plugin) : 0;
}


/*!

\brief Gets next Plugin in the container.
\param[in] it PluginIterator.
\return Returns pointer to next Plugin in the container. Returns NULL when end of the
list is reached.

*/
dmz::Plugin *
dmz::PluginContainer::get_next (PluginIterator &it) const {

   pluginStruct *current (_state.pluginTable.get_next (it.state.it));
   return current ? &(current->plugin) : 0;
}


/*!

\brief Discovers external Plugin.
\details The \a PluginPtr is discovered by all Plugins in the container. The \a PluginPtr
does \b not discover any of the Plugins in the container. This function is useful
when the PluginContainer is used to store extensions for a Plugin and the extensions
need access to other Plugins in the system. \a PluginPtr is not added to the container.
\param[in] PluginPtr Pointer to external Plugin to discover.

*/
void
dmz::PluginContainer::discover_external_plugin (const Plugin *PluginPtr) {

   if (has_rtti_interface (PluginPtr)) {

      HashTableHandleIterator it;

      if (PluginPtr) {

         if (_state.externTable.store (PluginPtr->get_plugin_handle (), PluginPtr) &&
               _state.discovered) {

            _state.discover_plugin (PluginPtr);
         }
      }
   }
}


/*!

\brief Removes external Plugin.
\details The \a PluginPtr is removed from all Plugins in the container.
when the PluginContainer is used to store extensions for a Plugin and the extensions
need to be notified that other Plugins in the system are being removed.
\param[in] PluginPtr Pointer to external Plugin to be removed.

*/
void
dmz::PluginContainer::remove_external_plugin (const Plugin *PluginPtr) {

   if (PluginPtr && _state.externTable.remove (PluginPtr->get_plugin_handle ())) {

      _state.remove_plugin (PluginPtr);
   }
}

