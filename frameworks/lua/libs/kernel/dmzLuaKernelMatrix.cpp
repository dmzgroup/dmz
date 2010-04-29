#include <dmzLuaKernel.h>
#include "dmzLuaKernelPrivate.h"
#include <dmzLuaKernelValidate.h>
#include <dmzTypesMatrix.h>
#include <dmzTypesString.h>
#include <dmzTypesVector.h>
#include <luacpp.h>

using namespace dmz;

namespace {

static const char MatrixName[] = "dmz.types.matrix";

inline Matrix**
matrix_check (lua_State *L, int index) {

   if (index < 0) { index = lua_gettop (L) + index + 1; }

   Matrix **result = 0;

   if (lua_istable (L, index)) {

      lua_rawgeti (L, index, 1);
      lua_rawgeti (L, index, 2);
      lua_rawgeti (L, index, 3);
      lua_rawgeti (L, index, 4);
      lua_rawgeti (L, index, 5);
      lua_rawgeti (L, index, 6);
      lua_rawgeti (L, index, 7);
      lua_rawgeti (L, index, 8);
      lua_rawgeti (L, index, 9);

      Float64 array[9];

      array[0] = luaL_checknumber (L, -9);
      array[1] = luaL_checknumber (L, -8);
      array[2] = luaL_checknumber (L, -7);
      array[3] = luaL_checknumber (L, -6);
      array[4] = luaL_checknumber (L, -5);
      array[5] = luaL_checknumber (L, -4);
      array[6] = luaL_checknumber (L, -3);
      array[7] = luaL_checknumber (L, -2);
      array[8] = luaL_checknumber (L, -1);

      Matrix *ptr = lua_create_matrix (L);

      if (ptr) {

         ptr->from_array (array);
         lua_replace (L, index);
      }

      lua_pop (L, 9);

      result = matrix_check (L, index);

   }
   else { result = (Matrix **)luaL_checkudata (L, index, MatrixName); }

   return result;
}


static int
matrix_new (lua_State *L) {

   int result (0);

   int stackCount = lua_gettop (L);

   Matrix *ptr = lua_create_matrix (L);

   if (ptr) {

      result = 1;

      if (0 == stackCount) {

         // do nothing
      }
      else if (1 == stackCount) {

         if (lua_isuserdata (L, 1)) {

            Matrix **matPtr = matrix_check (L, 1);

            if (matPtr && *matPtr) { *ptr = **matPtr; }
         }
         else if (lua_istable (L, 1)) {

            Float64 array[9];

            for (int ix = 1; ix <= 9; ix++) {

               lua_rawgeti (L, 1, ix);
               array[ix - 1] = lua_tonumber (L, -1);
               lua_pop (L, 1);
            }

            ptr->from_array (array);
         }
         else { luaL_error (L, "Unsupported parameter."); }
      }
      else if (2 == stackCount) {

         Vector *vec1 (lua_check_vector (L, 1));
         Vector *vec2 (lua_to_vector (L, 2));

         if (vec1 && vec2) {

            ptr->from_two_vectors (*vec1, *vec2);
         }
         else if (vec1) {

            lua_Number angle = luaL_checknumber (L, 2);
            ptr->from_axis_and_angle (*vec1, angle);
         }
      }
      else if (9 == stackCount) {

         Float64 array[9];

         array[1] = luaL_checknumber (L, 1);
         array[2] = luaL_checknumber (L, 2);
         array[3] = luaL_checknumber (L, 3);
         array[4] = luaL_checknumber (L, 4);
         array[5] = luaL_checknumber (L, 5);
         array[6] = luaL_checknumber (L, 6);
         array[7] = luaL_checknumber (L, 7);
         array[8] = luaL_checknumber (L, 8);
         array[9] = luaL_checknumber (L, 9);

         ptr->from_array (array);
      }
      else {

         luaL_error (L, "Too many parameters.");
      }
   }

   return result;
}


static int
matrix_is_a (lua_State *L) {

   if (lua_to_matrix (L, 1)) { lua_pushvalue (L, 1); }
   else { lua_pushnil (L); }

   return 1;
}


static const luaL_Reg arrayFunc [] = {
   {"new", matrix_new},
   {"is_a", matrix_is_a},
   {NULL, NULL},
};


static int
matrix_to_string (lua_State *L) {

   int result (0);

   Matrix **mat = matrix_check (L, 1);

   if (mat && *mat) {

      Float64 array[9];
      (*mat)->to_array (array);

      String str;

      str << "{"
         << array[0] << ", "
         << array[1] << ", "
         << array[2] << ", "
         << array[3] << ", "
         << array[4] << ", "
         << array[5] << ", "
         << array[6] << ", "
         << array[7] << ", "
         << array[8] << "}";

      lua_pushstring (L, str.get_buffer ());
      result = 1;
   }

   return result;
}


static int
matrix_add (lua_State *L) {

   int result (0);

   Matrix **mat1 = matrix_check (L, 1);
   Matrix **mat2 = matrix_check (L, 2);

   if (mat1 && mat2 && *mat1 && *mat2) {

      Matrix value (**mat1 + **mat2);
      if (lua_create_matrix (L, &value)) { result = 1; }
   }

   return result;
}


static int
matrix_subtract (lua_State *L) {

   int result (0);

   Matrix **mat1 = matrix_check (L, 1);
   Matrix **mat2 = matrix_check (L, 2);

   if (mat1 && mat2 && *mat1 && *mat2) {

      Matrix value (**mat1 - **mat2);
      if (lua_create_matrix (L, &value)) { result = 1; }
   }

   return result;
}


static int
matrix_multiply (lua_State *L) {

   int result (0);

   Matrix **mat = matrix_check (L, 1);

   if (lua_isuserdata (L, 2)) {

      Matrix **mat2 = matrix_check (L, 2);

      if (mat2 && *mat2) {

         Matrix value ((**mat) * (**mat2));
         if (lua_create_matrix (L, &value)) { result = 1; }
      }
   }
   else {

      lua_Number value (luaL_checknumber (L, 2));

      Matrix mat2 ((**mat) * value);
      if (lua_create_matrix (L, &mat2)) { result = 1; }
   }

   return result;
}


static int
matrix_equal (lua_State *L) {

   int result (0);

   Matrix **mat1 = matrix_check (L, 1);
   Matrix **mat2 = matrix_check (L, 2);

   if (mat1 && mat2 && *mat1 && *mat2) {

      lua_pushboolean (L, (**mat1 == **mat2 ? 1 : 0));
      result = 1;
   }

   return result;
}


static int
matrix_unm (lua_State *L) {

   int result (0);

   Matrix **mat = matrix_check (L, 1);

   if (mat && *mat) {

      Matrix value ((**mat).negate ());
      if (lua_create_matrix (L, &value)) { result = 1; }
   }

   return result;
}


static int
matrix_transpose (lua_State *L) {

   int result (0);

   Matrix **mat = matrix_check (L, 1);

   if (mat && *mat) {

      Matrix value ((**mat).transpose ());
      if (lua_create_matrix (L, &value)) { result = 1; }
   }

   return result;
}


static int
matrix_invert (lua_State *L) {

   int result (0);

   Matrix **mat = matrix_check (L, 1);

   if (mat && *mat) {

      Matrix value;
      (**mat).invert (value);
      if (lua_create_matrix (L, &value)) { result = 1; }
   }

   return result;
}


static int
matrix_transform (lua_State *L) {

   int result (0);

   Matrix **mat = matrix_check (L, 1);
   Vector *vec (lua_check_vector (L, 2));

   if (mat && *mat && vec) {

      Vector *value = lua_create_vector (L, vec);
      if (value) { (*mat)->transform_vector (*value); result = 1; }
   }

   return result;
}


static int
matrix_from_table (lua_State *L) {

   int result (0);

   Matrix **mat = matrix_check (L, 1);
   luaL_checktype (L, 2, LUA_TTABLE);

   if (mat && *mat) {

      Float64 array[9];

      for (int ix = 1; ix <= 9; ix++) {

         lua_rawgeti (L, 2, ix);
         array[ix - 1] = lua_tonumber (L, -1);
         lua_pop (L, 1);
      }

      (*mat)->from_array (array);

      lua_pushvalue (L, 1);
      result = 1;
   }

   return result;
}


static int
matrix_to_table (lua_State *L) {

   int result (0);

   Matrix **mat = matrix_check (L, 1);

   if (mat && *mat) {

      Float64 array[9];
      (*mat)->to_array (array);

      lua_createtable (L, 9,0);

      for (int ix = 1; ix <= 9; ix++) {

         lua_pushnumber (L, array[ix -1]);
         lua_rawseti (L, -2, ix);
      }

      result = 1;
   }

   return result;
}


static int
matrix_from_two_vectors (lua_State *L) {

   int result (0);

   Matrix **mat = matrix_check (L, 1);
   Vector *vec1 (lua_check_vector (L, 2));
   Vector *vec2 (lua_check_vector (L, 3));

   if (mat && *mat && vec1 && vec2) {

      (*mat)->from_two_vectors (*vec1, *vec2);
      lua_pushvalue (L, 1);
      result = 1;
   }

   return result;
}


static int
matrix_from_axis_and_angle (lua_State *L) {

   int result (0);

   Matrix **mat = matrix_check (L, 1);
   Vector *axis (lua_check_vector (L, 2));
   Float64 angle (luaL_checknumber (L, 3));

   if (mat && *mat && axis) {

      (*mat)->from_axis_and_angle (*axis, angle);
      lua_pushvalue (L, 1);
      result = 1;
   }

   return result;
}


static int
matrix_set_identity (lua_State *L) {

   int result (0);

   Matrix **mat = matrix_check (L, 1);

   if (mat && *mat) { (*mat)->set_identity (); lua_pushvalue (L, 1); result = 1; }

   return result;
}


static int
matrix_is_identity (lua_State *L) {

   int result (0);

   Matrix **mat = matrix_check (L, 1);

   if (mat && *mat) {

      lua_pushboolean (L, (*mat)->is_identity () ? 1 : 0);
      result = 1;
   }

   return result;
}


static int
matrix_delete (lua_State *L) {

   Matrix **mat = matrix_check (L, 1);
   if (mat && *mat) { delete (*mat); *mat = 0; }

   return 0;
}


static const luaL_Reg arrayMembers [] = {
   {"__tostring", matrix_to_string},
   {"__add", matrix_add},
   {"__sub", matrix_subtract},
   {"__mul", matrix_multiply},
   {"__eq", matrix_equal},
   {"__unm", matrix_unm},
   {"transpose", matrix_transpose},
   {"invert", matrix_invert},
   {"transform", matrix_transform},
   {"from_table", matrix_from_table},
   {"to_table", matrix_to_table},
   {"from_two_vectors", matrix_from_two_vectors},
   {"from_axis_and_angle", matrix_from_axis_and_angle},
   {"is_identity", matrix_is_identity},
   {"set_identity", matrix_set_identity},
   {"__gc", matrix_delete},
   {NULL, NULL},
};

};


