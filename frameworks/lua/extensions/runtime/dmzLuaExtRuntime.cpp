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

typedef dmz::LuaExtRuntime::RuntimeStruct rstruct;

static char RuntimeKey = 'r';

static inline rstruct *
get_runtime_struct (lua_State *L) {

  rstruct *result (0);
  lua_pushlightuserdata (L, (void *)&RuntimeKey);
  lua_rawget (L, LUA_REGISTRYINDEX);

  rstruct **runtime = (rstruct **)lua_touserdata (L, -1);

  if (runtime && *runtime) { result = *runtime; }
  lua_pop (L, 1); // pop runtime module

  return result;
}


static inline dmz::RuntimeModule *
get_runtime (lua_State *L) {

  RuntimeModule *result (0);
  lua_pushlightuserdata (L, (void *)&RuntimeKey);
  lua_rawget (L, LUA_REGISTRYINDEX);

  rstruct **runtime = (rstruct **)lua_touserdata (L, -1);

  if (runtime && *runtime) { result = (*runtime)->runtime; }
  lua_pop (L, 1); // pop runtime module

  return result;
}


static int
runtime_plugin_list (lua_State *L) {

   LUA_START_VALIDATE (L);

   int result (0);

   RuntimeModule *runtime (get_runtime (L));

   if (runtime) {

      HandleContainer list;
      runtime->get_plugin_list (list);
      lua_handle_container_to_table (L, list);
      result = 1;
   }

   LUA_END_VALIDATE (L, result);

   return result;
}


static int
runtime_print_plugin_list (lua_State *L) {

   LUA_START_VALIDATE (L);

   int result (0);

   RuntimeModule *runtime (get_runtime (L));

   if (runtime) {

      RuntimeContext *context = lua_get_runtime_context (L);
      Log log ("", context);

      HandleContainer list;
      runtime->get_plugin_list (list);

      Handle plugin = list.get_first ();

      while (plugin) {

         const PluginInfo *InfoPtr = runtime->lookup_plugin_info (plugin);

         if (InfoPtr) {

            const String Name = InfoPtr->get_name ();
            const String ClassName = InfoPtr->get_class_name ();

            String out;
            out << Name << "[" << plugin << "]";

            if (Name != ClassName) { out << " of type: " << ClassName; }

            log.info << out << endl;
         }

         plugin = list.get_next ();
      }
   }

   LUA_END_VALIDATE (L, result);

   return result;
}


static int
runtime_load_plugin (lua_State *L) {

   LUA_START_VALIDATE (L);

   int result (0);

   rstruct *rs (get_runtime_struct (L));
   RuntimeModule *runtime (rs ? rs->runtime : 0);
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
      Config global (rs ? rs->global : empty);

      if (load_plugins (context, pluginList, global, empty, container, &log)) {

         if (!runtime->add_plugins (container)) {

            log.error << "Failed to add plugin: " << PluginName << endl;
         }
      }
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


static const luaL_Reg arrayFunc[] = {
   {"plugin_list", runtime_plugin_list},
   {"print_plugin_list", runtime_print_plugin_list},
   {"load_plugin", runtime_load_plugin},
   {"unload_plugin", runtime_unload_plugin},
   {NULL, NULL},
};

};

dmz::LuaExtRuntime::LuaExtRuntime (
      const PluginInfo &Info,
      Config &local,
      Config &global) :
      Plugin (Info),
      LuaExt (Info),
      _log (Info),
      _runtimePtr (0) {

   _runtime.global = global;
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

      if (!_runtime.runtime) {

         _runtime.runtime = RuntimeModule::cast (PluginPtr);
      }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_runtime.runtime && (_runtime.runtime == RuntimeModule::cast (PluginPtr))) {

         _runtime.runtime = 0;
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
   _runtimePtr = (RuntimeStruct **)lua_newuserdata (L, sizeof (RuntimeStruct *));

   if (_runtimePtr) {

      *_runtimePtr = &_runtime;
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

   return new dmz::LuaExtRuntime (Info, local, global);
}

};
