#include <dmzRuntimeLog.h>
#include <dmzRuntimeModule.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzRuntimePluginContainer.h>
#include <dmzRuntimeRTTI.h>
#include <dmzTypesHandleContainer.h>
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

struct PluginStruct {

   dmz::Boolean HasInterface;
   dmz::PluginInfo *info;
   dmz::Plugin *plugin;
   dmz::Log *log;

   void delete_plugin () {

      if (info && (dmz::PluginDeleteModeDelete == info->get_delete_mode ())) {

         if (plugin) {

            if (log && info) {

               log->info << "Deleting plugin: " << info->get_name () << dmz::endl;
            }

            delete plugin; plugin = 0;
         }
      }
      else if (log) {

         log->info << "Not Deleting plugin: " << (info ? info->get_name () : "Unknown")
            << dmz::endl;
      }
   }

   PluginStruct (
         dmz::PluginInfo *theInfo,
         dmz::Plugin *thePlugin,
         dmz::Log *theLog) :
         HasInterface (dmz::has_rtti_interface (thePlugin)),
         info (theInfo),
         plugin (thePlugin),
         log (theLog) {;}

   ~PluginStruct () {

      // Info must be delete AFTER plugin so the lib is unloaded After the
      // plugin's destructor is called.
      if (info) {

         delete_plugin ();
         delete info; info = 0;
      }
   }
};

typedef dmz::HashTableHandleTemplate<PluginStruct> PluginTable;

struct LevelStruct {

   const dmz::UInt32 Level;
   LevelStruct *next;
   LevelStruct *prev;
   PluginTable table;

   LevelStruct (const dmz::UInt32 TheLevel) : Level (TheLevel), next (0), prev (0) {;}
   ~LevelStruct () { table.clear (); if (next) { delete next; next = 0; } }
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

struct dmz::PluginContainer::State : public Plugin, public RuntimeModule {

   PluginInfo &info;
   Boolean discovered;
   Boolean started;
   PluginTable interfaceTable;
   PluginTable pluginTable;
   HashTableHandleTemplate<const Plugin> externTable;
   LevelStruct *levelsHead;
   LevelStruct *levelsTail;
   UInt32 maxLevel;
   Log *log;

   void delete_plugins () {

      discovered = False;
      started = False;
      interfaceTable.clear ();

      HashTableHandleIterator it;
      PluginStruct *ps (pluginTable.get_first (it));

      while (ps) {

         ps->delete_plugin ();
         ps = pluginTable.get_next (it);
      }

      pluginTable.empty ();
      externTable.clear ();
      if (levelsHead) { delete levelsHead; levelsHead = 0; }
      levelsTail = 0;
      maxLevel = 1;
   }

   State (PluginInfo &theInfo, Log *theLog) :
         Plugin (theInfo),
         RuntimeModule (theInfo),
         info (theInfo),
         discovered (False),
         started (False),
         levelsHead (0),
         levelsTail (0),
         maxLevel (1),
         log (theLog) {

      externTable.store (get_plugin_handle (), this);
   }


   ~State () { delete_plugins (); }


   // Plugin Interface
   virtual void update_plugin_state (const PluginStateEnum State, const UInt32 Level) {;}

   virtual void discover_plugin (const PluginDiscoverEnum Mode, const Plugin *PluginPtr) {

   }


   // RuntimeModule Interface
   virtual void get_plugin_list (HandleContainer &container) {

      HashTableHandleIterator it;
      PluginStruct *ps (0);
   
      while (pluginTable.get_next (it, ps)) {

         container.add_handle (it.get_hash_key ());
      }
   }


   virtual const PluginInfo *lookup_plugin_info (const Handle PluginHandle) {

      PluginInfo *result (0);

      PluginStruct *ps (pluginTable.lookup (PluginHandle));

      if (ps) { result = ps->info; }

      return result;
   }


