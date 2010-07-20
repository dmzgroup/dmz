#include <dmzLuaKernel.h>
#include "dmzLuaKernelPrivate.h"
#include <dmzLuaKernelValidate.h>
#include <dmzRuntimeData.h>
#include <dmzRuntimeIterator.h>
#include <dmzSystemStreamString.h>
#include <dmzTypesString.h>
#include <dmzTypesVector.h>
#include <luacpp.h>

using namespace dmz;

namespace {

static const char DataName[] = "dmz.runtime.data";

inline Data**
data_check (lua_State *L, int index) {

   return (Data **)luaL_checkudata (L, index, DataName);
}


static int
data_new (lua_State *L) {

   int result (0);

   int stackCount = lua_gettop (L);

   Data *ptr = lua_create_data (L);

   if (ptr) {

      result = 1;

      if (0 == stackCount) {

         // do nothing
      }
      else if (1 == stackCount) {

         if (lua_isuserdata (L, 1)) {

            Data **dataPtr = data_check (L, 1);

            if (dataPtr && *dataPtr) { *ptr = **dataPtr; }
         }
         else { luaL_error (L, "Unsupported parameter."); }
      }
      else {

         luaL_error (L, "Too many parameters.");
      }
   }

   return result;
}


static int
data_is_a (lua_State *L) {

   if (lua_to_data (L, 1)) { lua_pushvalue (L, 1); }
   else { lua_pushnil (L); }

   return 1;
}


static const luaL_Reg arrayFunc [] = {
   {"new", data_new},
   {"is_a", data_is_a},
   {NULL, NULL},
};


static int
data_to_string (lua_State *L) {

   int result (0);

   Data **data = data_check (L, 1);

   if (data && *data) {

      String str;
      StreamString stream (str);
      stream << **data;
      lua_pushstring (L, str.get_buffer ());
      result = 1;
   }

   return result;
}


static int
data_equal (lua_State *L) {

   int result (0);

   Data **data1 = data_check (L, 1);
   Data **data2 = data_check (L, 2);

   if (data1 && data2 && *data1 && *data2) {

      lua_pushboolean (L, **data1 == **data2 ? 1 : 0);
      result = 1;
   }

   return result;
}


static int
data_get_attributes (lua_State *L) {

   LUA_START_VALIDATE (L);

   int result (0);

   Data **dataPtr = data_check (L, 1);

   if (dataPtr && *dataPtr) {

      Data &data = **dataPtr;

      const int Size (data.get_attribute_count ());

      lua_createtable (L, Size, 0);

      RuntimeIterator it;
      int index (1);

      for (
            Handle handle = data.get_first_attribute (it);
            handle;
            handle = data.get_next_attribute (it)) {

         Handle *ptr = lua_create_handle (L, handle);

         if (ptr) { lua_rawseti (L, -2, index); index++; }
         else { lua_pop (L, 1); } // pop nil
      }

      result = 1;
   }

   LUA_END_VALIDATE (L, 1);

   return result;
}


static int
data_store_boolean (lua_State *L) {

   Data **data = data_check (L, 1);
   Handle *handlePtr = lua_check_handle (L, 2);
   Int32 index = (Int32)luaL_checkinteger (L, 3) - 1;
   Boolean value = lua_toboolean (L, 4) > 0 ? True : False;

   if (data && *data && handlePtr) {

      (*data)->store_boolean (*handlePtr, index, value);
   }

   return 0;
}


static int
data_lookup_boolean (lua_State *L) {

   int result (0);

   Data **data = data_check (L, 1);
   Handle *handlePtr = lua_check_handle (L, 2);
   Int32 index = (Int32)luaL_checkinteger (L, 3) - 1;

   if (data && *data && handlePtr) {

      Boolean value;

      if ((*data)->lookup_boolean (*handlePtr, index, value)) {

         lua_pushboolean (L, value ? 1 : 0);
         result = 1;
      }
   }

   return result;
}


static int
data_store_string (lua_State *L) {

   Data **data = data_check (L, 1);
   Handle *handlePtr = lua_check_handle (L, 2);
   Int32 index = (Int32)luaL_checkinteger (L, 3) - 1;
   String value = luaL_checkstring (L, 4);

   if (data && *data && handlePtr) {

      (*data)->store_string (*handlePtr, index, value);
   }

   return 0;
}


static int
data_lookup_string (lua_State *L) {

   int result (0);

   Data **data = data_check (L, 1);
   Handle *handlePtr = lua_check_handle (L, 2);
   Int32 index = (Int32)luaL_checkinteger (L, 3) - 1;

   if (data && *data && handlePtr) {

      String value;

      if ((*data)->lookup_string (*handlePtr, index, value)) {

         lua_pushstring (L, value.get_buffer ());
         result = 1;
      }
   }

   return result;
}


static int
data_store_number (lua_State *L) {

   Data **data = data_check (L, 1);
   Handle *handlePtr = lua_check_handle (L, 2);
   Int32 index = (Int32)luaL_checkinteger (L, 3) - 1;
   Float32 value = (Float32)luaL_checknumber (L, 4);

   if (data && *data && handlePtr) {

      (*data)->store_float64 (*handlePtr, index, value);
   }

   return 0;
}


static int
data_lookup_number (lua_State *L) {

   int result (0);

   Data **data = data_check (L, 1);
   Handle *handlePtr = lua_check_handle (L, 2);
   Int32 index = (Int32)luaL_checkinteger (L, 3) - 1;

   if (data && *data && handlePtr) {

      Float64 value (0.0);

      if ((*data)->lookup_float64 (*handlePtr, index, value)) {

         lua_pushnumber (L, value);
         result = 1;
      }
   }

   return result;
}


static int
data_store_handle (lua_State *L) {

   Data **data = data_check (L, 1);
   Handle *handlePtr = lua_check_handle (L, 2);
   Int32 index = (Int32)luaL_checkinteger (L, 3) - 1;
   Handle *value = lua_check_handle (L, 4);

   if (data && *data && handlePtr && value) {

      (*data)->store_handle (*handlePtr, index, *value);
   }

   return 0;
}


static int
data_lookup_handle (lua_State *L) {

   int result (0);

   Data **data = data_check (L, 1);
   Handle *handlePtr = lua_check_handle (L, 2);
   Int32 index = (Int32)luaL_checkinteger (L, 3) - 1;

   if (data && *data && handlePtr) {

      Handle value (0);

      if ((*data)->lookup_handle (*handlePtr, index, value)) {

         Handle *ptr = lua_create_handle (L, value);

         if (ptr) { result = 1; }
         else { lua_pop (L, 1); } // pop nil
      }
   }

   return result;
}


static int
data_store_vector (lua_State *L) {

   Data **data = data_check (L, 1);
   Handle *handlePtr = lua_check_handle (L, 2);
   Int32 index = (Int32)luaL_checkinteger (L, 3) - 1;
   Vector *value = lua_check_vector (L, 4);

   if (data && *data && handlePtr && value) {

      (*data)->store_vector (*handlePtr, index, *value);
   }

   return 0;
}


static int
data_lookup_vector (lua_State *L) {

   int result (0);

   Data **data = data_check (L, 1);
   Handle *handlePtr = lua_check_handle (L, 2);
   Int32 index = (Int32)luaL_checkinteger (L, 3) - 1;

   if (data && *data && handlePtr) {

      Vector value;

      if ((*data)->lookup_vector (*handlePtr, index, value)) {

         Vector *ptr = lua_create_vector (L, &value);

         if (ptr) { result = 1; }
      }
   }

   return result;
}


static int
data_delete (lua_State *L) {

   Data **data = data_check (L, 1);
   if (data && *data) { delete (*data); *data = 0; }

   return 0;
}


static const luaL_Reg arrayMembers [] = {
   {"__tostring", data_to_string},
   {"__eq", data_equal},
   {"get_attributes", data_get_attributes},
   {"store_boolean", data_store_boolean},
   {"lookup_boolean", data_lookup_boolean},
   {"store_string", data_store_string},
   {"lookup_string", data_lookup_string},
   {"store_number", data_store_number},
   {"lookup_number", data_lookup_number},
   {"store_handle", data_store_handle},
   {"lookup_handle", data_lookup_handle},
   {"store_vector", data_store_vector},
   {"lookup_vector", data_lookup_vector},
   {"__gc", data_delete},
   {NULL, NULL},
};

};


