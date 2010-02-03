#include <dmzLuaKernel.h>
#include "dmzLuaKernelPrivate.h"
#include <dmzLuaKernelValidate.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzSystem.h>
#include <dmzTypesHandleContainer.h>
#include <dmzTypesHashTableHandleTemplate.h>
#include <dmzTypesString.h>
#include <luacpp.h>

using namespace dmz;

namespace {

static const char HandleName[] = "dmz.types.handle";
static const char HandleTableName[] = "dmz.types.handle.table";
static const char HandleTableKey = 'h';
static const char LuaHandleTableKey = 'l';

typedef HashTableHandleTemplate<int> HandleTable;


static HandleTable *
get_handle_table (lua_State *L) {

   lua_pushlightuserdata (L, (void *)&HandleTableKey);
   lua_rawget(L, LUA_REGISTRYINDEX);
   HandleTable **ptr = (HandleTable **)lua_touserdata (L, -1);
   lua_pop (L, 1); // pop user data

   return ptr ? *ptr : 0;
}

static int
handle_table_delete (lua_State *L) {

   HandleTable **ptr = (HandleTable **)lua_touserdata (L, 1);
   if (ptr && *ptr) { delete (*ptr); *ptr = 0; }

   return 0;
}


inline Handle*
handle_check (lua_State *L, int index) {

   LUA_START_VALIDATE (L);

   if (index < 0) { index = lua_gettop (L) + index + 1; }

   Handle *result (0);

   if (lua_isnumber (L, index)) {

      Handle value = (Handle)lua_tonumber (L, index);

      if (value) {

         result = lua_create_handle (L, value);
         lua_replace (L, index); // Replace string with handle
      }
   }
   else if (lua_isstring (L, index)) {

      String name = lua_tostring (L, index);

      if (name) {

         Definitions def (lua_get_runtime_context (L));

         Handle value = def.create_named_handle (name);

         result = lua_create_handle (L, value);

         lua_replace (L, index); // Replace string with handle
      }
      else {

         lua_pushstring (L, "Empty string can not be converted to named handle");
         lua_error (L);
      }
   }
   else { result = (Handle *)luaL_checkudata (L, index, HandleName); }

   LUA_END_VALIDATE (L, 0);

   return result;
}


static int
handle_new (lua_State *L) {

   lua_pushvalue (L, 1);

   return handle_check (L, -1) ? 1 : 0;
}


static int
handle_is_a (lua_State *L) {

   if (lua_to_handle (L, 1)) { lua_pushvalue (L, 1); }
   else { lua_pushnil (L); }

   return 1;
}


static int
handle_name (lua_State *L) {

   int result (0);

   Handle *ptr = lua_to_handle (L, 1);

   Handle value (ptr ? *ptr : 0);

   if (!value) { value = (Handle)lua_tointeger (L, 1); }

   if (value) {

      Definitions def (lua_get_runtime_context (L));
      const String Name = def.lookup_named_handle_name (value);

      if (Name) {

         lua_pushstring (L, Name.get_buffer ());
         result = 1;
      }
   }

   return result;
}


static const luaL_Reg arrayFunc [] = {
   {"new", handle_new},
   {"is_a", handle_is_a},
   {"name", handle_name},
   {NULL, NULL},
};


static int
handle_to_string (lua_State *L) {

   LUA_START_VALIDATE (L);

   int result (0);

   Handle *handle = handle_check (L, 1);

   if (handle) {

      String str;
      str << *handle;
      lua_pushstring (L, str.get_buffer ());
      result = 1;
   }

   LUA_END_VALIDATE (L, result);

   return result;
}


static int
handle_equal (lua_State *L) {

   int result (0);

   Handle *handle1 = handle_check (L, 1);
   Handle *handle2 = handle_check (L, 2);

   if (handle1 && handle2) {

      lua_pushboolean (L, (handle1 == handle2 ? 1 : 0));
      result = 1;
   }

   return result;
}


static int
handle_delete (lua_State *L) {

   LUA_START_VALIDATE (L);

   Handle *ptr = handle_check (L, 1);
   HandleTable *ht = get_handle_table (L);

   if (ptr && ht) {

      lua_pushlightuserdata (L, (void *)&LuaHandleTableKey);
      lua_rawget (L, LUA_REGISTRYINDEX);
      const int Table (lua_gettop (L));

      if (lua_istable (L, Table)) {

         int *indexPtr = ht->lookup (*ptr);

         if (indexPtr) {

            lua_rawgeti (L, Table, *indexPtr);
            Handle *found = (Handle *)lua_touserdata (L, -1);

            if (!found) {

               if (ht->remove (*ptr)) { delete indexPtr; indexPtr = 0; }
            }

            lua_pop (L, 1); // pop Handle;
         }
      }

      lua_pop (L, 1); // pop Handle table
   }

   LUA_END_VALIDATE (L, 0);

   return 0;
}


static const luaL_Reg arrayMembers [] = {
   {"__tostring", handle_to_string},
   {"__eq", handle_equal},
   {"__gc", handle_delete},
   {NULL, NULL},
};

};


