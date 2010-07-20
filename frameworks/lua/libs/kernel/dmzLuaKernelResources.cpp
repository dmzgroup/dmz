#include <dmzLuaKernel.h>
#include "dmzLuaKernelPrivate.h"
#include <dmzLuaKernelValidate.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeResources.h>
#include <dmzRuntimeResourcesUpdate.h>
#include <dmzTypesString.h>
#include <luacpp.h>

using namespace dmz;

namespace {

static int
rc_find_file (lua_State *L) {

   int result = 0;

   const String Name (luaL_checkstring (L, 1));

   Resources rc (lua_get_runtime_context (L));
   const String FileName (rc.find_file (Name));

   if (FileName) { lua_pushstring (L, FileName.get_buffer ()); result = 1; }

   return  result;
}


static int
rc_store_config (lua_State *L) {

   int result = 0;

   const String Name (luaL_checkstring (L, 1));
   const Config *resource (lua_check_config (L, 2));

   if (resource) {

      ResourcesUpdate rc (lua_get_runtime_context (L));

      if (rc.update_resource (Name, *resource)) {

         lua_pushboolean (L, 1); result = 1;
      }
   }

   return  result;
}


static int
rc_lookup_config (lua_State *L) {

   int result = 0;

   const String Name (luaL_checkstring (L, 1));

   Resources rc (lua_get_runtime_context (L));
   Config resource;

   if (rc.lookup_resource_config (Name, resource)) {

      Config *ptr = lua_create_config (L, resource.get_name ());

      if (ptr) { *ptr = resource; result = 1; }
   }

   return  result;
}


static int
rc_remove_config (lua_State *L) {

   int result = 0;

   const String Name (luaL_checkstring (L, 1));

   ResourcesUpdate rc (lua_get_runtime_context (L));

   if (rc.remove_resource (Name)) {

      lua_pushboolean (L, 1); result = 1;
   }

   return  result;
}


static const luaL_Reg arrayFunc [] = {
   {"find_file", rc_find_file},
   {"store_config", rc_store_config},
   {"lookup_config", rc_lookup_config},
   {"remove_config", rc_remove_config},
   {NULL, NULL},
};

};


//! \cond
void
dmz::open_lua_kernel_resources_lib (lua_State *L) {

   LUA_START_VALIDATE (L);

   lua_create_dmz_namespace (L, "resources");
   luaL_register (L, NULL, arrayFunc);
   lua_make_readonly (L, -1); // make time read only.
   lua_pop (L, 1); // dmz.time table.

   LUA_END_VALIDATE (L, 0);
}
//! \endcond

