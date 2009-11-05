#include <dmzLuaKernel.h>
#include "dmzLuaKernelPrivate.h"
#include <dmzLuaKernelValidate.h>
#include <dmzRuntimeExit.h>
#include <dmzTypesString.h>
#include <luacpp.h>

using namespace dmz;

namespace {


static int
runtime_exit (lua_State *L) {

   RuntimeContext *context = lua_get_runtime_context (L);
   Exit quit (context);
   const String Reason = lua_tostring (L, -1);
   quit.request_exit (ExitStatusNormal, Reason);

   return 0;
}


static int
runtime_exit_forced (lua_State *L) {

   RuntimeContext *context = lua_get_runtime_context (L);
   Exit quit (context);
   const String Reason = lua_tostring (L, -1);
   quit.request_exit (ExitStatusForced, Reason);

   return 0;
}


static const luaL_Reg arrayFunc [] = {
   {"exit", runtime_exit},
   {"exit_forced", runtime_exit_forced},
   {NULL, NULL},
};

};


//! \cond
void
dmz::open_lua_kernel_runtime_exit_lib (lua_State *L) {

   LUA_START_VALIDATE (L);

   lua_create_dmz_namespace (L, "runtime");
   luaL_register (L, NULL, arrayFunc);
   lua_make_readonly (L, -1); // make log read only.
   lua_pop (L, 1); // pops meta table and dmz.log table.

   LUA_END_VALIDATE (L, 0);
}
//! \endcond