//! \cond
void
dmz::open_lua_kernel_handle_lib (lua_State *L) {

   LUA_START_VALIDATE (L);

   lua_pushlightuserdata (L, (void *)&HandleTableKey);

   HandleTable **htPtr = (HandleTable **)lua_newuserdata (L, sizeof (HandleTable *));

   if (htPtr) {

      lua_set_gc (L, -1, handle_table_delete);
      *htPtr = new HandleTable;
      lua_rawset (L, LUA_REGISTRYINDEX);
   }
   else { lua_pop (L, 1); } // pop light user data

   lua_pushlightuserdata (L, (void *)&LuaHandleTableKey);
   lua_newtable (L);
   lua_set_weak_table (L, -1, "v");
   lua_rawset (L, LUA_REGISTRYINDEX);

   luaL_newmetatable (L, HandleName);
   luaL_register (L, NULL, arrayMembers);
   lua_pushvalue (L, -1);
   lua_setfield (L, -2, "__index");
   lua_create_dmz_namespace (L, "handle");
   luaL_register (L, NULL, arrayFunc);
   lua_make_readonly (L, -1); // make handle read only.
   lua_pop (L, 2); // pops meta table and dmz.handle table.

   LUA_END_VALIDATE (L, 0);
}
//! \endcond

//! \addtogroup Lua
//! @{

//! Creates a Handle on the Lua stack.
dmz::Handle *
dmz::lua_create_handle (lua_State *L, const Handle Value) {

   LUA_START_VALIDATE (L);

   Handle *result = 0;

   if (Value) {

      HandleTable *ht (get_handle_table (L));
      lua_pushlightuserdata (L, (void *)&LuaHandleTableKey);
      lua_rawget (L, LUA_REGISTRYINDEX);
      const int Table (lua_gettop (L));

      if (lua_istable (L, Table) && ht) {

         int *indexPtr = ht->lookup (Value);

         if (indexPtr) {

            lua_rawgeti (L, Table, *indexPtr);
            result = (Handle *)lua_touserdata (L, -1);
            if (!result) { lua_pop (L, 1); }
            else if (*result != Value) {

               lua_pop (L, 1); // pop invalid Handle;
               result = 0;
            }
         }

         if (!result) {

            if (indexPtr && ht->remove (Value)) { delete indexPtr; indexPtr = 0; }

            result = (Handle *)lua_newuserdata (L, sizeof (Handle));

            if (result) {

               lua_pushvalue (L, -1);
               int index = luaL_ref (L, Table);

               indexPtr = new int (index);

               if (!ht->store (Value, indexPtr)) { delete indexPtr; indexPtr = 0; }

               *result = Value;

               luaL_getmetatable (L, HandleName);
               lua_setmetatable (L, -2);
            }
         }
      }

      lua_remove (L, Table); // Remove Table;
   }

   if (!result) { lua_pushnil (L); }

   LUA_END_VALIDATE (L, 1);

   return result;
}


//! Attempts to convert the specified object on the Lua stack to a Handle.
dmz::Handle *
dmz::lua_to_handle (lua_State *L, int narg) {

   return (Handle *) lua_is_object (L, narg, HandleName);
}


//! Raises and error if the specified object on the stack is not a Handle.
dmz::Handle *
dmz::lua_check_handle (lua_State *L, int narg) { return handle_check (L, narg); }


//! Converts a HandleContainer to a Lua table on the Lua stack.
void
dmz::lua_handle_container_to_table (lua_State *L, const HandleContainer &Container) {

   LUA_START_VALIDATE (L);

   lua_createtable (L, (int)Container.get_count (), 0);
   const int Table (lua_gettop (L));

   int count (1);

   Handle value = Container.get_first ();

   while (value) {

      if (lua_create_handle (L, value)) {

         lua_rawseti (L, Table, count);
         count++;
      }

      if (lua_create_handle (L, value)) {

         lua_pushboolean (L, 1);
         lua_rawset (L, Table);
      }

      value = Container.get_next ();
   }

   LUA_END_VALIDATE (L, 1);
}

//! @}
