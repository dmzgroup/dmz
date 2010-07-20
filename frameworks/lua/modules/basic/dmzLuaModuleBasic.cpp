#include <dmzLuaKernel.h>
#include <dmzLuaKernelInit.h>
#include <dmzLuaKernelRuntime.h>
#include <dmzLuaKernelValidate.h>
#include "dmzLuaModuleBasic.h"
#include <dmzLuaObserver.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzRuntimeLoadPlugins.h>
#include <dmzRuntimeConfigToStringContainer.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzSystem.h>
#include <dmzSystemFile.h>

#ifdef DMZ_USE_LUA_JIT
#include <luajit.h>
#endif

#include <qdb.h>

static dmz::qdb out;

/*!

\class dmz::LuaModuleBasic
\ingroup Lua
\brief Basic implementation of the LuaModule interface.
\details
\code
<dmz>
<dmzLuaModuleBasic>
   <path value="Lua script search path"/>
   <!-- Note extensions work similar to the plugin specification -->
   <plugin-list>
      <plugin name="Name of Lua extension"/>
   </plugin-list>
   <!-- Note script instances work similar to the plugin specification.
        Everything but the name parameter is optional -->
   <instance
      name="Script instance name"
      unique="Scripts unique runtime name"
      scope="Scope to use for scripts initialization"
      platform="Platform supported by this script"
    /> 
</dmzLuaModuleBasic>
</dmz>

\endcode

*/


namespace {

static const char *localLuaKeywords[] = {
   "and",
   "break",
   "do",
   "else",
   "elseif",
   "end",
   "false",
   "for",
   "function",
   "if",
   "in",
   "local",
   "nil",
   "not",
   "or",
   "repeat",
   "return",
   "then",
   "true",
   "until",
   "while",
   NULL,
};

static const char LuaHookKey = 'h';

static void
local_lua_hook_function (lua_State *L, lua_Debug *ar) {

   lua_pushlightuserdata (L, (void *)&LuaHookKey);
   lua_rawget (L, LUA_REGISTRYINDEX);
   dmz::LuaModuleBasic **ptr = (dmz::LuaModuleBasic **)lua_touserdata (L, -1);
   lua_pop (L, 1); // pop user data

   if (ptr && *ptr) { (*ptr)->lua_hook_function (L, ar); }
}

};


//! \cond
dmz::LuaModuleBasic::LuaModuleBasic (
      const PluginInfo &Info,
      Config &local,
      Config &global) :
      Plugin (Info),
      TimeSlice (Info, TimeSliceTypeRuntime, TimeSliceModeSingle, 0.0),
      LuaModule (Info),
      _log (Info),
      _exit (Info),
      _extensions (Info.get_context (), &_log),
      _exitOnError (False),
      _startOptimizer (True),
      _luaState (0),
      _started (False),
      _hookCount (16),
      _global (global) {

   Int32 count (0);

   while (localLuaKeywords[count]) {

      _keywordTable.store (localLuaKeywords[count], (void *)this);
      count++;
   }

   _init (local, global);
}


dmz::LuaModuleBasic::~LuaModuleBasic () {

   HashTableHandleIterator it;
   LuaExt *ext (0);

   while (_extTable.get_next (it, ext)) { ext->remove_lua_module (*this); }

   _extTable.clear ();
   _instanceTable.empty ();
   _pathTable.clear ();
   _scriptTable.empty ();

   _obsCallTable.clear ();
   _obsReturnTable.clear ();
   _obsLineTable.clear ();
   _obsCountTable.clear ();
   _obsCountUpdateTable.clear ();
   _obsScriptTable.clear ();
   _obsErrorTable.clear ();
}


// Plugin Interface
void
dmz::LuaModuleBasic::update_plugin_state (
      const PluginStateEnum State,
      const UInt32 Level) {

   if (State == PluginStateStart) {

      _extensions.start_plugins ();
      if (!_started) { _start_lua_plugins (); }
      _started = True;
   }
   else if (State == PluginStateStop) { _extensions.stop_plugins (); }
   else if (State == PluginStateShutdown) {

      _stop_lua_plugins ();
      _close_lua ();

      _extensions.shutdown_plugins ();
   }
}


