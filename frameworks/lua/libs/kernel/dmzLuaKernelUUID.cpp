#include <dmzLuaKernel.h>
#include "dmzLuaKernelPrivate.h"
#include <dmzLuaKernelValidate.h>
#include <dmzSystem.h>
#include <dmzTypesHashTableUUIDTemplate.h>
#include <dmzTypesString.h>
#include <dmzTypesUUID.h>
#include <luacpp.h>

using namespace dmz;

namespace {

static const char UUIDName[] = "dmz.types.uuid";
static const char UUIDTableName[] = "dmz.types.uuid.table";
static const char UUIDTableKey = 'u';
static const char LuaUUIDTableKey = 'l';

typedef HashTableUUIDTemplate<int> UTable;

static UTable *
get_utable (lua_State *L) {

   lua_pushlightuserdata (L, (void *)&UUIDTableKey);
   lua_rawget(L, LUA_REGISTRYINDEX);
   UTable **ptr = (UTable **)lua_touserdata (L, -1);
   lua_pop (L, 1); // pop user data

   return ptr ? *ptr : 0;
}


static int
utable_delete (lua_State *L) {

   UTable **ptr = (UTable **)lua_touserdata (L, 1);
   if (ptr && *ptr) { delete (*ptr); *ptr = 0; }

   return 0;
}


inline UUID*
uuid_check (lua_State *L, int index) {

   if (index < 0) { index = lua_gettop (L) + index + 1; }

   UUID *result (0);

   if (lua_isstring (L, index)) {

      String uuidStr = lua_tostring (L, index);

      if (uuidStr) {

         UUID value;

         if (value.from_string (uuidStr)) { result = lua_create_uuid (L, value); }
         else {

            String error ("Malformed UUID string: ");
            error << uuidStr;
            lua_pushstring (L, error.get_buffer ());
            lua_error (L);
         }
      }
   }
   else {

      UUID **ptr = (UUID **)luaL_checkudata (L, index, UUIDName);

      if (ptr) { result = *ptr; }
   }

   return result;
}


static int
uuid_new (lua_State *L) {

   int result (0);

   const int StackCount (lua_gettop (L));

   if (!StackCount) {

      UUID value;

      if (create_uuid (value)) {

         result = (lua_create_uuid (L, value) ? 1 : 0);
      }
   }
   else {

      lua_pushvalue (L, 1);

      result = (uuid_check (L, -1) ? 1 : 0);
   }

   return result;
}


static int
uuid_is_a (lua_State *L) {

   if (lua_to_uuid (L, 1)) { lua_pushvalue (L, 1); }
   else { lua_pushnil (L); }

   return 1;
}


static const luaL_Reg arrayFunc [] = {
   {"new", uuid_new},
   {"is_a", uuid_is_a},
   {NULL, NULL},
};


static int
uuid_to_string (lua_State *L) {

   int result (0);

   UUID *uuid = uuid_check (L, 1);

   if (uuid) {

      String str (uuid->to_string ());
      lua_pushstring (L, str.get_buffer ());
      result = 1;
   }

   return result;
}


static int
uuid_equal (lua_State *L) {

   int result (0);

   UUID *uuid1 = uuid_check (L, 1);
   UUID *uuid2 = uuid_check (L, 2);

   if (uuid1 && uuid2) {

      lua_pushboolean (L, (uuid1 == uuid2 ? 1 : 0));
      result = 1;
   }

   return result;
}


static int
uuid_delete (lua_State *L) {

   LUA_START_VALIDATE (L);

   UUID **uuid = (UUID **)luaL_checkudata (L, 1, UUIDName);
   UTable *ut (get_utable (L));

   if (uuid && *uuid) {

      lua_pushlightuserdata (L, (void *)&LuaUUIDTableKey);
      lua_rawget (L, LUA_REGISTRYINDEX);
      const int Table (lua_gettop (L));

      if (lua_istable (L, Table) && ut) {

         int *indexPtr = ut->lookup (**uuid);

         if (indexPtr) {

            lua_rawgeti (L, Table, *indexPtr);
            UUID *value = (UUID *)lua_touserdata (L, -1);

            if (!value) {

               if (ut->remove (**uuid)) { delete indexPtr; indexPtr = 0; }
            }

            lua_pop (L, 1); // pop UUID
         }
      }

      lua_pop (L, 1); // pop UUID table

      delete (*uuid); *uuid = 0;
   }

   LUA_END_VALIDATE (L, 0);

   return 0;
}


static const luaL_Reg arrayMembers [] = {
   {"__tostring", uuid_to_string},
   {"__eq", uuid_equal},
   {"__gc", uuid_delete},
   {NULL, NULL},
};

};