//! \cond
void
dmz::open_lua_kernel_data_lib (lua_State *L) {

   LUA_START_VALIDATE (L);

   luaL_newmetatable (L, DataName);
   luaL_register (L, NULL, arrayMembers);
   lua_pushvalue (L, -1);
   lua_setfield (L, -2, "__index");
   lua_create_dmz_namespace (L, "data");
   luaL_register (L, NULL, arrayFunc);
   lua_make_readonly (L, -1); // make data read only.
   lua_pop (L, 2); // pops meta table and dmz.data table.

   LUA_END_VALIDATE (L, 0);
}
//! \endcond

//! \addtogroup Lua
//! @{

//! Creates a Data object on the Lua stack.
dmz::Data *
dmz::lua_create_data (lua_State *L, const Data *Value) {

   Data *result (0);

   Data **ptr = (Data **)lua_newuserdata (L, sizeof (Data *));

   if (ptr) {

      *ptr = new Data (lua_get_runtime_context (L));

       if (*ptr && Value) { (**ptr) = *Value; }

      luaL_getmetatable (L, DataName);
      lua_setmetatable (L, -2);

      result = *ptr;
   }

   return result;
}


//! Attempts to convert specified object on the stack to a Data object.
dmz::Data *
dmz::lua_to_data (lua_State *L, int narg) {

   Data *result (0);
   Data **value = (Data **) lua_is_object (L, narg, DataName);
   if (value) { result = *value; }

   return result;
}


//! Raises an error if the specified object on the stack is no a Data object.
dmz::Data *
dmz::lua_check_data (lua_State *L, int narg) {

   Data *result (0);
   Data **value = data_check (L, narg);
   if (value) { result = *value; }

   return result;
}
//! @}