void
dmz::LuaModuleBasic::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      LuaObserver *obs = LuaObserver::cast (PluginPtr);

      if (obs) { obs->store_lua_module (get_plugin_name (), *this); }

      _discover_ext (PluginPtr);

      _extensions.discover_external_plugin (PluginPtr);
   }
   else if (Mode == PluginDiscoverRemove) {

      LuaObserver *obs = LuaObserver::cast (PluginPtr);

      if (obs) {

         obs->remove_lua_module (get_plugin_name (), *this);
         release_lua_observer (LuaAllCallbacksMask, *obs);
      }

      _extensions.remove_external_plugin (PluginPtr);

      _remove_ext (PluginPtr);
   }
}


// TimeSlice Interface
void
dmz::LuaModuleBasic::update_time_slice (const Float64 DeltaTime) {

   _log.info << "Reseting Lua Runtime." << endl;
   _stop_lua_plugins ();
   _close_lua ();
   _open_lua ();
   _start_lua_plugins ();
}


// LuaModule Interface
void
dmz::LuaModuleBasic::add_lua_path (const String &Path) {

   _luaPaths.append (Path);
   _add_lua_paths ();
}


void
dmz::LuaModuleBasic::register_lua_observer (
      const UInt32 CallbackMask,
      LuaObserver &observer) {

   const Handle ObsHandle (observer.get_lua_observer_handle ());

   if (CallbackMask & LuaHookCallMask) { _obsCallTable.store (ObsHandle, &observer); }
   if (CallbackMask & LuaHookReturnMask) { _obsReturnTable.store (ObsHandle, &observer); }
   if (CallbackMask & LuaHookLineMask) { _obsLineTable.store (ObsHandle, &observer); }
   if (CallbackMask & LuaHookCountMask) { _obsCountTable.store (ObsHandle, &observer); }

   if (CallbackMask & LuaHookCountUpdateMask) {

      if (_obsCountUpdateTable.store (ObsHandle, &observer)) {

         observer.update_lua_hook_count (_luaState, _hookCount);
      }
   }

   if (CallbackMask & LuaScriptUpdateMask) {

      _obsScriptTable.store (ObsHandle, &observer);
   }

   if (CallbackMask & LuaErrorMask) {

      _obsErrorTable.store (ObsHandle, &observer);
   }

   _update_hook_function ();
}


void
dmz::LuaModuleBasic::release_lua_observer (
      const UInt32 CallbackMask,
      LuaObserver &observer) {

   const Handle ObsHandle (observer.get_lua_observer_handle ());

   if (CallbackMask & LuaHookCallMask) { _obsCallTable.remove (ObsHandle); }
   if (CallbackMask & LuaHookReturnMask) { _obsReturnTable.remove (ObsHandle); }
   if (CallbackMask & LuaHookLineMask) { _obsLineTable.remove (ObsHandle); }
   if (CallbackMask & LuaHookCountMask) { _obsCountTable.remove (ObsHandle); }
   if (CallbackMask & LuaHookCountUpdateMask) { _obsCountUpdateTable.remove (ObsHandle); }
   if (CallbackMask & LuaScriptUpdateMask) { _obsScriptTable.remove (ObsHandle); }
   if (CallbackMask & LuaErrorMask) { _obsErrorTable.remove (ObsHandle); }

   _update_hook_function ();
}


void
dmz::LuaModuleBasic::reset_lua () { start_time_slice (); }


void
dmz::LuaModuleBasic::set_hook_count (const Int32 Count) {

   _hookCount = Count;

   HashTableHandleIterator it;

   LuaObserver *obs (_obsCountUpdateTable.get_first (it));

   while (obs) {

      obs->update_lua_hook_count (_luaState, _hookCount);
      obs = _obsCountUpdateTable.get_next (it);
   }

   _update_hook_function ();
}


dmz::Int32
dmz::LuaModuleBasic::get_hook_count () { return _hookCount; }


