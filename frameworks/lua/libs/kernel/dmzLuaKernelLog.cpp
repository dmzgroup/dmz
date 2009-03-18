#include <dmzLuaKernel.h>
#include "dmzLuaKernelPrivate.h"
#include <dmzLuaKernelValidate.h>
#include <dmzSystem.h>
#include <dmzTypesString.h>
#include <dmzRuntimeLog.h>
#include <luacpp.h>

using namespace dmz;

namespace {

static const char LogName[] = "dmz.runtime.log";


inline Log**
log_check (lua_State *L, int index) {

   return (Log **)luaL_checkudata (L, index, LogName);
}


static Log *
log_create (lua_State *L, const String &Name) {

   Log *result (0);

   Log **ptr = (Log **)lua_newuserdata (L, sizeof (Log *));

   if (ptr) {

      *ptr = 0;

      luaL_getmetatable (L, LogName);
      lua_setmetatable (L, -2);

      *ptr = new Log (Name, lua_get_runtime_context (L));

      result = *ptr;
   }

   return result;
}


static int
log_new (lua_State *L) {

   int result (0);

   int stackCount = lua_gettop (L);

   String name ("LuaLog");

   if (0 == stackCount) {

      // do nothing
   }
   else if (1 == stackCount) {

      String value ((luaL_checkstring (L, 1)));
      name = value;
   }
   else { luaL_error (L, "Unsupported parameters."); }

   Log *ptr = log_create (L, name);

   if (ptr) { result = 1; }

   return result;
}


static int
log_is_a (lua_State *L) {

   if (lua_to_log (L, 1)) { lua_pushvalue (L, 1); }
   else { lua_pushnil (L); }

   return 1;
}


static const luaL_Reg arrayFunc [] = {
   {"new", log_new},
   {"is_a", log_is_a},
   {NULL, NULL},
};


static void
log_dump (lua_State *L, Stream &stream) {

   int stackCount = lua_gettop (L);

   for (int ix = 2; ix <= stackCount; ix++) {

      const char *ptr = lua_tostring (L, ix);

      if (ix != 2) { stream << " "; }

      if (ptr) {

         stream << ptr;
      }
      else {

         if (lua_getmetatable(L, ix)) {

            lua_getfield (L, -1, "__tostring");

            if (lua_isfunction (L, -1)) {

               lua_pushvalue (L, ix);

               lua_call (L, 1, 1);

               if (lua_isstring (L, -1)) {

                  stream << lua_tostring (L, -1);
               }

              lua_pop (L, 2);
            }
            else { stream << "<UNPRINTABLE>"; }
         }
      }
   }

   stream << endl;
}


static int
log_out (lua_State *L) {

   Log **log = log_check (L, 1);

   if (log && *log) { log_dump (L, (*log)->out); }

   return 0;
}


static int
log_error (lua_State *L) {

   Log **log = log_check (L, 1);

   if (log && *log) { log_dump (L, (*log)->error); }

   return 0;
}


static int
log_warn (lua_State *L) {

   Log **log = log_check (L, 1);

   if (log && *log) { log_dump (L, (*log)->warn); }

   return 0;
}


static int
log_info (lua_State *L) {

   Log **log = log_check (L, 1);

   if (log && *log) { log_dump (L, (*log)->info); }

   return 0;
}


static int
log_debug (lua_State *L) {

   Log **log = log_check (L, 1);

   if (log && *log) { log_dump (L, (*log)->debug); }

   return 0;
}


static int
log_delete (lua_State *L) {

   Log **log = log_check (L, 1);
   if (log && *log) { delete (*log); *log = 0; }

   return 0;
}


static const luaL_Reg arrayMembers[] = {
   {"out", log_out},
   {"error", log_error},
   {"warn", log_warn},
   {"info", log_info},
   {"debug", log_debug},
   {"__gc", log_delete},
   {NULL, NULL},
};

};


//! \cond
void
dmz::open_lua_kernel_log_lib (lua_State *L) {

   LUA_START_VALIDATE (L);

   luaL_newmetatable (L, LogName);
   luaL_register (L, NULL, arrayMembers);
   lua_pushvalue (L, -1);
   lua_setfield (L, -2, "__index");
   lua_create_dmz_namespace (L, "log");
   luaL_register (L, NULL, arrayFunc);
   lua_make_readonly (L, -1); // make log read only.
   lua_pop (L, 2); // pops meta table and dmz.log table.

   LUA_END_VALIDATE (L, 0);
}
//! \endcond


//! \addtogroup Lua
//! @{

//! Attempts to convert the specified object on the stack to a Log.
dmz::Log *
dmz::lua_check_log (lua_State *L, int narg) {

   Log *result (0);
   Log **value = log_check (L, narg);
   if (value) { result = *value; }

   return result;
}


//! Raises an error if the specified object on the stack is not a Log.
dmz::Log *
dmz::lua_to_log (lua_State *L, int index) {

   Log *result (0);

   Log **value = (Log **)lua_touserdata (L, index);

   if (value) {

      if (lua_getmetatable(L, index)) {

         lua_getfield(L, LUA_REGISTRYINDEX, LogName);

         if (lua_rawequal(L, -1, -2)) {

            lua_pop(L, 2);

            result = *value;
         }
      }
   }

   return result;
}
//! @}