//! \cond
void
dmz::open_lua_kernel_matrix_lib (lua_State *L) {

   LUA_START_VALIDATE (L);

   luaL_newmetatable (L, MatrixName);
   luaL_register (L, NULL, arrayMembers);
   lua_pushvalue (L, -1);
   lua_setfield (L, -2, "__index");
   lua_create_dmz_namespace (L, "matrix");
   luaL_register (L, NULL, arrayFunc);
   lua_make_readonly (L, -1); // make matrix read only.
   lua_pop (L, 2); // pops meta table and dmz.matrix table.

   LUA_END_VALIDATE (L, 0);
}
//! \endcond


//! \addtogroup Lua
//! @{

//! Creates a Matrix object on the Lua stack.
dmz::Matrix *
dmz::lua_create_matrix (lua_State *L, const Matrix *Value) {

   Matrix *result (0);

   Matrix **ptr = (Matrix **)lua_newuserdata (L, sizeof (Matrix *));

   if (ptr) {

      *ptr = new Matrix;

       if (*ptr && Value) { (**ptr) = *Value; }

      luaL_getmetatable (L, MatrixName);
      lua_setmetatable (L, -2);

      result = *ptr;
   }

   return result;
}


//! Attempts to convert the specified object on the Lua stack to a Matrix object.
dmz::Matrix *
dmz::lua_to_matrix (lua_State *L, int narg) {

   Matrix *result (0);
   Matrix **value = (Matrix **) lua_is_object (L, narg, MatrixName);
   if (value) { result = *value; }

   return result;
}


//! Raises and error if the specified object on the Lua stack is not a Matrix object.
dmz::Matrix *
dmz::lua_check_matrix (lua_State *L, int narg) {

   Matrix *result (0);
   Matrix **value = matrix_check (L, narg);
   if (value) { result = *value; }

   return result;
}
//! @}