// LuaErrorHandler Interface
int
dmz::LuaModuleBasic::handle_lua_error (lua_State *L) {

   const String ErrorMessage (lua_tostring (L, -1));

   HashTableHandleIterator it;
   LuaObserver *obs (_obsErrorTable.get_first (it));

   const int Top = lua_gettop (L);

   while (obs) {

      obs->lua_runtime_error (L);

      const int CurrentTop = lua_gettop (L);

      if (CurrentTop > Top) {

         _log.error << "LuaObserver: " << obs->get_lua_observer_name ()
            << " left lua stack in invalid state. Auto correcting stack." << endl;
         lua_remove (L, CurrentTop - Top);
      }
      else if (CurrentTop < Top) {

         _log.error << "LuaObserver: " << obs->get_lua_observer_name ()
            << " removed too many items from the stack." << endl;
      }

      obs = _obsErrorTable.get_next (it);
   }

   lua_pushvalue (L, -1); // push error message for return

   _log.error << "Error: " << ErrorMessage << endl;

   lua_Debug ar;

   int level = 1;

   while (lua_getstack (L, level, &ar) && (level <= 50)) {

      lua_getinfo (L, "Snl", &ar);

      _log.error << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-" << endl;

      _log.error << "    Line: " << (Int32)ar.currentline << endl;

      const String Source ((ar.source[0] == '@') ? &(ar.source[1]) : ar.short_src);

      if (ar.namewhat[0] != '\0') { _log.error << "Function: " << ar.name; }
      else {

         const char What = ar.what[0];

         if (What == 'm') { _log.error << "Function: in main chunk"; }
         else { _log.error << "Function: ?"; }
      }

      _log.error << endl;

      _log.error << "  Source: " << Source << endl;

      level++;
   }

   _log.error << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-" << endl;

   if (_exitOnError) { _exit.request_exit (ExitStatusError, "Script error"); }

   return 1; // Returning error message
}


// LuaModuleBasic Interface
void
dmz::LuaModuleBasic::lua_hook_function (lua_State *L, lua_Debug *ar) {

   if (L && (L == _luaState) && ar) {

      HashTableHandleTemplate<LuaObserver> *which (0);

      if (LUA_HOOKCOUNT == ar->event) { which = &_obsCountTable; }
      else if (LUA_HOOKCALL == ar->event) { which = &_obsCallTable; }
      else if (LUA_HOOKLINE == ar->event) { which = &_obsLineTable; }
      else if ((LUA_HOOKRET == ar->event) && (LUA_HOOKTAILRET == ar->event)) {

         which = &_obsReturnTable;
      }

      if (which) {

         HashTableHandleIterator it;
         LuaObserver *obs (which->get_first (it));

         while (obs) {

            obs->lua_hook (L, ar);
            obs = which->get_next (it);
         }
      }
   }
}


void
dmz::LuaModuleBasic::_open_lua () {

   _log.info << "Opening Lua Runtime Scripts and Extensions." << endl;
   if (!_luaState) {

      _luaState = lua_open ();

      LUA_START_VALIDATE (_luaState);

      lua_pushlightuserdata (_luaState, (void *)&LuaHookKey);

      LuaModuleBasic **ptr =
         (LuaModuleBasic **)lua_newuserdata (_luaState, sizeof (LuaModuleBasic *));

      if (ptr) {

         *ptr = this;
         lua_rawset (_luaState, LUA_REGISTRYINDEX);
      }
      else { lua_pop (_luaState, 1); } // pop light user data

      _update_hook_function ();

      lua_register_error_handler (_luaState, this);

      luaL_openlibs (_luaState);
      dmz::open_lua_kernel_libs (_global, _luaState, get_plugin_runtime_context ());

      _add_lua_paths ();
      _start_optimizer ();

      HashTableHandleIterator it;

      LuaExt *ext = _extTable.get_first (it);

      while (ext) {

         ext->open_lua_extension (_luaState);

         ext = _extTable.get_next (it);
      }

      LUA_END_VALIDATE (_luaState, 0);
   }
}


void
dmz::LuaModuleBasic::_start_lua_plugins () {

   _log.info << "Starting Lua Plugins." << endl;
   HashTableStringIterator it;

   ScriptStruct *ss = _scriptTable.get_first (it);

   while (ss) {

      _load_script (*ss);
      ss = _scriptTable.get_next (it);
   }

   InstanceStruct *is = _instanceTable.get_first (it);

   while (is) {

      _instance_script (*is);

      is = _instanceTable.get_next (it);
   }
}


