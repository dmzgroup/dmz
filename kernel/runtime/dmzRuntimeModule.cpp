/*!

\class dmz::RuntimeModule
\ingroup Runtime
\brief Provides an interface to access plugins loaded into the runtime.

\fn dmz::RuntimeModule *dmz::RuntimeModule::cast (const Plugin *PluginPtr, const String &PluginName)
\brief Casts Plugin pointer to an RuntimeModule
\details If the Plugin object implements the RuntimeModule interface, a pointer
to the RuntimeModule interface of the Plugin is returned.
\param[in] PluginPtr Pointer to the Plugin.
\param[in] PluginName String containing name of desired RuntimeModule.
\return Returns a pointer to the RuntimeModule. Returns NULL if the Plugin does not
implement the RuntimeModule interface or the \a PluginName is not empty and not
equal to the Plugin's name.

\fn dmz::RuntimeModule::RuntimeModule (const PluginInfo &Info)
\brief Constructor.
\param[in] Info PluginInfo containing initialization data.

\fn dmz::RuntimeModule::~RuntimeModule ()
\brief Destructor.

\fn dmz::String dmz::RuntimeModule::get_runtime_module_name () const
\brief Gets the name of the runtime module.
\return Returns String containing the name of the runtime module.

\fn dmz::Handle dmz::RuntimeModule::get_runtime_module_handle () const
\brief Gets the Handle of the runtime module.
\return Returns Handle containing the runtime handle of the runtime module.

\fn void dmz::RuntimeModule::get_plugin_list (HandleContainer &container)
\brief Gets a list of loaded plugin runtime handles.
\param[out] container HandleContainer used to return the list of plugin runtime handles.

\fn const dmz::PluginInfo *dmz::RuntimeModule::lookup_plugin_info (const Handle PluginHandle)
\brief Returns a pointer to the PluginInfo of the specified Plugin.
\param[in] PluginHandle Handle containing the runtime handle of the plugin.
\return Returns a pointer to the PluginInfo of the specified Plugin. Returns NULL if
the handle is not valid.

\fn dmz::Boolean dmz::RuntimeModule::add_plugin (PluginInfo *info, Plugin *plugin)
\brief Adds a Plugin to the runtime.
\param[in] info PluginInfo associated with the Plugin being registered.
\param[in] plugin Pointer to Plugin being registered.
\return Returns dmz::True if the plugin was successfully registered.

\fn dmz::Boolean dmz::RuntimeModule::add_plugins (PluginContainer &container)
\brief Add Plugins to the runtime.
\details The \a container is emptied if the operation is successful.
\param[in] container PluginContainer holding Plugins to be added to the runtime.
\return Returns dmz::True if the Plugins were added.

\fn dmz::Boolean dmz::RuntimeModule::unload_plugin (const Handle PluginHandle)
\brief Unloads the specified plugin from the runtime.
\details The specified Plugin is unloaded from the runtime. The Plugin is also deleted if
possible. Additionally, the DynamicLibrary used to load the Plugin is unloaded if
possible.
\param[in] PluginHandle Handle of the Plugin to unload.
\return Returns dmz::True if the specified Plugin was successfully unloaded.

*/