   virtual Boolean add_plugin (PluginInfo *info, Plugin *plugin) {

      Boolean result (False);

      if (info && plugin) {

         PluginStruct *ps (new PluginStruct (info, plugin, log));

         if (ps && ps->info) {

            update_levels (*ps);

            if (pluginTable.store (ps->info->get_handle (), ps)) {

               if (ps->info && ps->HasInterface) {

                  interfaceTable.store (ps->info->get_handle (), ps);
               }

               result = True;
            }
            else { delete ps; ps = 0; }

            if (discovered) {

               discover_all_plugins (plugin);

               if (ps->HasInterface) { discover_plugin (plugin); }
            }

            if (started) {

                LevelStruct *ls (levelsTail);

                while (ls) {

                   if (info->uses_level (ls->Level)) {

                      plugin->update_plugin_state (PluginStateInit, ls->Level);
                   }

                   ls = ls->prev;
                }

                ls = levelsTail;

                while (ls) {

                   if (info->uses_level (ls->Level)) {

                      plugin->update_plugin_state (PluginStateStart, ls->Level);
                   }

                   ls = ls->prev;
               }
            }
         }

         result = True;
      }

      return result;
   }


   virtual Boolean add_plugins (PluginContainer &container) {

      Boolean result (True);

      HandleContainer pluginList;

      container.get_plugin_list (pluginList);

      Handle plugin = pluginList.get_first ();

      while (plugin) {

         PluginInfo *infoPtr = container.lookup_plugin_info (plugin);
         Plugin *pluginPtr = container.lookup_plugin (plugin);

         if (add_plugin (infoPtr, pluginPtr)) { container.release_plugin (plugin); }
         else { container.remove_plugin (plugin); result = False; } 

         plugin = pluginList.get_next ();
      }

      return result;
   }


   virtual Boolean unload_plugin (const Handle PluginHandle) {

      Boolean result (False);

      PluginStruct *ps = release_plugin (PluginHandle);

      if (ps) { delete ps; ps = 0; result = True; }

      return result;
   }


   // PluginContainer::State Interface
   LevelStruct *get_level (const UInt32 Level) {

      if (Level > maxLevel) { maxLevel = Level; }

      LevelStruct *result (0);

      if (!levelsHead) { result = levelsHead = levelsTail = new LevelStruct (Level); }
      else {

         LevelStruct *current (levelsHead);

         while (current && !result) {

            if (current->Level == Level) { result = current; }
            else if (!current->next) {

               result = new LevelStruct (Level);

               if (result) {

                  current->next = levelsTail = result;
                  result->prev = current;
               }
               else { current = 0; }
            }
            else if (current->Level > Level) {

               result = new LevelStruct (Level);

               if (result) {

                  if (!current->prev) { levelsHead = result; }
                  else { current->prev->next = result; }
                  result->next = current;
                  result->prev = current->prev;
                  current->prev = result;
               }
               else { current = 0; }
            }
            else { current = current->next; }
         }
      }

      return result;
   }

   void update_levels (PluginStruct &ps) {

      if (ps.info) {

         UInt32 level (ps.info->get_first_level ());

         while (level) {

            LevelStruct *ls (get_level (level));

            if (ls) {

               ls->table.store (ps.info->get_handle (), &ps);
            }

            level = ps.info->get_next_level ();
         }
      }
   }

   void discover_plugin (const Plugin *PluginPtr) {

      HashTableHandleIterator it;

      for (
            PluginStruct *current = pluginTable.get_first (it);
            current;
            current = pluginTable.get_next (it)) {

         if (current->plugin) {

            current->plugin->discover_plugin (PluginDiscoverAdd, PluginPtr);
         }
      }
   }

   void remove_plugin (const Plugin *PluginPtr) {

      HashTableHandleIterator it;

      for (
            PluginStruct *current = pluginTable.get_last (it);
            current;
            current = pluginTable.get_prev (it)) {

         if (current->plugin) {

            current->plugin->discover_plugin (PluginDiscoverRemove, PluginPtr);
         }
      }
   }

