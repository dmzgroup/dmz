#include <dmzLuaKernel.h>
#include "dmzLuaKernelPrivate.h"
#include <dmzLuaKernelValidate.h>
#include <dmzRuntimeEventType.h>
#include <dmzSystemStreamString.h>
#include <dmzTypesString.h>
#include <luacpp.h>

using namespace dmz;

namespace {

static const char EventTypeName[] = "dmz.runtime.event_type";

inline EventType**
event_type_check (lua_State *L, int index) {

   if (index < 0) { index = lua_gettop (L) + index + 1; }

   EventType **result = 0;

   if (lua_isstring (L, index)) {

      String name = lua_tostring (L, index);

      if (name) {

         EventType *ptr = lua_create_event_type (L);

         if (ptr) {

            ptr->set_type (name, lua_get_runtime_context (L));

            if (!(*ptr)) {

               String error ("Invalid event type name: ");
               error << name;
               lua_pushstring (L, error.get_buffer ());
               lua_error (L);
            }

            lua_replace (L, index);

            result = event_type_check (L, index);
         }
      }
      else {

         lua_pushstring (L, "Empty string can not be converted to an event type");
         lua_error (L);
      }
   }
   else { result = (EventType **)luaL_checkudata (L, index, EventTypeName); }

   return result;
}


static int
event_type_new (lua_State *L) {

   int result (0);

   int stackCount = lua_gettop (L);

   EventType *ptr = lua_create_event_type (L);

   if (ptr) {

      result = 1;

      if (0 == stackCount) {

         // do nothing
      }
      else if (1 == stackCount) {

         EventType **eventPtr = event_type_check (L, 1);

         if (eventPtr && *eventPtr) { *ptr = **eventPtr; }
      }
      else {

         luaL_error (L, "Too many parameters.");
      }
   }

   return result;
}


static int
event_type_is_a (lua_State *L) {

   if (lua_to_event_type (L, 1)) { lua_pushvalue (L, 1); }
   else { lua_pushnil (L); }

   return 1;
}


static const luaL_Reg arrayFunc [] = {
   {"new", event_type_new},
   {"is_a", event_type_is_a},
   {NULL, NULL},
};


static int
event_type_equal (lua_State *L) {

   int result (0);

   EventType **event1 = event_type_check (L, 1);
   EventType **event2 = event_type_check (L, 2);

   if (event1 && event2 && *event1 && *event2) {

      lua_pushboolean (L, **event1 == **event2 ? 1 : 0);
      result = 1;
   }

   return result;
}


static int
event_type_to_string (lua_State *L) {

   int result (0);

   EventType **event = event_type_check (L, 1);

   if (event && *event) {

      lua_pushstring (L, (*event)->get_name ().get_buffer ());
      result = 1;
   }

   return result;
}


static int
event_type_is_of_type (lua_State *L) {

   int result (0);

   EventType **event1 = event_type_check (L, 1);
   EventType **event2 = event_type_check (L, 2);

   if (event1 && event2 && *event1 && *event2) {

      lua_pushboolean (L, (*event1)->is_of_type (**event2) ? 1 : 0);
      result = 1;
   }

   return result;
}


static int
event_type_get_handle (lua_State *L) {

   int result (0);

   EventType **event = event_type_check (L, 1);

   if (event && *event) {

      Handle *ptr = lua_create_handle (L, (*event)->get_handle ());

      if (ptr) { result = 1; }
      else { lua_pop (L, 1); } // pop nil
   }

   return result;
}


static int
event_type_get_parent (lua_State *L) {

   int result (0);

   EventType **event = event_type_check (L, 1);

   if (event && *event) {

      EventType parent ((*event)->get_parent ());

      if (parent) {

         if (lua_create_event_type (L, &parent)) { result = 1; }
      }
      else { lua_pushnil (L); result = 1; }
   }

   return result;
}


static int
event_type_become_parent (lua_State *L) {

   int result (0);

   EventType **event = event_type_check (L, 1);

   if (event && *event) {

      lua_pushboolean (L, (*event)->become_parent () ? 1 : 0);
      result = 1;
   }

   return result;
}


static int
event_type_get_children (lua_State *L) {

   int result (0);

   EventType **event = event_type_check (L, 1);

   if (event && *event) {

      RuntimeIterator it;

      EventType type;

      int index (1);

      if ((*event)->get_first_child (it, type)) {

         lua_newtable (L);
         const int Table = lua_gettop (L);

         do {

            lua_create_event_type (L, &type);
            lua_rawseti (L, Table, index);
            index++;

         } while ((*event)->get_next_child (it, type));
      }
      else { lua_pushnil (L); }

      result = 1;
   }

   return result;
}


static int
event_type_delete (lua_State *L) {

   EventType **event = event_type_check (L, 1);
   if (event && *event) { delete (*event); *event = 0; }

   return 0;
}


static const luaL_Reg arrayMembers [] = {
   {"__eq", event_type_equal},
   {"__tostring", event_type_to_string},
   {"is_of_type", event_type_is_of_type},
   {"get_name", event_type_to_string},
   {"get_handle", event_type_get_handle},
   {"get_parent", event_type_get_parent},
   {"become_parent", event_type_become_parent},
   {"get_children", event_type_get_children},
   {"__gc", event_type_delete},
   {NULL, NULL},
};

};


//! \cond
void
dmz::open_lua_kernel_event_type_lib (lua_State *L) {

   LUA_START_VALIDATE (L);

   luaL_newmetatable (L, EventTypeName);
   luaL_register (L, NULL, arrayMembers);
   lua_pushvalue (L, -1);
   lua_setfield (L, -2, "__index");
   lua_create_dmz_namespace (L, "event_type");
   luaL_register (L, NULL, arrayFunc);
   lua_make_readonly (L, -1); // make event_type read only.
   lua_pop (L, 2); // pops meta table and dmz.event_type table.

   LUA_END_VALIDATE (L, 0);
}
//! \endcond


//! \addtogroup Lua
//! @{

//! Creates an EventType on the Lua stack.
dmz::EventType *
dmz::lua_create_event_type (lua_State *L, const EventType *Value) {

   EventType *result (0);

   EventType **ptr = (EventType **)lua_newuserdata (L, sizeof (EventType *));

   if (ptr) {

      *ptr = new EventType;

       if (*ptr && Value) { (**ptr) = *Value; }

      luaL_getmetatable (L, EventTypeName);
      lua_setmetatable (L, -2);

      result = *ptr;
   }

   return result;
}


//! Attempts to convert the specified object on the Lua stack to an EventType.
dmz::EventType *
dmz::lua_to_event_type (lua_State *L, int narg) {

   EventType *result (0);
   EventType **value = (EventType **) lua_is_object (L, narg, EventTypeName);
   if (value) { result = *value; }

   return result;
}


//! Raises an error if the specified object on the Lua stack is not an EventType.
dmz::EventType *
dmz::lua_check_event_type (lua_State *L, int narg) {

   EventType *result (0);
   EventType **value = event_type_check (L, narg);
   if (value) { result = *value; }

   return result;
}
//! @}
