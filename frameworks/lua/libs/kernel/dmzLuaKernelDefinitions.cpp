#include <dmzLuaKernel.h>
#include "dmzLuaKernelPrivate.h"
#include <dmzLuaKernelValidate.h>
#include <dmzSystem.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzTypesMask.h>
#include <dmzTypesString.h>
#include <luacpp.h>

using namespace dmz;

namespace {

static int
defs_lookup_state (lua_State *L) {

   int result = 0;

   const String Name (luaL_checkstring (L, 1));
   Mask value;

   Definitions defs (lua_get_runtime_context (L));
   defs.lookup_state (Name, value);

   if (lua_create_mask (L, &value)) { result = 1; }

   return  result;
}


static int
defs_lookup_state_name (lua_State *L) {

   int result = 0;

   const Mask *Ptr (lua_check_mask (L, 1));

   if (Ptr) {

      String value;
      Definitions defs (lua_get_runtime_context (L));
      defs.lookup_state_name (*Ptr, value);

      lua_pushstring (L, value.get_buffer ());
      result = 1;
   }

   return  result;
}

static const luaL_Reg arrayFunc [] = {
   {"lookup_state", defs_lookup_state},
   {"lookup_state_name", defs_lookup_state_name},
   {NULL, NULL},
};

};


//! \cond
void
dmz::open_lua_kernel_definitions_lib (lua_State *L) {

   LUA_START_VALIDATE (L);

   lua_create_dmz_namespace (L, "definitions");
   luaL_register (L, NULL, arrayFunc);
   lua_make_readonly (L, -1); // make time read only.
   lua_pop (L, 1); // dmz.time table.

   LUA_END_VALIDATE (L, 0);
}
//! \endcond

