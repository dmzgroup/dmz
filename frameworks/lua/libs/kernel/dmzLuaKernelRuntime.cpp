#include <dmzLuaKernel.h>
#include "dmzLuaKernelPrivate.h"
#include <dmzLuaKernelRuntime.h>
#include <dmzLuaKernelValidate.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimeMessaging.h>
#include <dmzTypesString.h>

#include <luacpp.h>

using namespace dmz;

namespace {

static const char InstanceTableName[] = "dmz.runtime.table.instance";

}

//! Finds the first empty index in a Lua table.
int
dmz::lua_find_empty_index (lua_State *L) {

   LUA_START_VALIDATE (L);

   int result (0);

   if (lua_istable (L, -1)) {

      const int Table = lua_gettop (L);

      lua_pushnil (L);

      while (!result && lua_next (L, Table)) {

         if (lua_objlen (L, -1) == 0) {

            lua_pushvalue (L, -2);
            result = lua_tointeger (L, -1);
            lua_pop (L, 1);
         }

         lua_pop (L, 1);
      }

      if (result) { lua_pop (L, 1); }
      else { result = lua_objlen (L, Table) + 1; }
   }

   LUA_END_VALIDATE (L, 0);

   return result;
}


//! Pushes an instance table onto the stack from the registry.
void
dmz::lua_push_instance_table (lua_State *L) {

   LUA_START_VALIDATE (L);

   lua_pushstring (L, InstanceTableName);
   lua_rawget (L, LUA_REGISTRYINDEX);

   LUA_END_VALIDATE (L, 1);
}


//! \cond
void
dmz::open_lua_kernel_runtime_lib (lua_State *L) {

   LUA_START_VALIDATE (L);

   lua_pushstring (L, InstanceTableName);
   lua_create_dmz_namespace (L, "plugins");
   lua_rawset (L, LUA_REGISTRYINDEX);

   LUA_END_VALIDATE (L, 0);
}
//! \endcond

