#include <dmzLuaKernel.h>
#include "dmzLuaKernelPrivate.h"
#include <dmzLuaKernelValidate.h>
#include <dmzTypesString.h>
#include <dmzTypesVector.h>
#include <luacpp.h>

using namespace dmz;

namespace {

static const char VectorName[] = "dmz.types.vector";

inline Vector**
vector_check (lua_State *L, int index) {

   if (index < 0) { index = lua_gettop (L) + index + 1; }

   Vector **result = 0;

   if (lua_istable (L, index)) {

      lua_rawgeti (L, index, 1);
      lua_rawgeti (L, index, 2);
      lua_rawgeti (L, index, 3);

      lua_Number x = luaL_checknumber (L, -3);
      lua_Number y = luaL_checknumber (L, -2);
      lua_Number z = luaL_checknumber (L, -1);

      Vector *ptr = lua_create_vector (L);

      if (ptr) {

         ptr->set_xyz (x, y, z);
         lua_replace (L, index);
      }

      lua_pop (L, 3);

      result = vector_check (L, index);

   }
   else { result = (Vector **)luaL_checkudata (L, index, VectorName); }

   return result;
}


static int
vector_new (lua_State *L) {

   int result (0);

   const int StackCount = lua_gettop (L);

   Vector *ptr = lua_create_vector (L);

   if (ptr) {

      result = 1;

      if (0 == StackCount) {

         // do nothing
      }
      else if (1 == StackCount) {

         if (lua_isuserdata (L, 1)) {

            Vector **vecPtr = vector_check (L, 1);

            if (vecPtr && *vecPtr) { *ptr = **vecPtr; }
         }
         else if (lua_istable (L, 1)) {

            lua_rawgeti (L, 1, 1);
            lua_rawgeti (L, 1, 2);
            lua_rawgeti (L, 1, 3);

            lua_Number x = luaL_checknumber (L, -3);
            lua_Number y = luaL_checknumber (L, -2);
            lua_Number z = luaL_checknumber (L, -1);

            ptr->set_xyz (x, y, z);

            lua_pop (L, 3);
         }
         else { luaL_error (L, "Unsupported parameter."); }
      }
      else if (3 == StackCount) {

         lua_Number x = luaL_checknumber (L, 1);
         lua_Number y = luaL_checknumber (L, 2);
         lua_Number z = luaL_checknumber (L, 3);

         ptr->set_xyz (x, y, z);
      }
      else {

         luaL_error (L, "Too many parameters.");
      }
   }

   return result;
}


static int
vector_is_a (lua_State *L) {

   if (lua_to_vector (L, 1)) { lua_pushvalue (L, 1); }
   else { lua_pushnil (L); }

   return 1;
}


static int
vector_right (lua_State *L) {

   const Vector Value (1.0, 0.0, 0.0);
   return lua_create_vector (L, &Value) ? 1 : 0;
}


static int
vector_left (lua_State *L) {

   const Vector Value (-1.0, 0.0, 0.0);
   return lua_create_vector (L, &Value) ? 1 : 0;
}


static int
vector_up (lua_State *L) {

   const Vector Value (0.0, 1.0, 0.0);
   return lua_create_vector (L, &Value) ? 1 : 0;
}


static int
vector_down (lua_State *L) {

   const Vector Value (0.0, -1.0, 0.0);
   return lua_create_vector (L, &Value) ? 1 : 0;
}


static int
vector_backward (lua_State *L) {

   const Vector Value (0.0, 0.0, 1.0);
   return lua_create_vector (L, &Value) ? 1 : 0;
}


static int
vector_forward (lua_State *L) {

   const Vector Value (0.0, 0.0, -1.0);
   return lua_create_vector (L, &Value) ? 1 : 0;
}


static const luaL_Reg arrayFunc [] = {
   {"new", vector_new},
   {"is_a", vector_is_a},
   {"right", vector_right},
   {"left", vector_left},
   {"up", vector_up},
   {"down", vector_down},
   {"backward", vector_backward},
   {"forward", vector_forward},
   {NULL, NULL},
};


static int
vector_to_string (lua_State *L) {

   int result (0);

   Vector **vec = vector_check (L, 1);

   if (vec && *vec) {

      String str;
      str << "{"
         << (*vec)->get_x () << ", "
         << (*vec)->get_y () << ", "
         << (*vec)->get_z () << "}";

      lua_pushstring (L, str.get_buffer ());
      result = 1;
   }

   return result;
}


static int
vector_add (lua_State *L) {

   int result (0);

   Vector **vec1 = vector_check (L, 1);
   Vector **vec2 = vector_check (L, 2);

   if (vec1 && vec2 && *vec1 && *vec2) {

      Vector value (**vec1 + **vec2);
      if (lua_create_vector (L, &value)) { result = 1; }
   }

   return result;
}


static int
vector_subtract (lua_State *L) {

   int result (0);

   Vector **vec1 = vector_check (L, 1);
   Vector **vec2 = vector_check (L, 2);

   if (vec1 && vec2 && *vec1 && *vec2) {

      Vector value (**vec1 - **vec2);
      if (lua_create_vector (L, &value)) { result = 1; }
   }

   return result;
}


static int
vector_multiply (lua_State *L) {

   int result (0);

   Vector **vec = vector_check (L, 1);

   if (lua_isuserdata (L, 2)) {

      Vector **vec2 = vector_check (L, 2);

      if (vec2 && *vec2) {

         Vector value ((**vec) * (**vec2));
         if (lua_create_vector (L, &value)) { result = 1; }
      }
   }
   else {

      lua_Number value (luaL_checknumber (L, 2));

      Vector vec2 ((**vec) * value);
      if (lua_create_vector (L, &vec2)) { result = 1; }
   }

   return result;
}


static int
vector_equal (lua_State *L) {

   int result (0);

   Vector **vec1 = vector_check (L, 1);
   Vector **vec2 = vector_check (L, 2);

   if (vec1 && vec2 && *vec1 && *vec2) {

      lua_pushboolean (L, (**vec1 == **vec2 ? 1 : 0));
      result = 1;
   }

   return result;
}


static int
vector_unm (lua_State *L) {

   int result (0);

   Vector **vec = vector_check (L, 1);

   if (vec && *vec) {

      Vector value (-(**vec));
      if (lua_create_vector (L, &value)) { result = 1; }
   }

   return result;
}


static int
vector_set_x (lua_State *L) {

   int result (0);

   Vector **vec = vector_check (L, 1);
   lua_Number value (luaL_checknumber (L, 2));

   if (vec && *vec) { (*vec)->set_x (value); lua_pushvalue (L, 1); result = 1; }

   return result;
}


static int
vector_set_y (lua_State *L) {

   int result (0);

   Vector **vec = vector_check (L, 1);
   lua_Number value (luaL_checknumber (L, 2));

   if (vec && *vec) { (*vec)->set_y (value); lua_pushvalue (L, 1); result = 1; }

   return result;
}


static int
vector_set_z (lua_State *L) {

   int result (0);

   Vector **vec = vector_check (L, 1);
   lua_Number value (luaL_checknumber (L, 2));

   if (vec && *vec) { (*vec)->set_z (value); lua_pushvalue (L, 1); result = 1; }

   return result;
}

static int
vector_set_xyz (lua_State *L) {

   int result (0);

   Vector **vec = vector_check (L, 1);
   lua_Number x (luaL_checknumber (L, 2));
   lua_Number y (luaL_checknumber (L, 3));
   lua_Number z (luaL_checknumber (L, 4));

   if (vec && *vec) { (*vec)->set_xyz (x, y, z); lua_pushvalue (L, 1); result = 1; }

   return result;
}


static int
vector_get_x (lua_State *L) {

   int result (0);

   Vector **vec = vector_check (L, 1);

   if (vec && *vec) { lua_pushnumber (L, (*vec)->get_x ()); result = 1; }

   return result;
}


static int
vector_get_y (lua_State *L) {

   int result (0);

   Vector **vec = vector_check (L, 1);

   if (vec && *vec) { lua_pushnumber (L, (*vec)->get_y ()); result = 1; }

   return result;
}


static int
vector_get_z (lua_State *L) {

   int result (0);

   Vector **vec = vector_check (L, 1);

   if (vec && *vec) { lua_pushnumber (L, (*vec)->get_z ()); result = 1; }

   return result;
}


static int
vector_get_xyz (lua_State *L) {

   int result (0);

   Vector **vec = vector_check (L, 1);

   if (vec && *vec) {

      lua_pushnumber (L, (*vec)->get_x ());
      lua_pushnumber (L, (*vec)->get_y ());
      lua_pushnumber (L, (*vec)->get_z ());

      result = 3;
   }

   return result;
}


static int
vector_magnitude (lua_State *L) {

   int result (0);

   Vector **vec = vector_check (L, 1);

   if (vec && *vec) {

      lua_pushnumber (L, (*vec)->magnitude ());
      result = 1;
   }

   return result;
}


static int
vector_normalize (lua_State *L) {

   int result (0);

   Vector **vec = vector_check (L, 1);

   if (vec && *vec) {

      Vector value ((*vec)->normalize ());
      if (lua_create_vector (L, &value)) { result = 1; }
   }

   return result;
}


static int
vector_dot (lua_State *L) {

   int result (0);

   Vector **vec1 = vector_check (L, 1);
   Vector **vec2 = vector_check (L, 2);

   if (vec1 && vec2 && *vec1 && *vec2) {

      lua_pushnumber (L, (*vec1)->dot (**vec2));
      result = 1;
   }

   return result;
}


static int
vector_cross (lua_State *L) {

   int result (0);

   Vector **vec1 = vector_check (L, 1);
   Vector **vec2 = vector_check (L, 2);

   if (vec1 && vec2 && *vec1 && *vec2) {

      Vector value = (*vec1)->cross (**vec2);
      if (lua_create_vector (L, &value)) { result = 1; }
   }

   return result;
}


static int
vector_is_zero (lua_State *L) {

   int result (0);

   Vector **vec = vector_check (L, 1);

   if (vec && *vec) {

      const int StackCount = lua_gettop (L);

      if (StackCount == 1) {

         lua_pushboolean (L, (*vec)->is_zero ());
         result = 1;
      }
      else if (StackCount > 1) {

         lua_Number value (luaL_checknumber (L, 2));

         lua_pushboolean (L, (*vec)->is_zero (value));
         result = 1;
      }
   }

   return result;
}


static int
vector_get_angle (lua_State *L) {

   int result (0);

   Vector **vec1 = vector_check (L, 1);
   Vector **vec2 = vector_check (L, 2);

   if (vec1 && vec2 && *vec1 && *vec2) {

      lua_pushnumber (L, (*vec1)->get_angle (**vec2));
      result = 1;
   }

   return result;
}


static int
vector_delete (lua_State *L) {

   Vector **vec = vector_check (L, 1);
   if (vec && *vec) { delete (*vec); *vec = 0; }

   return 0;
}


static const luaL_Reg arrayMembers [] = {
   {"__tostring", vector_to_string},
   {"__add", vector_add},
   {"__sub", vector_subtract},
   {"__mul", vector_multiply},
   {"__eq", vector_equal},
   {"__unm", vector_unm},
   {"set_x", vector_set_x},
   {"set_y", vector_set_y},
   {"set_z", vector_set_z},
   {"set_xyz", vector_set_xyz},
   {"get_x", vector_get_x},
   {"get_y", vector_get_y},
   {"get_z", vector_get_z},
   {"get_xyz", vector_get_xyz},
   {"magnitude", vector_magnitude},
   {"normalize", vector_normalize},
   {"dot", vector_dot},
   {"cross", vector_cross},
   {"is_zero", vector_is_zero},
   {"get_angle", vector_get_angle},
   {"__gc", vector_delete},
   {NULL, NULL},
};

};