void
dmz::LuaModuleBasic::_stop_lua_plugins () {

   _log.info << "Stopping Lua Plugins." << endl;
   HashTableStringIterator it;

   lua_push_instance_table (_luaState);

   InstanceStruct *is = _instanceTable.get_first (it);

   while (is) {

      LUA_START_VALIDATE (_luaState);

      lua_getfield (_luaState, -1, is->Name.get_buffer ());

      if (lua_istable (_luaState, -1)) {

         const int Instance (lua_gettop (_luaState));

         lua_pushcfunction (_luaState, lua_error_handler);
         const int Handler (lua_gettop (_luaState));

         lua_pushstring (_luaState, "stop_plugin");
         lua_rawget (_luaState, Instance);

         if (lua_isfunction (_luaState, -1)) {

            lua_pushvalue (_luaState, Instance);

            if (lua_pcall (_luaState, 1, 0, Handler)) {

               lua_pop (_luaState, 1); // pop error message
            }
         }
         else { lua_pop (_luaState, 1); } // pop the item from getfield

         lua_pop (_luaState, 1); // pop lua_error_handler and Instance
      }

      lua_pop (_luaState, 1); // Pop the instance table

      LUA_END_VALIDATE (_luaState, 0);

      is = _instanceTable.get_next (it);
   }
}

void
dmz::LuaModuleBasic::_close_lua () {

   _log.info << "Closing Lua Runtime Scripts and Extensions." << endl;
   if (_luaState) {

      HashTableStringIterator scriptIt;

      ScriptStruct *ss (_scriptTable.get_first (scriptIt));

      while (ss) {

         HashTableHandleIterator it;
         LuaObserver *obs (_obsScriptTable.get_first (it));

         while (obs) {

            obs->remove_lua_script (_luaState, ss->File);
            obs = _obsScriptTable.get_next (it);
         }

         ss->loaded = False;
         ss = _scriptTable.get_next (scriptIt);
      }

      HashTableHandleIterator it;

      LuaExt *ext = _extTable.get_first (it);

      while (ext) {

         ext->close_lua_extension (_luaState);

         ext = _extTable.get_next (it);
      }

      lua_release_error_handler (_luaState, this);
      lua_close (_luaState); _luaState = 0;
   }
}


void
dmz::LuaModuleBasic::_update_hook_function () {

   if (_luaState) {

      int mask (0);

      if (_obsCallTable.get_count ()) { mask |= LUA_MASKCALL; }
      if (_obsReturnTable.get_count ()) { mask |= LUA_MASKRET; }
      if (_obsLineTable.get_count ()) { mask |= LUA_MASKLINE; }
      if (_obsCountTable.get_count ()) { mask |= LUA_MASKCOUNT; }

      if (mask) {

         lua_sethook (_luaState, local_lua_hook_function, mask, (int)_hookCount);
      }
      else { lua_sethook (_luaState, 0, 0, 0); }
   }
}


dmz::LuaModuleBasic::ScriptStruct *
dmz::LuaModuleBasic::_request_script (Config &script) {

   ScriptStruct *result (0);

   String name (config_to_string ("name", script));
   String file (config_to_string ("file", script));
   Boolean globals (config_to_boolean ("globals", script, False));

   result = _scriptTable.lookup (name);

   if (!result) {

      if (!file) { file = name + ".lua"; }

      if (_keywordTable.lookup (name)) {

         _log.error << "Name: '" << name << "' for script: " << file
            << " is a reserved Lua keyword." << endl;

         name.empty ();
      }
      else {

         String foundFile;

         if (find_file (_luaPaths, file, foundFile)) { file = foundFile; }
         else {

            _log.error << "Unable to find script file: " << file << endl;
            file.flush ();
         }
      }

      if (file && name) {

         result = new ScriptStruct (name, file, globals);

         if (result) {

            if (_scriptTable.store (name, result)) {

               if (_started) { _load_script (*result); }
            }
            else {

               _log.error << "Unable to load Lua script: " << file
                  << " because name: " << name << " is not unique." << endl;

               delete result; result = 0;
            }
         }
      }
   }

   return result;
}


