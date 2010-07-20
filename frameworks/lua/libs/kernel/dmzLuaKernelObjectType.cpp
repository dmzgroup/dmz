#include <dmzLuaKernel.h>
#include "dmzLuaKernelPrivate.h"
#include <dmzLuaKernelValidate.h>
#include "dmzLuaKernelPrivate.h"
#include <dmzRuntimeObjectType.h>
#include <dmzSystemStreamString.h>
#include <dmzTypesString.h>
#include <luacpp.h>

using namespace dmz;

namespace {

static const char ObjectTypeName[] = "dmz.runtime.object_type";

inline ObjectType**
object_type_check (lua_State *L, int index) {

   if (index < 0) { index = lua_gettop (L) + index + 1; }

   ObjectType **result = 0;

   if (lua_isstring (L, index)) {

      String name = lua_tostring (L, index);

      if (name) {

         ObjectType *ptr = lua_create_object_type (L);

         if (ptr) {

            ptr->set_type (name, lua_get_runtime_context (L));

            if (!(*ptr)) {

               String error ("Invalid object type name: ");
               error << name;
               lua_pushstring (L, error.get_buffer ());
               lua_error (L);
            }

            lua_replace (L, index);

            result = object_type_check (L, index);
         }
      }
      else {

         lua_pushstring (L, "Empty string can not be converted to an object type");
         lua_error (L);
      }
   }
   else { result = (ObjectType **)luaL_checkudata (L, index, ObjectTypeName); }

   return result;
}


static int
object_type_new (lua_State *L) {

   int result (0);

   int stackCount = lua_gettop (L);

   ObjectType *ptr = lua_create_object_type (L);

   if (ptr) {

      result = 1;

      if (0 == stackCount) {

         // do nothing
      }
      else if (1 == stackCount) {

         ObjectType **objTypePtr = object_type_check (L, 1);

         if (objTypePtr && *objTypePtr) { *ptr = **objTypePtr; }
      }
      else {

         luaL_error (L, "Too many parameters.");
      }
   }

   return result;
}


static int
object_type_is_a (lua_State *L) {

   if (lua_to_object_type (L, 1)) { lua_pushvalue (L, 1); }
   else { lua_pushnil (L); }

   return 1;
}


static const luaL_Reg arrayFunc [] = {
   {"new", object_type_new},
   {"is_a", object_type_is_a},
   {NULL, NULL},
};


static int
object_type_equal (lua_State *L) {

   int result (0);

   ObjectType **objType1 = object_type_check (L, 1);
   ObjectType **objType2 = object_type_check (L, 2);

   if (objType1 && objType2 && *objType1 && *objType2) {

      lua_pushboolean (L, **objType1 == **objType2 ? 1 : 0);
      result = 1;
   }

   return result;
}


static int
object_type_to_string (lua_State *L) {

   int result (0);

   ObjectType **objType = object_type_check (L, 1);

   if (objType && *objType) {

      lua_pushstring (L, (*objType)->get_name ().get_buffer ());
      result = 1;
   }

   return result;
}


static int
object_type_is_of_type (lua_State *L) {

   int result (0);

   ObjectType **objType1 = object_type_check (L, 1);
   ObjectType **objType2 = object_type_check (L, 2);

   if (objType1 && objType2 && *objType1 && *objType2) {

      lua_pushboolean (L, (*objType1)->is_of_type (**objType2) ? 1 : 0);
      result = 1;
   }

   return result;
}


static int
object_type_get_handle (lua_State *L) {

   int result (0);

   ObjectType **objType = object_type_check (L, 1);

   if (objType && *objType) {

      lua_create_handle (L, (*objType)->get_handle ());
      result = 1;
   }

   return result;
}


static int
object_type_get_parent (lua_State *L) {

   int result (0);

   ObjectType **objType = object_type_check (L, 1);

   if (objType && *objType) {

      ObjectType parent ((*objType)->get_parent ());

      if (parent) {

         if (lua_create_object_type (L, &parent)) { result = 1; }
      }
      else { lua_pushnil (L); result = 1; }
   }

   return result;
}


static int
object_type_become_parent (lua_State *L) {

   int result (0);

   ObjectType **objType = object_type_check (L, 1);

   if (objType && *objType) {

      lua_pushboolean (L, (*objType)->become_parent () ? 1 : 0);
      result = 1;
   }

   return result;
}


static int
object_type_get_children (lua_State *L) {

   int result (0);

   ObjectType **objType = object_type_check (L, 1);

   if (objType && *objType) {

      RuntimeIterator it;

      ObjectType type;

      int index (1);

      if ((*objType)->get_first_child (it, type)) {

         lua_newtable (L);
         const int Table = lua_gettop (L);

         do {

            lua_create_object_type (L, &type);
            lua_rawseti (L, Table, index);
            index++;

         } while ((*objType)->get_next_child (it, type));
      }
      else { lua_pushnil (L); }

      result = 1;
   }

   return result;
}


static int
object_type_delete (lua_State *L) {

   ObjectType **objType = object_type_check (L, 1);
   if (objType && *objType) { delete (*objType); *objType = 0; }

   return 0;
}


static const luaL_Reg arrayMembers [] = {
   {"__eq", object_type_equal},
   {"__tostring", object_type_to_string},
   {"is_of_type", object_type_is_of_type},
   {"get_name", object_type_to_string},
   {"get_handle", object_type_get_handle},
   {"get_parent", object_type_get_parent},
   {"become_parent", object_type_become_parent},
   {"get_children", object_type_get_children},
   {"__gc", object_type_delete},
   {NULL, NULL},
};

};


//! \cond
void
dmz::open_lua_kernel_object_type_lib (lua_State *L) {

   LUA_START_VALIDATE (L);

   luaL_newmetatable (L, ObjectTypeName);
   luaL_register (L, NULL, arrayMembers);
   lua_pushvalue (L, -1);
   lua_setfield (L, -2, "__index");
   lua_create_dmz_namespace (L, "object_type");
   luaL_register (L, NULL, arrayFunc);
   lua_make_readonly (L, -1); // make object_type read only.
   lua_pop (L, 2); // pops meta table and dmz.object_type table.

   LUA_END_VALIDATE (L, 0);
}
//! \endcond


//! \addtogroup Lua
//! @{

//! Creates an ObjectType object on the Lua stack.
dmz::ObjectType *
dmz::lua_create_object_type (lua_State *L, const ObjectType *Value) {

   ObjectType *result (0);

   ObjectType **ptr = (ObjectType **)lua_newuserdata (L, sizeof (ObjectType *));

   if (ptr) {

      *ptr = new ObjectType;

       if (*ptr && Value) { (**ptr) = *Value; }

      luaL_getmetatable (L, ObjectTypeName);
      lua_setmetatable (L, -2);

      result = *ptr;
   }

   return result;
}


//! Attempts to convert the specified object on the Lua stack to an ObjectType object.
dmz::ObjectType *
dmz::lua_to_object_type (lua_State *L, int narg) {

   ObjectType *result (0);
   ObjectType **value = (ObjectType **) lua_is_object (L, narg, ObjectTypeName);
   if (value) { result = *value; }

   return result;
}


//! Raises and error if the specified object on the Lua stack is not an ObjectType object.
dmz::ObjectType *
dmz::lua_check_object_type (lua_State *L, int narg) {

   ObjectType *result (0);
   ObjectType **value = object_type_check (L, narg);
   if (value) { result = *value; }

   return result;
}
//! @}