//! \cond
void
dmz::open_lua_kernel_vector_lib (lua_State *L) {

   LUA_START_VALIDATE (L);

   luaL_newmetatable (L, VectorName);
   luaL_register (L, NULL, arrayMembers);
   lua_pushvalue (L, -1);
   lua_setfield (L, -2, "__index");
   lua_create_dmz_namespace (L, "vector");
   luaL_register (L, NULL, arrayFunc);
   lua_make_readonly (L, -1); // make vector read only.
   lua_pop (L, 2); // pops meta table and dmz.vector table.

   LUA_END_VALIDATE (L, 0);
}
//! \endcond


//! \addtogroup Lua
//! @{

//! Creates a Vector object on the Lua stack.
dmz::Vector *
dmz::lua_create_vector (lua_State *L, const Vector *Value) {

   Vector *result (0);

   Vector **ptr = (Vector **)lua_newuserdata (L, sizeof (Vector *));

   if (ptr) {

      *ptr = new Vector;

      if (*ptr && Value) { (**ptr) = *Value; }

      luaL_getmetatable (L, VectorName);
      lua_setmetatable (L, -2);

      result = *ptr;
   }

   return result;
}


//! Attempts to convert the specified object on the Lua stack to a Vector object.
dmz::Vector *
dmz::lua_to_vector (lua_State *L, int narg) {

   Vector *result (0);
   Vector **value = (Vector **) lua_is_object (L, narg, VectorName);
   if (value) { result = *value; }

   return result;
}


//! Raises and error if the specified object on the Lua stack is not a Vector object.
dmz::Vector *
dmz::lua_check_vector (lua_State *L, int narg) {

   Vector *result (0);
   Vector **value = vector_check (L, narg);
   if (value) { result = *value; }

   return result;
}
//! @}
