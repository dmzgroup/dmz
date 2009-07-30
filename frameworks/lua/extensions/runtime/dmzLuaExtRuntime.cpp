#include "dmzLuaExtRuntime.h"
#include <dmzLuaKernelValidate.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginContainer.h>
#include <dmzRuntimeLoadPlugins.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzTypesHandleContainer.h>
#include <luacpp.h>

using namespace dmz;

namespace {

static char RuntimeKey = 'r';


static inline dmz::RuntimeModule *
get_runtime (lua_State *L) {

  RuntimeModule *result (0);
  lua_pushlightuserdata (L, (void *)&RuntimeKey);
  lua_rawget (L, LUA_REGISTRYINDEX);
  RuntimeModule **runtime = (RuntimeModule **)lua_touserdata (L, -1);
  if (runtime) { result = *runtime; }
  lua_pop (L, 1); // pop runtime module

  return result;
}


static int
runtime_plugin_list (lua_State *L) {

   LUA_START_VALIDATE (L);

   int result (0);

   RuntimeModule *runtime (get_runtime (L));
   HandleContainer list;

   if (runtime) {

      runtime->get_plugin_list (list);
      lua_handle_container_to_table (L, list);
      result = 1;
   }

   LUA_END_VALIDATE (L, result);

   return result;
}


static int
runtime_unload_plugin (lua_State *L) {

   LUA_START_VALIDATE (L);

   int result (0);

   RuntimeModule *runtime (get_runtime (L));
   Handle *pluginHandle (lua_check_handle (L, 1));


   if (runtime && pluginHandle) {

      lua_pushboolean (L, runtime->unload_plugin (*pluginHandle) ? 1 : 0); 
      result = 1;
   }

   LUA_END_VALIDATE (L, result);

   return result;
}


static int
runtime_load_plugin (lua_State *L) {

   LUA_START_VALIDATE (L);

   int result (0);

   RuntimeModule *runtime (get_runtime (L));
   const String PluginName (luaL_checkstring (L, 1));

   if (runtime && PluginName) {

      RuntimeContext *context = lua_get_runtime_context (L);
      Log log ("Lua Plugin Loader", context);
      PluginContainer container (context, &log);
      Config plugin ("plugin");
      plugin.store_attribute ("name", PluginName);
      Config pluginList ("plugin-list");
      pluginList.add_config (plugin);
      Config empty;

      if (load_plugins (context, pluginList, empty, empty, container, &log)) {

         if (!runtime->add_plugins (container)) {

            log.error << "Failed to add plugin: " << PluginName << endl;
         }
      }
   }

   LUA_END_VALIDATE (L, result);

   return result;
}


static const luaL_Reg arrayFunc[] = {
   {"plugin_list", runtime_plugin_list},
   {"unload_plugin", runtime_unload_plugin},
   {"load_plugin", runtime_load_plugin},
   {NULL, NULL},
};

};

dmz::LuaExtRuntime::LuaExtRuntime (const PluginInfo &Info, Config &local) :
      Plugin (Info),
      LuaExt (Info),
      _log (Info),
      _runtime (0),
      _runtimePtr (0) {

   _init (local);
}


dmz::LuaExtRuntime::~LuaExtRuntime () {

}


// Plugin Interface
void
dmz::LuaExtRuntime::update_plugin_state (
      const PluginStateEnum State,
      const UInt32 Level) {

   if (State == PluginStateInit) {

   }
   else if (State == PluginStateStart) {

   }
   else if (State == PluginStateStop) {

   }
   else if (State == PluginStateShutdown) {

   }
}


void
dmz::LuaExtRuntime::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      if (!_runtime) {

         _runtime = RuntimeModule::cast (PluginPtr);

         if (_runtime && _runtimePtr && !(*_runtimePtr)) { *_runtimePtr = _runtime; }
      }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_runtime && (_runtime == RuntimeModule::cast (PluginPtr))) {

         _runtime = 0;
         if (_runtimePtr) { *_runtimePtr = 0; }
      }
   }
}


// LuaExt Interface
void
dmz::LuaExtRuntime::store_lua_module (LuaModule &module) {

}


void
dmz::LuaExtRuntime::open_lua_extension (lua_State *L) {

   LUA_START_VALIDATE (L);

   lua_pushlightuserdata (L, (void *)&RuntimeKey);
   _runtimePtr = (RuntimeModule **)lua_newuserdata (L, sizeof (RuntimeModule *));

   if (_runtimePtr) {

      *_runtimePtr = _runtime;
      lua_rawset (L, LUA_REGISTRYINDEX);
   }
   else { lua_pop (L, 1); } // pop light data

   lua_create_dmz_namespace (L, "runtime");
   luaL_register (L, NULL, arrayFunc);

   lua_make_readonly (L, -1);
   lua_pop (L, 1); // pop dmz.runtime table

   LUA_END_VALIDATE (L, 0);
}


void
dmz::LuaExtRuntime::close_lua_extension (lua_State *L) {

}


void
dmz::LuaExtRuntime::remove_lua_module (LuaModule &module) {

}


// LuaExtRuntime Interface
void
dmz::LuaExtRuntime::_init (Config &local) {

}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzLuaExtRuntime (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::LuaExtRuntime (Info, local);
}

};
