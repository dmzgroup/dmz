#include <dmzLuaKernel.h>
#include "dmzLuaKernelPrivate.h"
#include <dmzLuaKernelValidate.h>
#include <dmzRuntimeData.h>
#include <dmzRuntimeUndo.h>
#include <dmzTypesHashTableHandleTemplate.h>
#include <dmzTypesString.h>
#include <luacpp.h>

using namespace dmz;

namespace {

static const char UndoName[] = "dmz.runtime.undo";

static int LuaUndoUndo = 0;
static int LuaUndoRedo = 0;

static UndoTypeEnum
to_undo_type (lua_State *L, int index) {

   UndoTypeEnum result (UndoTypeUndo);

   const int LuaType = luaL_checkinteger (L, index);
   if (LuaType == LuaUndoUndo) { result = UndoTypeUndo; }
   else if (LuaType == LuaUndoRedo) { result = UndoTypeRedo; }
   else { lua_enum_error (L, index, LuaType); }

   return result;
}


static int
undo_is_recording (lua_State *L) {

   Undo undo (lua_get_runtime_context (L));
   lua_pushboolean (L, undo.is_recording () ? 1 : 0);

   return 1;
}


static int
undo_do_next (lua_State *L) {

   Undo undo (lua_get_runtime_context (L));
   lua_pushboolean (L, undo.do_next (to_undo_type (L, 1)) ? 1 : 0);

   return 1;
}


static int
undo_start_record (lua_State *L) {

   const String Name (luaL_checkstring (L, 1));

   Undo undo (lua_get_runtime_context (L));
   lua_create_handle (L, undo.start_record (Name));

   return 1;
}


static int
undo_store_action (lua_State *L) {

   int result (0);

   Message *type (lua_check_message (L, 1));

   Handle *ptr (0);

   if (!lua_isnoneornil (L, 2)) { ptr = lua_check_handle (L, 2); }

   Data *data (0);

   if (!lua_isnoneornil (L, 3)) { data = lua_check_data (L, 3); }

   if (type) {

      Undo undo (lua_get_runtime_context (L));

      lua_pushboolean (L, undo.store_action (*type, ptr ? *ptr : 0, data));
      result = 1;
   }

   return result;
}


static int
undo_stop_record (lua_State *L) {

   int result (0);

   Handle *ptr = lua_check_handle (L, 1);

   if (ptr) {

      Undo undo (lua_get_runtime_context (L));

      lua_pushboolean (L, undo.stop_record (*ptr));
      result = 1;
   }

   return result;
}


static int
undo_abort_record (lua_State *L) {

   int result (0);

   Handle *ptr = lua_check_handle (L, 1);

   if (ptr) {

      Undo undo (lua_get_runtime_context (L));

      lua_pushboolean (L, undo.abort_record (*ptr));
      result = 1;
   }

   return result;
}


static const luaL_Reg arrayFunc[] = {
   {"is_recording", undo_is_recording},
   {"do_next", undo_do_next},
   {"start_record", undo_start_record},
   {"store_action", undo_store_action},
   {"stop_record", undo_stop_record},
   {"abort_record", undo_abort_record},
   {NULL, NULL},
};

};


//! \cond
void
dmz::open_lua_kernel_undo_lib (lua_State *L) {

   LUA_START_VALIDATE (L);

   lua_create_dmz_namespace (L, "undo");
   luaL_register (L, NULL, arrayFunc);

   LuaUndoUndo = lua_create_enum (L, "dmzUndoTypeUndo");
   LuaUndoRedo = lua_create_enum (L, "dmzUndoTypeRedo");

   lua_pushinteger (L, LuaUndoUndo);
   lua_setfield (L, -2, "Undo");
   lua_pushinteger (L, LuaUndoRedo);
   lua_setfield (L, -2, "Redo");
   lua_make_readonly (L, -1); // make undo read only.
   lua_pop (L, 1); // pops dmz.undo table.

   LUA_END_VALIDATE (L, 0);
}
//! \endcond