//! \cond
void
dmz::open_lua_kernel_uuid_lib (lua_State *L) {

   LUA_START_VALIDATE (L);

   lua_pushlightuserdata (L, (void *)&UUIDTableKey);
   UTable **utPtr = (UTable **)lua_newuserdata (L, sizeof (UTable *));

   if (utPtr) {

      lua_set_gc (L, -1, utable_delete);
      *utPtr = new UTable;
      lua_rawset (L, LUA_REGISTRYINDEX);
   }
   else { lua_pop (L, 1); } // pop light user data

   lua_pushlightuserdata (L, (void *)&LuaUUIDTableKey);
   lua_newtable (L);
   lua_set_weak_table (L, -1, "v");
   lua_rawset (L, LUA_REGISTRYINDEX);

   luaL_newmetatable (L, UUIDName);
   luaL_register (L, NULL, arrayMembers);
   lua_pushvalue (L, -1);
   lua_setfield (L, -2, "__index");
   lua_create_dmz_namespace (L, "uuid");
   luaL_register (L, NULL, arrayFunc);
   lua_make_readonly (L, -1); // make uuid read only.
   lua_pop (L, 2); // pops meta table and dmz.uuid table.

   LUA_END_VALIDATE (L, 0);
}
//! \endcond


//! \addtogroup Lua
//! @{

//! Creates a UUID object on the Lua stack.
dmz::UUID *
dmz::lua_create_uuid (lua_State *L, const UUID &Value) {

   LUA_START_VALIDATE (L);

   UUID *result (0);

   UTable *ut (get_utable (L));
   lua_pushlightuserdata (L, (void *)&LuaUUIDTableKey);
   lua_rawget (L, LUA_REGISTRYINDEX);
   const int Table (lua_gettop (L));

   if (lua_istable (L, Table) && Value && ut) {

      int *indexPtr = ut->lookup (Value);

      if (indexPtr) {

         lua_rawgeti (L, Table, *indexPtr);
         result = (UUID *)lua_touserdata (L, -1);
         if (!result) { lua_pop (L, 1); } // pop nil
         else if (*result != Value) {

            lua_pop (L, 1); // pop invalid uuid
            result = 0;
         }
      }

      if (!result) {

         if (indexPtr && ut->remove (Value)) { delete indexPtr; indexPtr = 0; }

         UUID **ptr = (UUID **)lua_newuserdata (L, sizeof (UUID *));

         if (ptr) {

            lua_pushvalue (L, -1);
            int index = luaL_ref (L, Table);

            indexPtr = new int (index);

            if (!ut->store (Value, indexPtr)) { delete indexPtr; indexPtr = 0; }

            *ptr = new UUID (Value);

            result = *ptr;

            luaL_getmetatable (L, UUIDName);
            lua_setmetatable (L, -2);
         }
      }
   }

   lua_remove (L, Table);

   LUA_END_VALIDATE (L, (result ? 1 : 0));

   return result;
}


//! Attempts to convert the specified object on the Lua stack to a UUID object.
dmz::UUID *
dmz::lua_to_uuid (lua_State *L, int narg) {

   UUID *result (0);
   UUID **value = (UUID **) lua_is_object (L, narg, UUIDName);
   if (value) { result = *value; }

   return result;
}


//! Raises and error if the specified object on the Lua stack is not a UUID object.
dmz::UUID *
dmz::lua_check_uuid (lua_State *L, int narg) {

   return uuid_check (L, narg);
}
//! @}
