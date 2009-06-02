#include <dmzLuaKernel.h>
#include "dmzLuaKernelPrivate.h"
#include <dmzLuaKernelValidate.h>
#include <dmzSystem.h>
#include <dmzTypesString.h>
#include <dmzTypesSphere.h>
#include <dmzTypesVector.h>
#include <luacpp.h>

using namespace dmz;

namespace {

static const char SphereName[] = "dmz.types.sphere";

inline Sphere*
sphere_check (lua_State *L, int index) {

   if (index < 0) { index = lua_gettop (L) + index + 1; }

   Sphere *result (0);

   Sphere **ptr = (Sphere **)luaL_checkudata (L, index, SphereName);

   if (ptr) { result = *ptr; }

   return result;
}


static int
sphere_new (lua_State *L) {

   int result (0);

   const int StackCount (lua_gettop (L));

   if (!StackCount) {

      Sphere value;
      result = (lua_create_sphere (L, value) ? 1 : 0);
   }
   else if (StackCount == 2) {

      Vector *origin = lua_check_vector (L, 1);
      lua_Number radius = luaL_checknumber (L, 2);

      if (origin) {

         Sphere value (*origin, radius);
         result = (lua_create_sphere (L, value) ? 1 : 0);
      }
   }
   else {

      luaL_error (L, "Unsupported parameters.");
   }

   return result;
}


static int
sphere_is_a (lua_State *L) {

   if (lua_to_sphere (L, 1)) { lua_pushvalue (L, 1); }
   else { lua_pushnil (L); }

   return 1;
}


static const luaL_Reg arrayFunc [] = {
   {"new", sphere_new},
   {"is_a", sphere_is_a},
   {NULL, NULL},
};


static int
sphere_to_string (lua_State *L) {

   int result (0);

   Sphere *sphere = sphere_check (L, 1);

   if (sphere) {

      String str;
      str << "origin: " << sphere->get_origin () << " radius: " << sphere->get_radius ();
      lua_pushstring (L, str.get_buffer ());
      result = 1;
   }

   return result;
}


static int
sphere_equal (lua_State *L) {

   int result (0);

   Sphere *sphere1 = sphere_check (L, 1);
   Sphere *sphere2 = sphere_check (L, 2);

   if (sphere1 && sphere2) {

      lua_pushboolean (L, (sphere1 == sphere2 ? 1 : 0));
      result = 1;
   }

   return result;
}


static int
sphere_set_origin (lua_State *L) {

   Sphere *sphere = sphere_check (L, 1);
   Vector *origin = lua_check_vector (L, 2);

   if (sphere && origin) {

      sphere->set_origin (*origin);
   }

   return 0;
}


static int
sphere_get_origin (lua_State *L) {

   int result = 0;

   Sphere *sphere = sphere_check (L, 1);

   if (sphere) {

      const Vector Origin = sphere->get_origin ();

      lua_create_vector (L, &Origin);
      result = 1;
   }

   return result;
}


static int
sphere_contains_point (lua_State *L) {

   int result = 0;

   Sphere *sphere = sphere_check (L, 1);
   Vector *point = lua_check_vector (L, 2);

   if (sphere && point) {

      lua_pushboolean (L, sphere->contains_point (*point) ? 1 : 0);
      result = 1;
   }

   return result;
}


static int
sphere_get_extents (lua_State *L) {

   int result = 0;

   Sphere *sphere = sphere_check (L, 1);

   if (sphere) {

      Vector origin, min, max;
      sphere->get_extents (origin, min, max);

      lua_create_vector (L, &origin);
      lua_create_vector (L, &min);
      lua_create_vector (L, &max);

      result = 3;
   }

   return result;
}


static int
sphere_set_radius (lua_State *L) {

   Sphere *sphere = sphere_check (L, 1);
   lua_Number radius = luaL_checknumber (L, 2);

   if (sphere) { sphere->set_radius (radius); }

   return 0;
}


static int
sphere_get_radius (lua_State *L) {

   int result = 0;

   Sphere *sphere = sphere_check (L, 1);

   if (sphere) {

      lua_pushnumber (L, sphere->get_radius ());
      result = 1;
   }

   return result;
}


static int
sphere_delete (lua_State *L) {

   LUA_START_VALIDATE (L);

   Sphere **sphere = (Sphere **)luaL_checkudata (L, 1, SphereName);

   if (sphere) { delete (*sphere); *sphere = 0; }

   LUA_END_VALIDATE (L, 0);

   return 0;
}


static const luaL_Reg arrayMembers[] = {
   {"__tostring", sphere_to_string},
   {"__eq", sphere_equal},
   {"set_origin", sphere_set_origin},
   {"get_origin", sphere_get_origin},
   {"contains_point", sphere_contains_point},
   {"get_extents", sphere_get_extents},
   {"set_radius", sphere_set_radius},
   {"get_radius", sphere_get_radius},
   {"__gc", sphere_delete},
   {NULL, NULL},
};

};


//! \cond
void
dmz::open_lua_kernel_sphere_lib (lua_State *L) {

   LUA_START_VALIDATE (L);

   luaL_newmetatable (L, SphereName);
   luaL_register (L, NULL, arrayMembers);
   lua_pushvalue (L, -1);
   lua_setfield (L, -2, "__index");
   lua_create_dmz_namespace (L, "sphere");
   luaL_register (L, NULL, arrayFunc);
   lua_make_readonly (L, -1); // make sphere read only.
   lua_pop (L, 2); // pops meta table and dmz.sphere table.

   LUA_END_VALIDATE (L, 0);
}
//! \endcond


//! \addtogroup Lua
//! @{

//! Creates a Sphere object on the Lua stack.
dmz::Sphere *
dmz::lua_create_sphere (lua_State *L, const Sphere &Value) {

   LUA_START_VALIDATE (L);

   Sphere *result (0);

   Sphere **ptr = (Sphere **)lua_newuserdata (L, sizeof (Sphere *));

   if (ptr) {

      *ptr = new Sphere;

      if (*ptr) { (**ptr) = Value; }

      luaL_getmetatable (L, SphereName);
      lua_setmetatable (L, -2);

      result = *ptr;
   }

   LUA_END_VALIDATE (L, (result ? 1 : 0));

   return result;
}


//! Attempts to convert the specified object on the Lua stack to a Sphere object.
dmz::Sphere *
dmz::lua_to_sphere (lua_State *L, int narg) {

   Sphere *result (0);
   Sphere **value = (Sphere **) lua_is_object (L, narg, SphereName);
   if (value) { result = *value; }

   return result;
}


//! Raises and error if the specified object on the Lua stack is not a Sphere object.
dmz::Sphere *
dmz::lua_check_sphere (lua_State *L, int narg) {

   return sphere_check (L, narg);
}
//! @}
