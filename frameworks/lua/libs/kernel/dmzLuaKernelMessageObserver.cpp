#include <dmzLuaKernel.h>
#include "dmzLuaKernelPrivate.h"
#include <dmzLuaKernelValidate.h>
#include <dmzLuaKernelErrorHandler.h>
#include <dmzRuntimeData.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimeMessaging.h>
#include <dmzSystemStreamString.h>
#include <dmzTypesHashTableHandleTemplate.h>
#include <dmzTypesString.h>
#include <luacpp.h>

using namespace dmz;

namespace {

static const char MessageObserverLuaName[] = "dmz.runtime.message_observer";

class MessageObserverLua : public MessageObserver {

   public:
      MessageObserverLua (
         const Handle ObsHandle,
         const String &Name,
         RuntimeContext *context,
         lua_State *LState);

      ~MessageObserverLua ();

      virtual void receive_message (
         const Message &Type,
         const Handle MessageSendHandle,
         const Handle TargetObserverHandle,
         const Data *InData,
         Data *outData);

      Boolean register_callback (const Message &Type, const int Index);
      Boolean release_callback (const Message &Type);
      void release_all ();

   protected:
      lua_State *L;
      HashTableHandleTemplate<int> _table;
};

MessageObserverLua::MessageObserverLua (
      const Handle ObsHandle,
      const String &Name,
      RuntimeContext *context,
      lua_State *LState) :
      MessageObserver (ObsHandle, Name, context),
      L (LState) {

}


MessageObserverLua::~MessageObserverLua () { release_all (); }


void
MessageObserverLua::receive_message (
      const Message &Type,
      const Handle MessageSendHandle,
      const Handle TargetObserverHandle,
      const Data *InData,
      Data *outData) {

   LUA_START_VALIDATE (L);

   Message current (Type);

   int *ptr (0);

   while (current && !ptr) {

      ptr = _table.lookup (current.get_handle ());

      if (!ptr) { current.become_parent (); }
   }

   if (ptr && L) {

      lua_rawgeti (L, LUA_REGISTRYINDEX, *ptr);

      if (lua_istable (L, -1)) {

         const int CallBackTable = lua_gettop (L);

         lua_pushcfunction (L, lua_error_handler);

         const int Handler (lua_gettop (L));

         lua_rawgeti (L, CallBackTable, 1);

         if (lua_isfunction (L, -1)) {

            lua_rawgeti (L, CallBackTable, 2);
            lua_create_message (L, &Type);

            if (InData) { lua_create_data (L, InData); }
            else { lua_pushnil (L); }

            Data *outPtr (0);
            if (outData) { outPtr = lua_create_data (L); }
            else { lua_pushnil (L); }

            lua_pushnumber (L, (lua_Number)MessageSendHandle);
            lua_create_handle (L, TargetObserverHandle);

            if (lua_pcall (L, 6, 0, Handler)) {

               release_callback (current);

               lua_pop (L, 1); // error message
            }

            if (outData && outPtr) { *outData = *outPtr; }
         }

         lua_pop (L, 1); // pop error handler function
      }

      lua_pop (L, 1); // pop the function table
   }

   LUA_END_VALIDATE (L, 0);
}


Boolean
MessageObserverLua::register_callback (const Message &Type, const int Index) {

   Boolean result (True);

   int *ptr = new int (Index);

   if (!_table.store (Type.get_handle (), ptr)) { delete ptr; ptr = 0; result = False; }
   else { subscribe_to_message (Type); }

   return result;
}


Boolean
MessageObserverLua::release_callback (const Message &Type) {

   Boolean result (False);

   int *ptr (_table.remove (Type.get_handle ()));

   if (ptr) {

      luaL_unref (L, LUA_REGISTRYINDEX, *ptr);
      delete ptr; ptr = 0;
      result = unsubscribe_to_message (Type);
   }

   return result;
}


void
MessageObserverLua::release_all () {

   HashTableHandleIterator it;

   int *ptr (_table.get_first (it));

   while (ptr) {

      luaL_unref (L, LUA_REGISTRYINDEX, *ptr);
      ptr = _table.get_next (it);
   }

   _table.empty ();

   unsubscribe_to_all_messages ();
}


inline MessageObserverLua**
message_obs_check (lua_State *L, int index) {

   return (MessageObserverLua **)luaL_checkudata (L, index, MessageObserverLuaName);
}


static int
message_obs_new (lua_State *L) {

   const String Name (luaL_checkstring (L, 1));

   MessageObserverLua *result (0);

   RuntimeContext *context (lua_get_runtime_context (L));
   Definitions defs (context);

   const Handle ObsHandle (defs.create_named_handle (Name));

   MessageObserverLua **ptr =
      (MessageObserverLua **)lua_newuserdata (L, sizeof (MessageObserverLua *));

   if (ptr) {

      *ptr = new MessageObserverLua (ObsHandle, Name, context, L);

      luaL_getmetatable (L, MessageObserverLuaName);
      lua_setmetatable (L, -2);

      result = *ptr;
   }

   return result ? 1 : 0;
}


static int
message_obs_is_a (lua_State *L) {

   if (lua_is_object (L, 1, MessageObserverLuaName)) { lua_pushvalue (L, 1); }
   else { lua_pushnil (L); }

   return 1;
}


static const luaL_Reg arrayFunc [] = {
   {"new", message_obs_new},
   {"is_a", message_obs_is_a},
   {NULL, NULL},
};


static int
message_obs_register (lua_State *L) {

   int result (0);

   MessageObserverLua **obs = message_obs_check (L, 1);
   Message *type = lua_check_message (L, 2);
   luaL_checktype (L, 3, LUA_TFUNCTION);
   luaL_checktype (L, 4, LUA_TTABLE);

   if (obs && *obs) {

      lua_createtable (L, 2, 0);
      const int Table = lua_gettop (L);

      lua_pushvalue (L, 3);
      lua_rawseti (L, Table, 1);

      lua_pushvalue (L, 4);
      lua_rawseti (L, Table, 2);

      int index = luaL_ref (L, LUA_REGISTRYINDEX);

      lua_pushboolean (L, (*obs)->register_callback (*type, index) ? 1 : 0);
      result = 1;
   }

   return result;
}


static int
message_obs_release (lua_State *L) {

   int result (0);

   MessageObserverLua **obs = message_obs_check (L, 1);
   Message *type = lua_check_message (L, 2);

   if (obs && *obs) {

      lua_pushboolean (L, (*obs)->release_callback (*type) ? 1 : 0);
      result = 1;
   }

   return result;
}


static int
message_obs_release_all (lua_State *L) {

   MessageObserverLua **obs = message_obs_check (L, 1);

   if (obs && *obs) { (*obs)->release_all (); }

   return 0;
}


static int
message_obs_delete (lua_State *L) {

   MessageObserverLua **obs = message_obs_check (L, 1);
   if (obs && *obs) { delete (*obs); *obs = 0; }

   return 0;
}


static const luaL_Reg arrayMembers [] = {
   {"register", message_obs_register},
   {"release", message_obs_release},
   {"release_all", message_obs_release_all},
   {"__gc", message_obs_delete},
   {NULL, NULL},
};

};


//! \cond
void
dmz::open_lua_kernel_message_observer_lib (lua_State *L) {

   LUA_START_VALIDATE (L);

   luaL_newmetatable (L, MessageObserverLuaName);
   luaL_register (L, NULL, arrayMembers);
   lua_pushvalue (L, -1);
   lua_setfield (L, -2, "__index");
   lua_create_dmz_namespace (L, "message_observer");
   luaL_register (L, NULL, arrayFunc);
   lua_make_readonly (L, -1); // make message_observer read only.
   lua_pop (L, 2); // pops meta table and dmz.message table.

   LUA_END_VALIDATE (L, 0);
//! \endcond
}

