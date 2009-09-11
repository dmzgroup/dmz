#include <dmzLuaKernel.h>
#include "dmzLuaKernelPrivate.h"
#include <dmzLuaKernelValidate.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimeMessaging.h>
#include <dmzSystemStreamString.h>
#include <dmzTypesString.h>
#include <luacpp.h>

using namespace dmz;

namespace {

static const char MessageName[] = "dmz.runtime.message";

inline Message**
message_check (lua_State *L, int index) {

   if (index < 0) { index = lua_gettop (L) + index + 1; }

   Message **result = 0;

   if (lua_isstring (L, index)) {

      String name = lua_tostring (L, index);

      if (name) {

         Message *ptr = lua_create_message (L);

         if (ptr) {

            Definitions defs (lua_get_runtime_context (L));
            defs.create_message (name, *ptr);

            lua_replace (L, index);

            result = message_check (L, index);
         }
      }
      else {

         lua_pushstring (L, "Empty string can not be converted to named handle");
         lua_error (L);
      }
   }
   else { result = (Message **)luaL_checkudata (L, index, MessageName); }

   return result;
}


static int
message_new (lua_State *L) {

   int result (0);

   int stackCount = lua_gettop (L);

   Message *ptr = lua_create_message (L);

   if (ptr) {

      result = 1;

      if (0 == stackCount) {

         // do nothing
      }
      else if (1 == stackCount) {

         Message **messagePtr = message_check (L, 1);

         if (messagePtr && *messagePtr) { *ptr = **messagePtr; }
      }
      else {

         luaL_error (L, "Too many parameters.");
      }
   }

   return result;
}


static int
message_is_a (lua_State *L) {

   if (lua_to_message (L, 1)) { lua_pushvalue (L, 1); }
   else { lua_pushnil (L); }

   return 1;
}

static int
message_send (lua_State *L) {

   int result (0);

   Message *type = lua_check_message (L, 1);
   Handle *toPtr (0);
   Data *inData (0);
   Data *outData (0);

   if (!lua_isnoneornil (L, 2)) { toPtr = lua_check_handle (L, 2); }
   if (!lua_isnoneornil (L, 3)) { inData = lua_check_data (L, 3); }
   if (!lua_isnoneornil (L, 4)) { outData = lua_check_data (L, 4); }

   if (type) {

      lua_pushnumber (
         L,
         (lua_Number)type->send (toPtr ? *toPtr : 0, inData, outData));

      result = 1;
   }

   return result;
}


static const luaL_Reg arrayFunc [] = {
   {"new", message_new},
   {"is_a", message_is_a},
   {"send", message_send},
   {NULL, NULL},
};


static int
message_equal (lua_State *L) {

   int result (0);

   Message **message1 = message_check (L, 1);
   Message **message2 = message_check (L, 2);

   if (message1 && message2 && *message1 && *message2) {

      lua_pushboolean (L, **message1 == **message2 ? 1 : 0);
      result = 1;
   }

   return result;
}


static int
message_to_string (lua_State *L) {

   int result (0);

   Message **message = message_check (L, 1);

   if (message && *message) {

      lua_pushstring (L, (*message)->get_name ().get_buffer ());
      result = 1;
   }

   return result;
}


static int
message_is_of_type (lua_State *L) {

   int result (0);

   Message **message1 = message_check (L, 1);
   Message **message2 = message_check (L, 2);

   if (message1 && message2 && *message1 && *message2) {

      lua_pushboolean (L, (*message1)->is_of_type (**message2) ? 1 : 0);
      result = 1;
   }

   return result;
}


static int
message_get_handle (lua_State *L) {

   int result (0);

   Message **message = message_check (L, 1);

   if (message && *message) {

      Handle *ptr = lua_create_handle (L, (*message)->get_handle ());

      if (ptr) { result = 1; }
      else { lua_pop (L, 1); } // pop nil
   }

   return result;
}


static int
message_get_parent (lua_State *L) {

   int result (0);

   Message **message = message_check (L, 1);

   if (message && *message) {

      Message parent ((*message)->get_parent ());

      if (parent) {

         if (lua_create_message (L, &parent)) { result = 1; }
      }
      else { lua_pushnil (L); result = 1; }
   }

   return result;
}


static int
message_become_parent (lua_State *L) {

   int result (0);

   Message **message = message_check (L, 1);

   if (message && *message) {

      lua_pushboolean (L, (*message)->become_parent () ? 1 : 0);
      result = 1;
   }

   return result;
}


static int
message_delete (lua_State *L) {

   Message **message = message_check (L, 1);
   if (message && *message) { delete (*message); *message = 0; }

   return 0;
}


static const luaL_Reg arrayMembers [] = {
   {"__eq", message_equal},
   {"__tostring", message_to_string},
   {"is_of_type", message_is_of_type},
   {"get_name", message_to_string},
   {"get_handle", message_get_handle},
   {"get_parent", message_get_parent},
   {"become_parent", message_become_parent},
   {"send", message_send},
   {"__gc", message_delete},
   {NULL, NULL},
};

};


//! \cond
void
dmz::open_lua_kernel_message_lib (lua_State *L) {

   LUA_START_VALIDATE (L);

   luaL_newmetatable (L, MessageName);
   luaL_register (L, NULL, arrayMembers);
   lua_pushvalue (L, -1);
   lua_setfield (L, -2, "__index");
   lua_create_dmz_namespace (L, "message");
   luaL_register (L, NULL, arrayFunc);
   lua_make_readonly (L, -1); // make message read only.
   lua_pop (L, 2); // pops meta table and dmz.message table.

   LUA_END_VALIDATE (L, 0);
}
//! \endcond


//! \addtogroup Lua
//! @{

//! Creates a Message object on the Lua stack.
dmz::Message *
dmz::lua_create_message (lua_State *L, const Message *Value) {

   Message *result (0);

   Message **ptr = (Message **)lua_newuserdata (L, sizeof (Message *));

   if (ptr) {

      *ptr = new Message;

      if (*ptr && Value) { (**ptr) = *Value; }

      luaL_getmetatable (L, MessageName);
      lua_setmetatable (L, -2);

      result = *ptr;
   }

   return result;
}


//! Attempts to convert the specified object on the Lua stack to a Message object.
dmz::Message *
dmz::lua_to_message (lua_State *L, int narg) {

   Message *result (0);
   Message **value = (Message **) lua_is_object (L, narg, MessageName);
   if (value) { result = *value; }

   return result;
}


//! Raises and error if the specified object on the Lua stack is not a Message object.
dmz::Message *
dmz::lua_check_message (lua_State *L, int narg) {

   Message *result (0);
   Message **value = message_check (L, narg);
   if (value) { result = *value; }

   return result;
}
//! @}
