#include <dmzLuaKernel.h>
#include "dmzLuaKernelPrivate.h"
#include <dmzLuaKernelValidate.h>
#include <dmzRuntimeMessaging.h>
#include <dmzSystemStreamString.h>
#include <dmzTypesString.h>
#include <luacpp.h>

using namespace dmz;

namespace {

static const char MessageTypeName[] = "dmz.runtime.message";

inline MessageType**
message_type_check (lua_State *L, int index) {

   if (index < 0) { index = lua_gettop (L) + index + 1; }

   MessageType **result = 0;

   if (lua_isstring (L, index)) {

      String name = lua_tostring (L, index);

      if (name) {

         MessageType *ptr = lua_create_message_type (L);

         if (ptr) {

            ptr->set_type (name, lua_get_runtime_context (L));

            lua_replace (L, index);

            result = message_type_check (L, index);
         }
      }
      else {

         lua_pushstring (L, "Empty string can not be converted to named handle");
         lua_error (L);
      }
   }
   else { result = (MessageType **)luaL_checkudata (L, index, MessageTypeName); }

   return result;
}


static int
message_type_new (lua_State *L) {

   int result (0);

   int stackCount = lua_gettop (L);

   MessageType *ptr = lua_create_message_type (L);

   if (ptr) {

      result = 1;

      if (0 == stackCount) {

         // do nothing
      }
      else if (1 == stackCount) {

         MessageType **messagePtr = message_type_check (L, 1);

         if (messagePtr && *messagePtr) { *ptr = **messagePtr; }
      }
      else {

         luaL_error (L, "Too many parameters.");
      }
   }

   return result;
}


static int
message_type_is_a (lua_State *L) {

   if (lua_to_message_type (L, 1)) { lua_pushvalue (L, 1); }
   else { lua_pushnil (L); }

   return 1;
}

static int
message_type_send_message (lua_State *L) {

   int result (0);

   MessageType *type = lua_check_message_type (L, 1);
   Handle *toPtr (0);
   Data *inData (0);
   Data *outData (0);

   if (!lua_isnoneornil (L, 2)) { toPtr = lua_check_handle (L, 2); }
   if (!lua_isnoneornil (L, 3)) { inData = lua_check_data (L, 3); }
   if (!lua_isnoneornil (L, 4)) { outData = lua_check_data (L, 4); }

   if (type) {

      lua_pushnumber (
         L, 
         (lua_Number)type->send_message (toPtr ? *toPtr : 0, inData, outData));

      result = 1;
   }

   return result;
}


static const luaL_Reg arrayFunc [] = {
   {"new", message_type_new},
   {"is_a", message_type_is_a},
   {"send", message_type_send_message},
   {NULL, NULL},
};


static int
message_type_equal (lua_State *L) {

   int result (0);

   MessageType **message1 = message_type_check (L, 1);
   MessageType **message2 = message_type_check (L, 2);

   if (message1 && message2 && *message1 && *message2) {

      lua_pushboolean (L, **message1 == **message2 ? 1 : 0);
      result = 1;
   }

   return result;
}


static int
message_type_to_string (lua_State *L) {

   int result (0);

   MessageType **message = message_type_check (L, 1);

   if (message && *message) {

      lua_pushstring (L, (*message)->get_name ().get_buffer ());
      result = 1;
   }

   return result;
}


static int
message_type_is_of_type (lua_State *L) {

   int result (0);

   MessageType **message1 = message_type_check (L, 1);
   MessageType **message2 = message_type_check (L, 2);

   if (message1 && message2 && *message1 && *message2) {

      lua_pushboolean (L, (*message1)->is_of_type (**message2) ? 1 : 0);
      result = 1;
   }

   return result;
}


static int
message_type_get_handle (lua_State *L) {

   int result (0);

   MessageType **message = message_type_check (L, 1);

   if (message && *message) {

      Handle *ptr = lua_create_handle (L, (*message)->get_handle ());

      if (ptr) { result = 1; }
      else { lua_pop (L, 1); } // pop nil
   }

   return result;
}


static int
message_type_get_parent (lua_State *L) {

   int result (0);

   MessageType **message = message_type_check (L, 1);

   if (message && *message) {

      MessageType parent;

      if ((*message)->get_parent (parent)) {

         if (lua_create_message_type (L, &parent)) { result = 1; }
      }
      else { lua_pushnil (L); result = 1; }
   }

   return result;
}


static int
message_type_become_parent (lua_State *L) {

   int result (0);

   MessageType **message = message_type_check (L, 1);

   if (message && *message) {

      lua_pushboolean (L, (*message)->become_parent () ? 1 : 0);
      result = 1;
   }

   return result;
}


static int
message_type_delete (lua_State *L) {

   MessageType **message = message_type_check (L, 1);
   if (message && *message) { delete (*message); *message = 0; }

   return 0;
}


static const luaL_Reg arrayMembers [] = {
   {"__eq", message_type_equal},
   {"__tostring", message_type_to_string},
   {"is_of_type", message_type_is_of_type},
   {"get_name", message_type_to_string},
   {"get_handle", message_type_get_handle},
   {"get_parent", message_type_get_parent},
   {"become_parent", message_type_become_parent},
   {"send_message", message_type_send_message},
   {"__gc", message_type_delete},
   {NULL, NULL},
};

};


void
dmz::open_lua_kernel_message_type_lib (lua_State *L) {

   LUA_START_VALIDATE (L);

   luaL_newmetatable (L, MessageTypeName);
   luaL_register (L, NULL, arrayMembers);
   lua_pushvalue (L, -1);
   lua_setfield (L, -2, "__index");
   lua_create_dmz_namespace (L, "message_type");
   luaL_register (L, NULL, arrayFunc);
   lua_make_readonly (L, -1); // make message_type read only.
   lua_pop (L, 2); // pops meta table and dmz.message table.

   LUA_END_VALIDATE (L, 0);
}


dmz::MessageType *
dmz::lua_create_message_type (lua_State *L, const MessageType *Value) {

   MessageType *result (0);

   MessageType **ptr = (MessageType **)lua_newuserdata (L, sizeof (MessageType *));

   if (ptr) {

      *ptr = new MessageType;

      if (*ptr && Value) { (**ptr) = *Value; }

      luaL_getmetatable (L, MessageTypeName);
      lua_setmetatable (L, -2);

      result = *ptr;
   }

   return result;
}


dmz::MessageType *
dmz::lua_to_message_type (lua_State *L, int narg) {

   MessageType *result (0);
   MessageType **value = (MessageType **) lua_is_object (L, narg, MessageTypeName);
   if (value) { result = *value; }

   return result;
}


dmz::MessageType *
dmz::lua_check_message_type (lua_State *L, int narg) {

   MessageType *result (0);
   MessageType **value = message_type_check (L, narg);
   if (value) { result = *value; }

   return result;
}
