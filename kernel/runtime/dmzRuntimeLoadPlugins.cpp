#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include "dmzRuntimeContext.h"
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzRuntimePluginContainer.h>
#include <dmzRuntimeLoadPlugins.h>
#include <dmzRuntimeTimeSlice.h>
#include <dmzSystem.h>
#include <dmzSystemDynamicLibrary.h>

/*!

\brief Creates Plugin instances and stores them in PluginContainer.
\ingroup Runtime
\details The Config \a pluginList should contain a set of Config objects. Each Config
object may contain the flowing attributes:
- \b name [\b Required] Name of the plugin. This attribute is used to create default
values for other attributes if they are not specified.
- \b unique Unique name for the Plugin. Will use the \b name attribute if not specified.
This attribute is useful when loading multiple Plugins of the same type. This attribute
controls which Config is passed into the factory function as the local Config as well
as the String returned by dmz::Plugin::get_plugin_name. A Plugin with the same unique
name may be specified more than once. However, only the first occurrence will be loaded.
This allows a Plugin to be specified in multiple configuration files while only be loaded
once.
- \b library Name of library used to create Plugin. Will use the \b name attribute if not
specified.
- \b factory Name of the factory function used to create the Plugin. Will use the \b name
attribute with the prefix "create_" added to the front if not specified.
- \b platform Specifies the platform the Plugin supports. Compares the value of the
attribute to the result of dmz::get_system_name. If the values are equal, the Plugin
is loaded. If this attribute is not specified, the loader assumes the Plugin is
suitable for all platforms.
- \b delete { true | false } Specifies whether the Plugin should be deleted by the
PluginContainer. Will default to true if the attribute is not specified.
- \b unload { true | false } Specifies whether the DynamicLibrary is unloaded by
the PluginContainer. Will default to true if the attribute is not specified.
- \b reserve { true | false } Specifies that the next slot in the TimeSlice list be
reserved for a Plugin with the given name. If \b reserve is set to true, the
plugin is \b not loaded. The Plugin must be specified again with out the \b reserve
attribute set in order to be loaded. This attribute may be used to reserve a TimeSlice
slot for things like Lua Plugins and extensions that are frequently not loaded until
much later in the start up process.
Will default to false if the attribute is not specified.

The following XML specifies that a Plugin named dmzNetExtLineObjects will be created
from a library named dmzNetExtLib. The loader will use the default factory function
named create_dmzNetExtLineObjects.
\code
<dmz>
   <plugins>
      <plugin name="dmzNetExtLineObjects" library="dmzNetExtLib"/>
   </plugins>
</dmz>
\endcode

\param[in] context Pointer to the runtime context.
\param[in] pluginList Contains list of Plugins to load.
\param[in] pluginInit Contains initialization data for Plugins to load.
\param[in] global Contains the global configuration data.
\param[out] container PluginContainer to store loaded Plugins.
\param[in] log Pointer to dmz::Log.
\return Returns dmz::True if all Plugins are loaded successfully.

*/
dmz::Boolean
dmz::load_plugins (
      RuntimeContext *context,
      Config &pluginList,
      Config &pluginInit,
      Config &global,
      PluginContainer &container,
      Log *log) {

   Boolean error (False);

   Definitions defs (context);

   ConfigIterator it;
   Config data;

   Boolean done (!pluginList.get_first_config (it, data) || !context);

   if (done && log) { log->info << "No plugins found in plugin list" << endl; }

   const String SystemName (get_system_name ());

   while (!done) {

      const String NameValue (config_to_string ("name", data));

      if (NameValue) {

         String defaultFactory ("create_");
         defaultFactory << NameValue;

         const String PluginName (config_to_string ("unique", data, NameValue));
         const Boolean ReserveTimeSlice (config_to_boolean ("reserve", data, False));
         const String LibName (config_to_string ("library", data, NameValue));
         const String FactoryName (config_to_string ("factory", data, defaultFactory));
         const String ScopeName (config_to_string ("scope", data, PluginName));
         const String Platform (config_to_string ("platform", data));
         Config levelConfig;
         data.lookup_all_config ("level", levelConfig);

         const PluginDeleteModeEnum DeleteMode (
            config_to_boolean ("delete", data, True) ?
               PluginDeleteModeDelete : PluginDeleteModeDoNotDelete);

         const DynamicLibraryModeEnum LibMode (
            config_to_boolean ("unload", data, True) ?
               DynamicLibraryModeUnload : DynamicLibraryModeKeep);

         Boolean load (True);

         if (load && ReserveTimeSlice) {

            const Handle PluginHandle (defs.create_named_handle (PluginName));

            if (reserve_time_slice_place (PluginHandle, context)) {

               if (log) {

                  log->info << "Reserving time slice place for: " << PluginName << endl;
               }
            }
            else if (log) {

               log->error << "Failed reserving time slice place for: " << PluginName
                  << endl;
            }

            load = False;
         }

         if (load) {

            const Handle PluginHandle (defs.lookup_named_handle (PluginName));

            if (PluginHandle && container.lookup_plugin (PluginHandle)) {

               load = False;

               if (log) {

                  log->info << "Skipping duplicate plugin: " << PluginName << endl;
               }
            }
         }

         if (load && !defs.create_unique_name (PluginName)) {

            if (log) {

               log->error << "Plugin name: " << PluginName
                  << " is not unique and can not be loaded" << endl;
            }

            load = False;
         }

         if (load && Platform && (SystemName != Platform)) {

            if (log) {

               log->info << "Skipping plugin: " << NameValue << " target platform: "
                  << Platform << endl;
            }

            load = False;
         }

         if (load) {

            DynamicLibrary *lib (new DynamicLibrary (LibName, LibMode));

            if (lib && lib->is_loaded ()) {

               create_plugin_factory_function func =
                  (create_plugin_factory_function)lib->get_function_ptr (FactoryName);

               if (func) {

                  PluginInfo *info (
                     new PluginInfo (
                        PluginName,
                        DeleteMode,
                        context,
                        lib));

                  if (info) {

                     ConfigIterator it;
                     Config level;

                     while (levelConfig.get_next_config (it, level)) {

                        info->add_level (config_to_uint32 ("value", level, 0));
                     }

                     Config local;

                     pluginInit.lookup_all_config_merged (ScopeName, local);

                     if (log) {

                        log->info << "Loading plugin: " << PluginName << " from: "
                           << LibName << endl;
                     }

                     Plugin *plugin = func (*info, local, global);

                     if (plugin) {

                        if (container.add_plugin (info, plugin)) {

                           if (log) {

                              log->info << "Created plugin: " << PluginName
                              << "[" << plugin->get_plugin_handle () << "]" << endl;
                           }
                        }
                        else {

                           if (log) {

                              log->error << "Failed storing plugin: " << PluginName
                                 << " in plugin container." << endl;
                           }

                           if (info->get_delete_mode () == PluginDeleteModeDelete) {

                              delete plugin;
                           }

                           plugin = 0;
                           delete info; info = 0;

                           error = True;
                        }
                     }
                     else {

                        if (log) {

                           log->error << "Failed creating plugin: " << PluginName
                              << " from dynamic library: " << LibName
                              << " with factory fucntion: " << FactoryName << endl;
                        }

                        delete info; info = 0;

                        error = True;
                     }
                  }
                  else {

                     if (log) {

                        log->error << "Failed creating plugin info for plugin: "
                           << PluginName << endl;
                     }

                     error = True;
                  }
               }
               else {

                  if (log) {

                     log->error << "Unable to find factory function: " << FactoryName
                        << " in dynamic library: " << LibName << " for plugin: "
                        << PluginName << endl;
                  }

                  error = True;
               }
            }
            else if (lib && !lib->is_loaded ()) {

               if (log) {

                  log->error << "Failed to load dynamic library: " << LibName
                     << " for plugin: " << PluginName << " because: " << lib->get_error ()
                     << endl;
               }

               delete lib; lib = 0;

               error = True;
            }
            else {

               if (log) {

                  log->error << "Unable to create dynamic library: " << LibName
                     << " for plugin: " << PluginName << endl;
               }

               error = True;
            }
         }
      }
      else if (log) {

         log->error << "Plugin defined without name attribute." << endl;
      }

      done = !pluginList.get_next_config (it, data);
   }

   return error != True;
}

