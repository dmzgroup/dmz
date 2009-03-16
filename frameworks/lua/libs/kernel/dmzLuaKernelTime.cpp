#include <dmzLuaKernel.h>
#include "dmzLuaKernelPrivate.h"
#include <dmzLuaKernelValidate.h>
#include <dmzSystem.h>
#include <dmzTypesString.h>
#include <dmzRuntimeTime.h>
#include <luacpp.h>

using namespace dmz;

namespace {


static int
time_frame_time (lua_State *L) {

   Time time (lua_get_runtime_context (L));

   if (lua_isnumber (L, 1)) {

      time.set_frame_time (lua_tonumber (L, 1));
   }

   lua_pushnumber (L, time.get_frame_time ());

   return 1;
}


static int
time_last_frame_time (lua_State *L) {

   Time time (lua_get_runtime_context (L));
   lua_pushnumber (L, time.get_last_frame_time ());

   return 1;
}


static int
time_frame_delta (lua_State *L) {

   Time time (lua_get_runtime_context (L));
   lua_pushnumber (L, time.get_frame_delta ());

   return 1;
}


static int
time_time_factor (lua_State *L) {

   Time time (lua_get_runtime_context (L));

   if (lua_isnumber (L, 1)) {

      time.set_time_factor (lua_tonumber (L, 1));
   }

   lua_pushnumber (L, time.get_time_factor ());

   return 1;
}


static int
time_target_frame_frequency (lua_State *L) {

   Time time (lua_get_runtime_context (L));

   if (lua_isnumber (L, 1)) {

      time.set_target_frame_frequency (lua_tonumber (L, 1));
   }

   lua_pushnumber (L, time.get_target_frame_frequency ());

   return 1;
}


static const luaL_Reg arrayFunc [] = {
   {"frame_time", time_frame_time},
   {"last_frame_time", time_last_frame_time},
   {"frame_delta", time_frame_delta},
   {"time_factor", time_time_factor},
   {"target_frame_frequency", time_target_frame_frequency},
   {NULL, NULL},
};

};


//! \cond
void
dmz::open_lua_kernel_time_lib (lua_State *L) {

   LUA_START_VALIDATE (L);

   lua_create_dmz_namespace (L, "time");
   luaL_register (L, NULL, arrayFunc);
   lua_make_readonly (L, -1); // make time read only.
   lua_pop (L, 1); // dmz.time table.

   LUA_END_VALIDATE (L, 0);
}
//! \endcond

