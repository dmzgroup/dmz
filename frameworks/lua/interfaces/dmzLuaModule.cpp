/*!

\defgroup Lua DMZ Lua Framework
\brief The Lua Framework provides Lua bindings for the DMZ Kernel and various
frameworks. More information about Lua may be found here: http://www.lua.org/ \n\n
\htmlonly
The DMZ Lua binding documentation is available <a href="dmzlua.html">here</a>.
\endhtmlonly

\class dmz::LuaModule
\ingroup Lua
\brief Lua module interface.

\fn dmz::LuaModule *dmz::LuaModule::cast (
const Plugin *PluginPtr,
const String &PluginName)
\brief Casts Plugin pointer to an LuaModule.
\details If the Plugin object implements the LuaModule interface, a pointer to
the LuaModule interface of the Plugin is returned.
\param[in] PluginPtr Pointer to the Plugin to cast.
\param[in] PluginName String containing the name of the desired LuaModule.
\return Returns pointer to the LuaModule. Returns NULL if the PluginPtr does not
implement the LuaModule interface or the \a PluginName is not empty
and not equal to the Plugin's name.


\fn dmz::LuaModule::LuaModule (const PluginInfo &Info)
\brief Constructor.

\fn dmz::LuaModule::~LuaModule ()
\brief Destructor.

\fn void dmz::LuaModule::add_lua_path (const String &Path)
\brief Adds a path to the Lua search path.
\details Adds a path to the search path used by the Lua runtime to locate and load
Lua scripts reference by the "require" or "dofile" Lua functions.
\param[in] Path String containing the path to add to the Lua search path.

\fn void dmz::LuaModule::register_lua_observer (
const UInt32 CallbackMask,
LuaObserver &observer)
\brief Registers a LuaObserver.
\details If the observer has already been registered the \a CallbackMask is OR'd with
the previously stored \a  CallbackMask.
\param[in] CallbackMask A bit mask indicating which observer callbacks to activate.
\param[in] observer LuaObserver to register.
\sa dmzLuaObserver.h

\fn void dmz::LuaModule::release_lua_observer (
const UInt32 CallbackMask,
LuaObserver &observer)
\brief Releases a LuaObserver.
\details Only the callbacks specified by \a CallbackMask will be released. To release
all callback use dmz::LuaAllCallbacksMask.
\param[in] CallbackMask A bit mask indicating which observer callbacks to deactivate.
\param[in] observer LuaObserver to release.
\sa dmzLuaObserver.h

\fn dmz::Boolean dmz::LuaModule::reset_lua ()
\brief Resets the Lua runtime.
\return Returns dmz::True if the Lua runtime was successfully restarted.
\note Restarting the Lua runtime will cause all currently loaded Lua scripts to be
released from disk.

\fn void dmz::LuaModule::set_hook_count (const Int32 Count)
\brief Sets the hook count.
\param[in] Count Specifies the number of Lua instructions to process between hook count
callbacks.

\fn dmz::Int32 dmz::LuaModule::get_hook_count ()
\brief Gets the hook count.
\return Returns the number of Lua instructions executed between hook count callbacks.

*/
