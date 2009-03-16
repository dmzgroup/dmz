#include <dmzLuaKernel.h>
#include "dmzLuaKernelPrivate.h"
#include <dmzLuaKernelValidate.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeConfigToMatrix.h>
#include <dmzRuntimeConfigToVector.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimeEventType.h>
#include <dmzRuntimeObjectType.h>
#include <dmzSystem.h>
#include <dmzSystemStreamString.h>
#include <dmzTypesString.h>
#include <dmzTypesMask.h>
#include <dmzTypesUUID.h>
#include <luacpp.h>

using namespace dmz;

namespace {

static const char ConfigName[] = "dmz.runtime.config";
static const char ConfigGlobalName[] = "dmz.runtime.config.global";

static inline Config**
config_check (lua_State *L, int index) {

   return (Config **)luaL_checkudata (L, index, ConfigName);
}


static int
config_get_global (lua_State *L) {

   LUA_START_VALIDATE (L);
   lua_pushstring (L, ConfigGlobalName);
   lua_rawget (L, LUA_REGISTRYINDEX);
   LUA_END_VALIDATE (L, 1);

   return 1;
}


static int
config_new (lua_State *L) {

   LUA_START_VALIDATE (L);

   int result (0);

   int stackCount = lua_gettop (L);

   String name;

   if (0 == stackCount) {

      // do nothing
   }
   else if (1 == stackCount) {

      String value ((luaL_checkstring (L, 1)));
      name = value;
   }
   else { luaL_error (L, "Unsupported parameters."); }

   Config *ptr = lua_create_config (L, name);

   if (ptr) { result = 1; }

   LUA_END_VALIDATE (L, result);

   return result;
}


static int
config_is_a (lua_State *L) {

   if (lua_to_config (L, 1)) { lua_pushvalue (L, 1); }
   else { lua_pushnil (L); }

   return 1;
}


static const luaL_Reg arrayFunc [] = {
   {"get_global", config_get_global},
   {"new", config_new},
   {"is_a", config_is_a},
   {NULL, NULL},
};


static int
config_get_name (lua_State *L) {

   LUA_START_VALIDATE (L);

   int result (0);

   Config **config = config_check (L, 1);

   if (config && *config) {

      String str ((*config)->get_name ());
      lua_pushlstring (L, str.get_buffer (), str.get_length ());
      result = 1;
   }

   LUA_END_VALIDATE (L, result);

   return result;
}


static int
config_get_attributes (lua_State *L) {

   LUA_START_VALIDATE (L);

   int result (0);

   Config **config = config_check (L, 1);

   if (config && *config) {

      lua_newtable (L);

      String name, value;
      ConfigIterator it;

      for (
            Boolean found = (*config)->get_first_attribute (it, name, value);
            found;
            found = (*config)->get_next_attribute (it, name, value)) {

         lua_pushlstring (L, value.get_buffer (), value.get_length ());
         lua_setfield (L, -2, name.get_buffer ());
      }

      LUA_END_VALIDATE (L, 1);

      result = 1;
   }

   return result;
}


static int
config_store_attribute (lua_State *L) {

   int result (0);

   Config **config = config_check (L, 1);
   String name (luaL_checkstring (L, 2));
   String value (luaL_checkstring (L, 3));

   if (config && *config && name) {

      lua_pushboolean (L, (*config)->store_attribute (name, value) ? 1 : 0);
      result = 1;
   }

   return result;
}


static int
config_lookup_attribute (lua_State *L) {

   int result (0);

   Config **config = config_check (L, 1);
   String name (luaL_checkstring (L, 2));

   if (config && *config && name) {

      String value;

      if ((*config)->lookup_attribute (name, value)) {

         lua_pushlstring (L, value.get_buffer (), value.get_length ());
      }
      else {

         lua_pushnil (L);
      }

      result = 1;
   }

   return result;
}


static int
config_to_boolean (lua_State *L) {

   int result (0);

   Config **config = config_check (L, 1);
   String name (luaL_checkstring (L, 2));

   Boolean value (False);

   if (!lua_isnoneornil (L, 3)) {

      value = (lua_toboolean (L, 3) == 1);
   }

   if (config && *config) {

      lua_pushboolean (L, config_to_boolean (name, **config, value) ? 1 : 0);
      result = 1;
   }

   return result;
}


static int
config_to_string (lua_State *L) {

   int result (0);

   Config **config = config_check (L, 1);
   String name (luaL_checkstring (L, 2));

   String value;

   if (!lua_isnoneornil (L, 3)) {

      value = luaL_checkstring (L, 3);
   }

   if (config && *config) {

      lua_pushstring (L, config_to_string (name, **config, value).get_buffer ());
      result = 1;
   }

   return result;
}


static int
config_to_number (lua_State *L) {

   int result (0);

   Config **config = config_check (L, 1);
   String name (luaL_checkstring (L, 2));

   Float64 value (0.0);

   if (!lua_isnoneornil (L, 3)) {

      value = luaL_checknumber (L, 3);
   }

   if (config && *config) {

      lua_pushnumber (L, config_to_float64 (name, **config, value));
      result = 1;
   }

   return result;
}


static int
config_to_handle (lua_State *L) {

   int result (0);

   Config **config = config_check (L, 1);
   String name (luaL_checkstring (L, 2));

   Handle *value (0);

   if (!lua_isnoneornil (L, 3)) {

      value = lua_check_handle (L, 3);
   }

   if (config && *config) {

      const String HandleName (config_to_string (name, **config));

      if (HandleName) {

         Definitions defs (lua_get_runtime_context (L));
         Handle handle = defs.create_named_handle (HandleName);

         lua_create_handle (L, handle);
         result = 1;
      }
      else if (value) { lua_pushvalue (L, 3); result = 1; }
   }

   return result;
}


static int
config_to_matrix (lua_State *L) {

   int result (0);

   Config **config = config_check (L, 1);
   String name (luaL_checkstring (L, 2));

   Matrix *value (0);

   if (!lua_isnoneornil (L, 3)) {

      value = lua_check_matrix (L, 3);
   }

   if (config && *config) {

      Matrix defaultValue;

      if (value) { defaultValue = *value; }

      Matrix mat = config_to_matrix (name, **config, defaultValue);

      Matrix *ptr = lua_create_matrix (L, &mat);

      if (ptr) { result = 1; }
   }


   return result;
}


static int
config_to_event_type (lua_State *L) {

   int result (0);

   Config **config = config_check (L, 1);
   String name (luaL_checkstring (L, 2));

   EventType *value (0);

   if (!lua_isnoneornil (L, 3)) {

      value = lua_check_event_type (L, 3);
   }

   if (config && *config) {

      const String EventName (config_to_string (name, **config));

      EventType *type = lua_create_event_type (L);

      if (type) {

         if (!type->set_type (EventName, lua_get_runtime_context (L))) {

            if (value) { *type = *value; }
         }

         result = 1;
      }
   }

   return result;
}


static int
config_to_message (lua_State *L) {

   int result (0);

   Config **config = config_check (L, 1);
   String name (luaL_checkstring (L, 2));

   Message *value (0);

   if (!lua_isnoneornil (L, 3)) {

      value = lua_check_message (L, 3);
   }

   if (config && *config) {

      const String MessageName (config_to_string (name, **config));

      Message *type = lua_create_message (L);

      if (type) {

         if (!type->set_type (MessageName, lua_get_runtime_context (L))) {

            if (value) { *type = *value; }
         }

         result = 1;
      }
   }
   return result;
}


static int
config_to_object_type (lua_State *L) {

   int result (0);

   Config **config = config_check (L, 1);
   String name (luaL_checkstring (L, 2));

   ObjectType *value (0);

   if (!lua_isnoneornil (L, 3)) {

      value = lua_check_object_type (L, 3);
   }

   if (config && *config) {

      const String ObjectName (config_to_string (name, **config));

      ObjectType *type = lua_create_object_type (L);

      if (type) {

         if (!type->set_type (ObjectName, lua_get_runtime_context (L))) {

            if (value) { *type = *value; }
         }

         result = 1;
      }
   }

   return result;
}


static int
config_to_state (lua_State *L) {

   int result (0);

   Config **config = config_check (L, 1);
   String name (luaL_checkstring (L, 2));

   Definitions defs (lua_get_runtime_context (L));

   Mask *value (0);

   if (!lua_isnoneornil (L, 3)) {

      Mask *tmpValue = lua_to_mask (L, 3);

      if (tmpValue) { value = new Mask (*tmpValue); }
      else {

         const String StateName (lua_tostring (L, 3));

         if (StateName) {

            value = new Mask;

            if (value) { defs.lookup_state (StateName, *value); }
         }
      }

      if (!value) {

         lua_pushstring (L, "Unable to convert parameter #3 to dmz.types.mask");
         lua_error (L);
      }
   }

   if (config && *config) {

      const String StateName (config_to_string (name, **config));

      Mask *state = lua_create_mask (L);

      if (state) {

         if (StateName) { defs.lookup_state (StateName, *state); }
         else if (value) { *state = *value; }

         result = 1;
      }
   }

   if (value) { delete value; value = 0; }

   return result;
}


static int
config_to_uuid (lua_State *L) {

   int result (0);

   Config **config = config_check (L, 1);
   String name (luaL_checkstring (L, 2));

   UUID value;

   if (!lua_isnoneornil (L, 3)) {

      UUID *ptr = lua_check_uuid (L, 3);
      if (ptr) { value = *ptr; }
   }

   if (config && *config) {

      UUID uuid (value);

      String uuidStr = config_to_string (name, **config);

      if (uuidStr && uuid.from_string (uuidStr) && lua_create_uuid (L, uuid)) {

         result = 1;
      }
   }

   return result;
}


static int
config_to_vector (lua_State *L) {

   int result (0);

   Config **config = config_check (L, 1);
   String name (luaL_checkstring (L, 2));
   Vector *value (0);

   if (!lua_isnoneornil (L, 3)) { value = lua_check_vector (L, 3); }

   if (config && *config) {

      Vector vec;
      if (value) { vec = *value; }
      vec = config_to_vector (name, **config, vec);

      Vector *ptr = lua_create_vector (L, &vec);

      if (ptr) { result = 1; }
   }

   return result;
}


static int
config_set_value (lua_State *L) {

   int result (0);

   Config **config = config_check (L, 1);
   String value (luaL_checkstring (L, 2));

   if (config && *config) {

      lua_pushboolean (L, (*config)->set_value (value) ? 1 : 0);
      result = 1;
   }

   return result;
}


static int
config_append_value (lua_State *L) {

   int result (0);

   Config **config = config_check (L, 1);
   String value (luaL_checkstring (L, 2));

   if (config && *config) {

      lua_pushboolean (L, (*config)->append_value (value, False) ? 1 : 0);
      result = 1;
   }

   return result;
}


static int
config_get_value (lua_State *L) {

   int result (0);

   Config **config = config_check (L, 1);

   if (config && *config) {

      String value;

      if ((*config)->get_value (value)) {

         lua_pushlstring (L, value.get_buffer (), value.get_length ());
      }
      else { lua_pushnil (L); }

      result = 1;
   }

   return result;
}


static int
config_get_children (lua_State *L) {

   int result (0);

   Config **config = config_check (L, 1);

   if (config && *config) {

      lua_newtable (L);

      Config child;
      ConfigIterator it;
      int count (0);

      for (
            Boolean found = (*config)->get_first_config (it, child);
            found;
            found = (*config)->get_next_config (it, child)) {

         Config *ptr (lua_create_config (L, ""));

         if (ptr) {

            count++;
            *ptr = child;
            lua_rawseti (L, -2, count);
         }
      }

      result = 1;
   }

   return result;
}


static int
config_add_config (lua_State *L) {

   int result (0);

   Config **config1 = config_check (L, 1);
   String scope (lua_tostring (L, 2));
   const int Offset (scope ? 3 : 2);
   const Boolean IsTable (lua_istable (L, Offset));
   Config **config2 = 0;
   if (!IsTable) { config2 = config_check (L, Offset); }

   if (config1 && *config1) {

      Boolean fail (False);

      if (IsTable) {

         lua_pushnil (L);

         while (lua_next (L, Offset) != 0) {

            Config *ptr = lua_to_config (L, -1);

            if (ptr) {

               if (!(*config1)->add_config (scope, *ptr)) { fail = True; }
            }

            lua_pop (L, 1);
         }

         lua_pushboolean (L, fail ? 0 : 1);
      }
      else if (config2 && *config2) {

        lua_pushboolean (L, (*config1)->add_config (scope, **config2) ? 1 : 0);
      }
      else { lua_pushboolean (L, 0); }

      result = 1;
   }

   return result;
}


static int
config_lookup_config (lua_State *L) {

   int result (0);

   Config **config = config_check (L, 1);
   String scope (lua_tostring (L, 2));

   if (config && *config) {

      Config value;

      if ((*config)->lookup_config (scope, value)) {

         Config *ptr = lua_create_config (L, "");

         if (ptr) { *ptr = value; }
      }
      else { lua_pushnil (L); }

      result = 1;
   }

   return result;
}


static int
config_lookup_all_config (lua_State *L) {

   int result (0);

   Config **config = config_check (L, 1);
   String scope (lua_tostring (L, 2));

   if (config && *config) {

      Config value;

      if ((*config)->lookup_all_config (scope, value)) {

         lua_newtable (L);

         Config child;
         ConfigIterator it;
         int count (0);

         for (
               Boolean found = value.get_first_config (it, child);
               found;
               found = value.get_next_config (it, child)) {

            Config *ptr (lua_create_config (L, ""));

            if (ptr) {

               count++;
               *ptr = child;
               lua_rawseti (L, -2, count);
            }
         }
      }
      else { lua_pushnil (L); }

      result = 1;
   }

   return result;
}


static int
config_lookup_all_config_merged (lua_State *L) {

   int result (0);

   Config **config = config_check (L, 1);
   String scope (lua_tostring (L, 2));

   if (config && *config) {

      Config value;

      if ((*config)->lookup_all_config_merged (scope, value)) {

         Config *ptr = lua_create_config (L, "");

         if (ptr) { *ptr = value; }
      }
      else { lua_pushnil (L); }

      result = 1;
   }

   return result;
}


static int
config_tostring (lua_State *L) {

   int result (0);

   Config **config = config_check (L, 1);

   if (config && *config) {

      String str;
      StreamString stream (str);
      stream << **config;
      lua_pushlstring (L, str.get_buffer (), str.get_length ());
      result = 1;
   }

   return result;
}


static int
config_equal (lua_State *L) {

   int result (0);

   Config **config1 = config_check (L, 1);
   Config **config2 = config_check (L, 2);

   if (config1 && config2 && *config1 && *config2) {

      lua_pushboolean (L, (**config1) == (**config2) ? 1 : 0);
      result = 1;
   }

   return result;
}


static int
config_delete (lua_State *L) {

   Config **config = config_check (L, 1);
   if (config && *config) { delete (*config); *config = 0; }

   return 0;
}


static const luaL_Reg arrayMembers [] = {
   {"get_name", config_get_name},
   {"get_attributes", config_get_attributes},
   {"store_attribute", config_store_attribute},
   {"lookup_attribute", config_lookup_attribute},
   {"to_boolean", config_to_boolean},
   {"to_string", config_to_string},
   {"to_number", config_to_number},
   {"to_handle", config_to_handle},
   {"to_matrix", config_to_matrix},
   {"to_event_type", config_to_event_type},
   {"to_message", config_to_message},
   {"to_object_type", config_to_object_type},
   {"to_state", config_to_state},
   {"to_uuid", config_to_uuid},
   {"to_vector", config_to_vector},
   {"set_value", config_set_value},
   {"append_value", config_append_value},
   {"get_value", config_get_value},
   {"get_children", config_get_children},
   {"add_config", config_add_config},
   {"lookup_config", config_lookup_config},
   {"lookup_all_config", config_lookup_all_config},
   {"lookup_all_config_merged", config_lookup_all_config_merged},
   {"__tostring", config_tostring},
   {"__eq", config_equal},
   {"__gc", config_delete},
   {NULL, NULL},
};

};


