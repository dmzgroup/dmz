#include <dmzLuaKernel.h>
#include "dmzLuaKernelPrivate.h"
#include <dmzLuaKernelValidate.h>
#include <dmzSystem.h>
#include <dmzTypesString.h>
#include <dmzTypesMask.h>
#include <luacpp.h>

using namespace dmz;

namespace {

static const char MaskName[] = "dmz.types.mask";

inline Mask**
mask_check (lua_State *L, int index) {

   return (Mask **)luaL_checkudata (L, index, MaskName);
}


static int
mask_new (lua_State *L) {

   int result (0);

   int stackCount = lua_gettop (L);

   Mask *ptr = lua_create_mask (L);

   if (ptr) {

      result = 1;

      if (0 == stackCount) {

         // do nothing
      }
      else if (1 == stackCount) {

         if (lua_isuserdata (L, 1)) {

            Mask **maskPtr = mask_check (L, 1);

            if (maskPtr && *maskPtr) { *ptr = **maskPtr; }
         }
      }
      else { luaL_error (L, "Unsupported parameters."); }
   }

   return result;
}


static int
mask_is_a (lua_State *L) {

   if (lua_to_mask (L, 1)) { lua_pushvalue (L, 1); }
   else { lua_pushnil (L); }

   return 1;
}


static const luaL_Reg arrayFunc [] = {
   {"new", mask_new},
   {"is_a", mask_is_a},
   {NULL, NULL},
};


static int
mask_clear (lua_State *L) {

   int result (0);

   Mask **mask = mask_check (L, 1);

   if (mask && *mask) {

      (*mask)->clear ();
      lua_pushvalue (L, 1);
      result = 1;
   }

   return result;
}


static int
mask_contains (lua_State *L) {

   int result (0);

   Mask **mask1 = mask_check (L, 1);
   Mask **mask2 = mask_check (L, 2);

   if (mask1 && mask2 && *mask1 && *mask2) {

      lua_pushboolean (L, (*mask1)->contains (**mask2) ? 1 : 0);
      result = 1;
   }

   return result;
}


static int
mask_unset (lua_State *L) {

   int result (0);

   Mask **mask1 = mask_check (L, 1);
   Mask **mask2 = mask_check (L, 2);

   if (mask1 && mask2 && *mask1 && *mask2) {

      lua_pushboolean (L, (*mask1)->unset (**mask2) ? 1 : 0);
      result = 1;
   }

   return result;
}


static int
mask_bit_and (lua_State *L) {

   int result (0);

   Mask **mask1 = mask_check (L, 1);
   Mask **mask2 = mask_check (L, 2);

   if (mask1 && mask2 && *mask1 && *mask2) {

      Mask value ((**mask1) & (**mask2));
      if (lua_create_mask (L, &value)) { result = 1; }
   }

   return result;
}


static int
mask_bit_or (lua_State *L) {

   int result (0);

   Mask **mask1 = mask_check (L, 1);
   Mask **mask2 = mask_check (L, 2);

   if (mask1 && mask2 && *mask1 && *mask2) {

      Mask value ((**mask1) | (**mask2));
      if (lua_create_mask (L, &value)) { result = 1; }
   }

   return result;
}


static int
mask_bit_xor (lua_State *L) {

   int result (0);

   Mask **mask1 = mask_check (L, 1);
   Mask **mask2 = mask_check (L, 2);

   if (mask1 && mask2 && *mask1 && *mask2) {

      Mask value ((**mask1) ^ (**mask2));
      if (lua_create_mask (L, &value)) { result = 1; }
   }

   return result;
}


static int
mask_bit_set (lua_State *L) {

   int result (0);

   Mask **mask1 = mask_check (L, 1);
   Int32 bit ((Int32)luaL_checkint (L, 2));

   if (mask1 && *mask1) { (*mask1)->set_bit (bit); lua_pushvalue (L, 1); result = 1; }

   return result;
}


static int
mask_bit_unset (lua_State *L) {

   int result (0);

   Mask **mask1 = mask_check (L, 1);
   Int32 bit ((Int32)luaL_checkint (L, 2));

   if (mask1 && *mask1) { (*mask1)->unset_bit (bit); lua_pushvalue (L, 1); result = 1; }

   return result;
}


static int
mask_bit_not (lua_State *L) {

   int result (0);

   Mask **mask1 = mask_check (L, 1);

   if (mask1 && *mask1) {

      Mask value (~(**mask1));
      if (lua_create_mask (L, &value)) { result = 1; }
   }

   return result;
}


static int
mask_shift (lua_State *L) {

   int result (0);

   Mask **mask1 = mask_check (L, 1);
   Int32 shift ((Int32)luaL_checkint (L, 2));

   if (mask1 && *mask1) {

      if (shift > 0) { (**mask1) << shift; }
      else if (shift < 0) { (**mask1) >> (-shift); }

      lua_pushvalue (L, 1);
      result = 1;
   }

   return result;
}


static int
mask_to_string (lua_State *L) {

   int result (0);

   Mask **mask = mask_check (L, 1);

   if (mask && *mask) {

      String str;
      str << (**mask);
      lua_pushstring (L, str.get_buffer ());
      result = 1;
   }

   return result;
}


static int
mask_equal (lua_State *L) {

   int result (0);

   Mask **mask1 = mask_check (L, 1);
   Mask **mask2 = mask_check (L, 2);

   if (mask1 && mask2 && *mask1 && *mask2) {

      lua_pushboolean (L, (**mask1 == **mask2 ? 1 : 0));
      result = 1;
   }

   return result;
}

static int
mask_delete (lua_State *L) {

   Mask **mask = mask_check (L, 1);
   if (mask && *mask) { delete (*mask); *mask = 0; }

   return 0;
}


static const luaL_Reg arrayMembers [] = {
   {"__add", mask_bit_or},
   {"clear", mask_clear},
   {"contains", mask_contains},
   {"unset", mask_unset},
   {"bit_and", mask_bit_and},
   {"bit_or", mask_bit_or},
   {"bit_xor", mask_bit_xor},
   {"bit_set", mask_bit_set},
   {"bit_unset", mask_bit_unset},
   {"bit_not", mask_bit_not},
   {"bit_shift", mask_shift},
   {"__tostring", mask_to_string},
   {"__eq", mask_equal},
   {"__gc", mask_delete},
   {NULL, NULL},
};

};


