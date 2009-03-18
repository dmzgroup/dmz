/*!

\class dmz::LuaExt
\ingroup Lua
\brief Lua extension interface.

\fn dmz::LuaExt::LuaExt (const PluginInfo &Info)
\brief Constructor.

\fn dmz::LuaExt::~LuaExt ()
\brief Destructor.

\fn dmz::LuaExt *dmz::LuaExt::cast (
const Plugin *PluginPtr,
const String &PluginName)
\brief Casts Plugin pointer to an LuaExt.
\details If the Plugin object implements the LuaExt interface, a pointer to
the LuaExt interface of the Plugin is returned.
\param[in] PluginPtr Pointer to the Plugin to cast.
\param[in] PluginName String containing the name of the desired LuaExt.
\return Returns pointer to the LuaExt. Returns NULL if the PluginPtr does not
implement the LuaExt interface or the \a PluginName is not empty
and not equal to the Plugin's name.

\fn dmz::Handle dmz::LuaExt::get_lua_extension_handle ()
\brief Gets the Handle of the Lua Extension.

\fn dmz::String dmz::LuaExt::get_lua_extension_name ()
\brief Gets the name of the Lua Extension.

\fn void dmz::LuaExt::store_lua_module (LuaModule &module)
\brief Stores the LuaModule in the extension.

\fn void dmz::LuaExt::open_lua_extension (lua_State *L)
\brief Opens the extension.

\fn void dmz::LuaExt::close_lua_extension (lua_State *L)
\brief Closes the extension.

\fn void dmz::LuaExt::remove_lua_module (LuaModule &module)
\brief Remove the LuaModule from the extension.

*/
