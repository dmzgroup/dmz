/*!

\file dmzLuaObserver.h
\ingroup Lua
\brief Defines the lua observer interface and callback masks.

\class dmz::LuaObserver
\ingroup Lua
\brief Lua observer interface.

\fn dmz::LuaObserver *dmz::LuaObserver::cast (
const Plugin *PluginPtr,
const String &PluginName)
brief Casts Plugin pointer to an LuaObserver.
\details If the Plugin object implements the LuaObserver interface, a pointer to
the LuaObserver interface of the Plugin is returned.
\param[in] PluginPtr Pointer to the Plugin to cast.
\param[in] PluginName String containing the name of the desired LuaObserver.
\return Returns pointer to the LuaObserver. Returns NULL if the PluginPtr does not
implement the LuaObserver interface or the \a PluginName is not empty
and not equal to the Plugin's name.

\fn dmz::LuaObserver::LuaObserver (const PluginInfo &Info)
\brief Constructor.

\fn dmz::LuaObserver::~LuaObserver ()
\brief Destructor.

\fn dmz::String dmz::LuaObserver::get_lua_observer_name ()
\brief Gets observer's name.
\return Returns String containing the observer name.

\fn dmz::Handle dmz::LuaObserver::get_lua_observer_handle ()
\brief Gets the observer's handle.
\return Returns the observer's Handle.

\fn void dmz::LuaObserver::store_lua_module (const String &ModuleName, LuaModule &module)
\brief Stores Lua module in observer.
\param[in] ModuleName String containing the name of the module.
\param[in] module LuaModule reference.

\fn void dmz::LuaObserver::remove_lua_module (const String &ModuleName, LuaModule &module)
\brief Removes Lua module from observer.
\param[in] ModuleName String containing the name of the module.
\param[in] module LuaModule reference.

\fn void dmz::LuaObserver::add_lua_script (lua_State *L, const String &Path)
\brief Add script callback.
\details This callback is activated by dmz::LuaScriptUpdateMask.
\param[in] L Lua state.
\param[in] Path String containing the path to the added Lua script.

\fn void dmz::LuaObserver::remove_lua_script (lua_State *L, const String &Path)
\brief Remove script callback.
\details This callback is activated by dmz::LuaScriptUpdateMask.
\param[in] L Lua state.
\param[in] Path String containing the path to the Lua script being removed.

\fn void dmz::LuaObserver::update_lua_hook_count (lua_State *L, const Int32 Count)
\brief Hook count updated callback.
\details This callback is activated by dmz::LuaHookCountUpdateMask.
\param[in] L Lua state.
\param[in] Count Updated hook count.

\fn void dmz::LuaObserver::lua_hook (lua_State *L, lua_Debug *ar)
\brief Lua hook callback.
\details This callback is activated by dmz::LuaHookCallMask, dmz::LuaHookReturnMask,
dmz::LuaHookLineMask, and dmz::LuaHookCountMask. See Lua documentation for a
description of the various hook callbacks.

\fn void dmz::LuaObserver::lua_parse_error (lua_State *L)
\brief Lua script parse error callback.
\details This callback is activated by dmz::LuaErrorMask
\param[in] L Lua state.

\fn void dmz::LuaObserver::lua_runtime_error (lua_State *L)
\brief Lua runtime error callback.
\details This callback is activated by dmz::LuaErrorMask
\param[in] L Lua state.

*/