static void
local_set_func_env (lua_State *L, const dmz::String &Name) {

   LUA_START_VALIDATE (L);

   lua_newtable (L); // Create function env table
   lua_pushvalue (L, -1); // Copy the table
   lua_setglobal (L, Name.get_buffer ()); // Set the env table as "name" in global env
   lua_newtable (L); // Create meta table
   lua_getglobal (L, "_G"); // Get the global table
   lua_setfield (L, -2, "__index"); // set __index = _G in meta table
   lua_setmetatable (L, -2); // set meta table in function table
   lua_setfenv (L, -2); // set function env table

   LUA_END_VALIDATE (L, 0);
}


void
dmz::LuaModuleBasic::_load_script (ScriptStruct &ss) {

   LUA_START_VALIDATE (_luaState);

   if (_luaState && ss.File && !ss.loaded) {

      lua_pushcfunction (_luaState, lua_error_handler);
      const int Handler = lua_gettop (_luaState);

      Int32 Status = luaL_loadfile (_luaState, ss.File.get_buffer ());

      if (Status) {

         if (!lua_isnil (_luaState, -1)) {

            const String ErrorMsg (lua_tostring (_luaState, -1));
            _log.error << ErrorMsg << endl;
         }
         else { _log.error << "Unknown error while loading: " << ss.File << endl; }

         lua_pop (_luaState, 1);
      }
      else {

         local_set_func_env (_luaState, ss.Name);

         if (lua_pcall (_luaState, 0, 0, Handler)) {

            _log.error << "Failed initializing script: " << ss.Name << " '"
               << ss.File << "'" << endl;
            lua_pop (_luaState, 1); // pop error message

            lua_pushnil (_luaState);
            lua_setglobal (_luaState, ss.Name.get_buffer ());
         }
         else {

            ss.loaded = True;

            _log.info << "Loaded Lua script: " << ss.File << endl;

            HashTableHandleIterator it;

            LuaObserver *obs (_obsScriptTable.get_first (it));

            while (obs) {

               obs->add_lua_script (_luaState, ss.File);
               obs = _obsScriptTable.get_next (it);
            }

            if (!ss.Globals) {

               lua_getglobal (_luaState, ss.Name.get_buffer ());

               if (lua_istable (_luaState, -1)) { lua_make_readonly (_luaState, -1); }

               lua_pop (_luaState, 1);
            }
         }
      }

      lua_pop (_luaState, 1); // pop error function
   }

   LUA_END_VALIDATE (_luaState, 0);
}


void
dmz::LuaModuleBasic::_instance_script (InstanceStruct &is) {

   LUA_START_VALIDATE (_luaState);

   if (is.Script.loaded) {

      lua_pushcfunction (_luaState, lua_error_handler);
      const int Handler = lua_gettop (_luaState);

      lua_getglobal (_luaState, is.Script.Name.get_buffer ());

      if (lua_istable (_luaState, -1)) {

         lua_getfield (_luaState, -1, "new");

         if (lua_isfunction (_luaState, -1)) {

            Config config;
            String scope ("dmz.");
            scope << is.Scope;
            _global.lookup_all_config_merged (scope, config);
            Config *ptr (lua_create_config (_luaState, ""));

            if (ptr) {

               *ptr = config;

               lua_pushstring (_luaState, is.Name.get_buffer ());

               if (!lua_pcall (_luaState, 2, 1, Handler)) {

                  _register_instance (is);
               }
               else {

                  _log.error << "Failed creating instance: " << is.Name
                     << " of script: " << is.Script.Name << endl;

                  lua_pop (_luaState, 1); // pop error message
               }
            }
         }
         else {

            _log.error << "Failed creating instance: " << is.Name
               << " because unable to find function \"new\" in script: "
               << is.Script.Name << endl;

            lua_pop (_luaState, 1); // pop error message
         }
      }
      else {

         _log.error << "Unable to create instance: " << is.Name << " of script: "
            << is.Script.Name << endl;
      }

      lua_pop (_luaState, 2); // pop error function and script table
   }

   LUA_END_VALIDATE (_luaState, 0);
}