   void discover_all_plugins (Plugin *pluginPtr) {

      if (pluginPtr) {

         HashTableHandleIterator it;

         for (
               PluginStruct *current = interfaceTable.get_first (it);
               current;
               current = interfaceTable.get_next (it)) {

            pluginPtr->discover_plugin (PluginDiscoverAdd, current->plugin);
         }

         for (
               const Plugin *ExPtr = externTable.get_first (it);
               ExPtr;
               ExPtr = externTable.get_next (it)) {

            pluginPtr->discover_plugin (PluginDiscoverAdd, ExPtr);
         }
      }
   }

   void remove_all_plugins (Plugin *pluginPtr) {

      if (pluginPtr) {

         HashTableHandleIterator it;

         for (
               PluginStruct *current = interfaceTable.get_last (it);
               current;
               current = interfaceTable.get_prev (it)) {

            pluginPtr->discover_plugin (PluginDiscoverRemove, current->plugin);
         }

         for (
               const Plugin *ExPtr = externTable.get_last (it);
               ExPtr;
               ExPtr = externTable.get_prev (it)) {

            pluginPtr->discover_plugin (PluginDiscoverRemove, ExPtr);
         }
      }
   }

   PluginStruct *release_plugin (Handle PluginHandle) {

      PluginStruct *ps = (pluginTable.remove (PluginHandle));

      if (ps && ps->info && ps->plugin) {

         if (started) {

            LevelStruct *ls (levelsHead);

            while (ls) {

               if (ps->info->uses_level (ls->Level)) {

                  ps->plugin->update_plugin_state (PluginStateStop, ls->Level);
               }

               ls = ls->next;
            }

            ls = levelsHead;

            while (ls) {

               if (ps->info->uses_level (ls->Level)) {

                  ps->plugin->update_plugin_state (PluginStateShutdown, ls->Level);
               }

               ls = ls->next;
            }
         }

         if (discovered) {

            remove_all_plugins (ps->plugin);

            if (ps->HasInterface) {

               remove_plugin (ps->plugin);
               interfaceTable.remove (PluginHandle);
            }
         }
      }

      LevelStruct *ls = levelsHead;

      while (ls) { ls->table.remove (PluginHandle); ls = ls->next; }

      return ps;
   }
};


/*!

\brief Constructor.
\param[in] context Pointer to the runtime context.
\param[in] log Pointer to dmz::Log used for log messaging. If the pointer is NULL
The container will not create any log messages.

*/
dmz::PluginContainer::PluginContainer (RuntimeContext *context, Log *log) :
      _state (
         *(new State (
            *(new PluginInfo (
               "",
               "PluginContainer",
               "",
               "",
               PluginDeleteModeDoNotDelete,
               context,
               0)),
         log))) {;}


/*!

\brief Destructor.

All Plugin instances and their corresponding libraries will be delete unless otherwise
specified by the PluginInfo.

*/
dmz::PluginContainer::~PluginContainer () {

   PluginInfo *info = &(_state.info);
   delete &_state;
   if (info) { delete info; info = 0; }
}


/*!

\brief Fills a HandleContainer with the Handles of all the loaded Plugins in the PluginContainer.
\param[out] container HandleContainer used to return the loaded Plugin Handles.

*/
void
dmz::PluginContainer::get_plugin_list (HandleContainer &container) {

   _state.get_plugin_list (container);
}


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

   return _state.add_plugin (info, plugin);
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

   PluginStruct *ps = _state.pluginTable.lookup (PluginHandle);

   if (ps) { result = ps->info; }

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

   PluginStruct *ps = _state.pluginTable.lookup (PluginHandle);

   if (ps) { result = ps->plugin; }

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

   PluginStruct *ps = _state.release_plugin (PluginHandle);

   if (ps) { delete ps; ps = 0; result = True; }