//! \cond
void
dmz::open_lua_kernel_config_lib (lua_State *L, Config &global) {

   LUA_START_VALIDATE (L);

   luaL_newmetatable (L, ConfigName);
   luaL_register (L, NULL, arrayMembers);
   lua_pushvalue (L, -1);
   lua_setfield (L, -2, "__index");
   lua_create_dmz_namespace (L, "config");
   luaL_register (L, NULL, arrayFunc);
   lua_make_readonly (L, -1); // make config read only.
   lua_pop (L, 2); // pops meta table and dmz.config table.

   lua_pushstring (L, ConfigGlobalName);
   Config *ptr (lua_create_config (L, ""));
   if (ptr) { *ptr = global; }
   lua_rawset (L, LUA_REGISTRYINDEX);

   LUA_END_VALIDATE (L, 0);
}
//! \endcond


//! \addtogroup Lua
//! @{

//! Raises an error if the specified object is not a Config object.
dmz::Config *
dmz::lua_check_config (lua_State *L, int narg) {

   Config *result (0);
   Config **value = config_check (L, narg);
   if (value) { result = *value; }

   return result;
}


//! Checks if the specified object is a Config object.
dmz::Config *
dmz::lua_to_config (lua_State *L, int index) {

   Config *result (0);
   Config **value = (Config **)lua_is_object(L, index, ConfigName);
   if (value) { result = *value; }

   return result;
}


//! Creates a Config object on the stack.
dmz::Config *
dmz::lua_create_config (lua_State *L, const String &Name) {

   Config *result (0);

   Config **ptr = (Config **)lua_newuserdata (L, sizeof (Config *));

   if (ptr) {

      *ptr = 0;

      luaL_getmetatable (L, ConfigName);
      lua_setmetatable (L, -2);

      if (Name) { *ptr = new Config (Name); }
      else { *ptr = new Config; }

      result = *ptr;
   }

   return result;
}
//! @}