void
dmz::LuaModuleBasic::_register_instance (InstanceStruct &is) {

   LUA_START_VALIDATE (_luaState);

   if (lua_istable (_luaState, -1)) {

      const int Instance (lua_gettop (_luaState));

      lua_pushcfunction (_luaState, lua_error_handler);
      const int Handler (lua_gettop (_luaState));

      lua_pushstring (_luaState, "start_plugin");
      lua_rawget (_luaState, Instance);

      if (lua_isfunction (_luaState, -1)) {

         lua_pushvalue (_luaState, Instance);

         if (lua_pcall (_luaState, 1, 0, Handler)) {

            lua_pop (_luaState, 1); // pop error message
         }
      }
      else { lua_pop (_luaState, 1); } // pop the item from rawget

      lua_pop (_luaState, 1); // pop lua_error_handler
   }

   lua_push_instance_table (_luaState);
   lua_insert (_luaState, -2); // Move instance table after instance
   lua_setfield (_luaState, -2, is.Name.get_buffer ());
   lua_pop (_luaState, 1); // Pop the instance table

   LUA_END_VALIDATE (_luaState, -1);
}


void
dmz::LuaModuleBasic::_discover_ext (const Plugin *PluginPtr) {

   const Handle ExtHandle (PluginPtr ? PluginPtr->get_plugin_handle () : 0);

   LuaExt *ext = LuaExt::cast (PluginPtr);

   if (ext && ExtHandle) {

      ext->store_lua_module (*this);

      if (_luaState) { ext->open_lua_extension (_luaState); }

      _extTable.store (ExtHandle, ext);
   }
}


void
dmz::LuaModuleBasic::_remove_ext (const Plugin *PluginPtr) {

   const Handle ExtHandle (PluginPtr ? PluginPtr->get_plugin_handle () : 0);

   LuaExt *ext = LuaExt::cast (PluginPtr);

   if (ext && ExtHandle) {

      if (_luaState) { ext->close_lua_extension (_luaState); }

      ext->remove_lua_module (*this);

      _extTable.remove (ExtHandle);
   }
}


void
dmz::LuaModuleBasic::_add_lua_paths () {

   if (_luaState && _luaPaths.get_count ()) {

      lua_getglobal (_luaState, "package");

      if (lua_istable (_luaState, -1)) {

         const int PackageTable (lua_gettop (_luaState));

         String pathStr;
         String path;
         Boolean found (_luaPaths.get_first (path));

         while (found) {

            if (path.get_char (-1) != '/') { path << "/"; }
            path << "?.lua";
            pathStr << path << ";";
            found = _luaPaths.get_next (path);
         }

         lua_pushstring (_luaState, pathStr.get_buffer ());

         lua_setfield (_luaState, PackageTable, "path");
      }
      else { _log.error << "Failed getting package table." << endl; }

      lua_pop (_luaState, 1); // pop package table
   }
}


void
dmz::LuaModuleBasic::_start_optimizer () {

#ifdef DMZ_LUA_USING_JIT_1
  if (_luaState) {

     lua_getglobal(_luaState, "require");
     lua_pushliteral (_luaState, "opt");
     if (!lua_pcall (_luaState, 1, 1, 0)) {

        const int OptTable (lua_gettop (_luaState));
        lua_getfield (_luaState, OptTable, "start");

        lua_remove (_luaState, OptTable);

        if (lua_isfunction (_luaState, -1)) {

           if (!lua_pcall (_luaState, 0, 0, 0)) {

              _log.info << "Lua optimizer started" << endl;
           }
           else {

              const String ErrorMsg (lua_tostring (_luaState, -1));
              lua_pop (_luaState, 1);
              _log.info << "Unable to start optimizer: " << ErrorMsg << endl;
           }
        }
        else { lua_pop (_luaState, -1); }
     }
     else {

        const String ErrorMsg (lua_tostring (_luaState, -1));
        lua_pop (_luaState, 1);
        _log.info << "Unable to load optimizer: " << ErrorMsg << endl;
     }
  }
#endif // DMZ_LUA_USING_JIT_1
//#ifdef DMZ_LUA_USING_JIT
#if 0
   if (_luaState) {

      lua_pushcfunction (_luaState, lua_error_handler);
      const int Handler (lua_gettop (_luaState));
      lua_getfield(_luaState, LUA_REGISTRYINDEX, "_LOADED");
      lua_getfield(_luaState, -1, "jit.opt");  // Get jit.opt.* module table.
      lua_remove(_luaState, -2); // Remove _LOADED
      lua_getfield(_luaState, -1, "start");
      lua_remove(_luaState, -2); // Remove jit.opt

      if (lua_pcall (_luaState, 1, 0, Handler)) {

         lua_pop (_luaState, 1); // pop error message
         _log.error << "Failed to run jit.opt.start function." << endl;
      }
      else { _log.info << "Called jit.opt.start." << endl; }

      lua_pop (_luaState, 1); // pop lua_error_handler
   }
#endif // DMZ_LUA_USING_JIT

}