   return result;
}


/*!

\brief Release Plugin with out deleting it.
\details The Plugin is removed from all Plugins still in the container. This does not
delete the plugin but only removed it from the container.
\param[in] PluginHandle Unique Plugin handle.
\return Returns dmz::True if the Plugin was released from the container.

*/
dmz::Boolean
dmz::PluginContainer::release_plugin (const Handle PluginHandle) {

   Boolean result (False);

   PluginStruct *ps = _state.release_plugin (PluginHandle);

   if (ps) {

      ps->plugin = 0;
      ps->info = 0;
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
         PluginStruct *ps = _state.pluginTable.get_first (it);
         ps;
         ps = _state.pluginTable.get_next (it)) {

      _state.discover_all_plugins (ps->plugin);
   }
}


//! All Plugins are initialized.
void
dmz::PluginContainer::init_plugins () {

   LevelStruct *ls (_state.levelsTail);

   while (ls) {

      if (_state.log) {

         _state.log->info << "Initializing Level " << ls->Level << " Plugins" << endl;
      }

      HashTableHandleIterator it;

      for (
            PluginStruct *current = ls->table.get_first (it);
            current;
            current = ls->table.get_next (it)) {

         if (current->plugin) { current->plugin->update_plugin_state (PluginStateInit, ls->Level); }
      }

      ls = ls->prev;
   }
}


//! All Plugins are started.
void
dmz::PluginContainer::start_plugins () {

   _state.started = True;

   LevelStruct *ls (_state.levelsTail);

   while (ls) {

      if (_state.log) {

         _state.log->info << "Starting Level " << ls->Level << " Plugins" << endl;
      }

      HashTableHandleIterator it;

      for (
            PluginStruct *current = ls->table.get_first (it);
            current;
            current = ls->table.get_next (it)) {

         if (current->plugin) { current->plugin->update_plugin_state (PluginStateStart, ls->Level); }
      }

      ls = ls->prev;
   }
}


//! All Plugins are stopped.
void
dmz::PluginContainer::stop_plugins () {

   _state.started = False;

   LevelStruct *ls (_state.levelsHead);

   while (ls) {

      if (_state.log) {

         _state.log->info << "Stopping Level " << ls->Level << " Plugins" << endl;
      }

      HashTableHandleIterator it;

      for (
            PluginStruct *current = ls->table.get_last (it);
            current;
            current = ls->table.get_prev (it)) {

         if (current->plugin) { current->plugin->update_plugin_state (PluginStateStop, ls->Level); }
      }

      ls = ls->next;
   }
}


//! All Plugins are shutdown.
void
dmz::PluginContainer::shutdown_plugins () {

   _state.started = False;

   LevelStruct *ls (_state.levelsHead);

   while (ls) {

      if (_state.log) {

         _state.log->info << "Shutting Down Level " << ls->Level << " Plugins" << endl;
      }

      HashTableHandleIterator it;

      for (
            PluginStruct *current = _state.pluginTable.get_last (it);
            current;
            current = _state.pluginTable.get_prev (it)) {

         if (current->plugin) { current->plugin->update_plugin_state (PluginStateShutdown, 0); }
      }

      ls = ls->next;
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
         PluginStruct *ps = _state.pluginTable.get_last (it);
         ps;
         ps = _state.pluginTable.get_prev (it)) {

      _state.remove_all_plugins (ps->plugin);
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

   PluginStruct *current (_state.pluginTable.get_first (it.state.it));
   return current ? current->plugin : 0;
}


/*!

\brief Gets next Plugin in the container.
\param[in] it PluginIterator.
\return Returns pointer to next Plugin in the container. Returns NULL when end of the
list is reached.

*/
dmz::Plugin *
dmz::PluginContainer::get_next (PluginIterator &it) const {

   PluginStruct *current (_state.pluginTable.get_next (it.state.it));
   return current ? current->plugin : 0;
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