//! \cond
void
dmz::open_lua_kernel_mask_lib (lua_State *L) {

   LUA_START_VALIDATE (L);

   luaL_newmetatable (L, MaskName);
   luaL_register (L, NULL, arrayMembers);
   lua_pushvalue (L, -1);
   lua_setfield (L, -2, "__index");
   lua_create_dmz_namespace (L, "mask");
   luaL_register (L, NULL, arrayFunc);
   lua_make_readonly (L, -1); // make mask read only.
   lua_pop (L, 2); // pops meta table and dmz.mask table.

   LUA_END_VALIDATE (L, 0);
}
//! \endcond


//! \addtogroup Lua
//! @{

//! Creates a Mask on the Lua stack.
dmz::Mask *
dmz::lua_create_mask (lua_State *L, const Mask *Value) {

   Mask *result (0);

   Mask **ptr = (Mask **)lua_newuserdata (L, sizeof (Mask *));

   if (ptr) {

      *ptr = new Mask;

       if (*ptr && Value) { (**ptr) = *Value; }

      luaL_getmetatable (L, MaskName);
      lua_setmetatable (L, -2);

      result = *ptr;
   }

   return result;
}


//! Attempts to convert the specified object on the stack to a Mask object.
dmz::Mask *
dmz::lua_to_mask (lua_State *L, int narg) {

   Mask *result (0);
   Mask **value = (Mask **)lua_is_object (L, narg, MaskName);
   if (value) { result = *value; }

   return result;
}


//! Raises and error if the specified object on the stack is not a Mask object.
dmz::Mask *
dmz::lua_check_mask (lua_State *L, int narg) {

   Mask *result (0);
   Mask **value = mask_check (L, narg);
   if (value) { result = *value; }

   return result;
}
//! @}