void
dmz::LuaModuleBasic::_init (Config &local, Config &global) {

#ifdef DMZ_USE_LUA_JIT
   _log.info << "Using: " << LUAJIT_VERSION << endl;
#else
   _log.info << "Using: " << LUA_VERSION << endl;
#endif

   _exitOnError = config_to_boolean ("error.exit", local, _exitOnError);
   _startOptimizer = config_to_boolean ("optimizer.start", local, _startOptimizer);

   _luaPaths = config_to_path_string_container (local);

   Config pluginList;

   if (local.lookup_all_config ("plugin-list.plugin", pluginList)) {

      RuntimeContext *context (get_plugin_runtime_context ());

      if (dmz::load_plugins (context, pluginList, local, global, _extensions, &_log)) {

         _extensions.discover_plugins ();

         RuntimeIterator it;

         Plugin *ptr (_extensions.get_first (it));

         while (ptr) {

            _discover_ext (ptr);
            ptr = _extensions.get_next (it);
         }
      }
   }
   else { _log.info << "No extensions specified" << endl; }

   _open_lua ();

   Config scripts;

   if (local.lookup_all_config ("script", scripts)) {

      Config script;
      ConfigIterator it;

      for (
            Boolean found = scripts.get_first_config (it, script);
            found;
            found = scripts.get_next_config (it, script)) { _request_script (script); }
   }
   else { _log.info << "No Lua scripts specified" << endl; }

   Config instances;

   if (local.lookup_all_config ("instance", instances)) {

      const String SystemName (get_system_name ());
      Definitions defs (get_plugin_runtime_context ());
      ConfigIterator instanceIt;
      Config instance;

      for (
            Boolean found = instances.get_first_config (instanceIt, instance);
            found;
            found = instances.get_next_config (instanceIt, instance)) {

         ScriptStruct *ss (_request_script (instance));

         if (ss) {

            const String InstanceName (config_to_string ("unique", instance, ss->Name));
            const String Scope (config_to_string ("scope", instance, InstanceName));
            const String Platform (config_to_string ("platform", instance));

            Boolean load (True);

            if (!defs.create_unique_name (InstanceName)) {

               _log.error << "Unable to create script instance: " << InstanceName
                  << " of script: " << ss->Name << " [" << ss->File << "]"
                  << endl << "Instance name is not unique." << endl;

               load = False;
            }

            if (load && Platform && (SystemName != Platform)) {

               _log.info << "Skipping script instance: " << InstanceName
                  << " of script: " << ss->Name << " [" << ss->File << "]"
                  << endl << "Target platform: " << Platform << endl;
               load = False;
            }

            if (load) {

               InstanceStruct *is (new InstanceStruct (InstanceName, Scope, *ss));

               if (is) {

                  if (!_instanceTable.store (InstanceName, is)) {

                     _log.error << "Multiple instances of script: " << InstanceName
                        << " of script: " << ss->Name << " [" << ss->File
                        << "]" << endl << "Instance name is not unique."
                        << endl;

                     delete is; is = 0;
                  }
                  else { ss->instanceTable.store (InstanceName, is); }
               }
            }
         }
      }
   }
}
//! \endcond


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzLuaModuleBasic (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::LuaModuleBasic (Info, local, global);
}

};
