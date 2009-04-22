#include "dmzLuaExtEvent.h"
#include "dmzLuaExtEventObserver.h"
#include <dmzLuaKernel.h>
#include <dmzLuaKernelErrorHandler.h>
#include <dmzLuaKernelValidate.h>
#include <dmzEventCallbackMasks.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeEventType.h>
#include <dmzTypesMask.h>

#include <luacpp.h>

#ifdef _WIN32
// This disables the warning regarding using the 'this' pointer as an
// argument in the constructor. The pointer isn't actually used for anything
// other than its value.
#pragma warning (disable : 4355)
#endif // _Win32


using namespace dmz;

namespace {

typedef LuaExtEventObserver * optr;

static const char EventObserverName[] = "dmz.event.observer";

static const char EventCreateFunc[] = "create_event";
static const char EventCloseFunc[]   = "close_event";

static inline optr
obs_check (lua_State *L, int index) {

   optr *obs = (optr *)luaL_checkudata (L, index, EventObserverName);

   return obs ? *obs : 0;
}


static inline optr
obs_to (lua_State *L, int index) {

   optr *obs = (optr *)lua_is_object (L, index, EventObserverName);

   return obs ? *obs : 0;
}


static int
obs_delete (lua_State *L) {

   optr *obs = (optr *)luaL_checkudata (L, 1, EventObserverName);

   if (obs && *obs) {

      lua_release_event_observer (L, **obs);
      delete *obs; *obs = 0;
   }

   return 0;
}


static const int
obs_setup_cb (
      lua_State *L,
      LuaExtEventObserver &obs,
      const EventType &Type,
      const String &FuncName) {

   int result (0);

   Boolean error (False);

   lua_pushcfunction (L, lua_error_handler);
   result = lua_gettop (L);

   lua_rawgeti (L, LUA_REGISTRYINDEX, obs.get_index ());
   const int IndexTable = lua_gettop (L);

   if (lua_istable (L, IndexTable)) {

      lua_create_handle (L, Type.get_handle ());
      lua_rawget (L, IndexTable);
      const int TypeTable = lua_gettop (L);

      if (lua_istable (L, TypeTable)) {

         lua_rawgeti (L, TypeTable, 1);

         if (lua_istable (L, -1)) {

            lua_getfield (L, -1, FuncName.get_buffer ());

            if (lua_isfunction (L, -1)) { lua_rawgeti (L, TypeTable, 2); }
         }
         else { error = True; }
      }
      else { error = True; }
   }
   else { error = True; }

   if (error) {

      lua_pop (L, lua_gettop (L) - (result - 1));
      result = 0;
   }

   return result;
}


static void
obs_do_cb (
      lua_State *L,
      LuaExtEventObserver &obs,
      const int ArgCount,
      const int Handler,
      const EventType &Type,
      const Mask &CBMask) {

   if (Handler) {

      if (lua_pcall (L, ArgCount + 1, 0, Handler)) {

         lua_pop (L, 1); // pop error message

         obs.deactivate_event_callback (Type, CBMask);
      }

      lua_pop (L, lua_gettop (L) - (Handler - 1));
   }
   else {

      lua_pop (L, ArgCount);
      obs.deactivate_event_callback (Type, CBMask);
   }
}


static const Mask &
obs_has_func (lua_State *L, int index, const String &Name, const Mask &FunctionMask) {

   static const Mask EmptyMask;

   if (index < 0) { index = lua_gettop (L) + index + 1; }

   lua_getfield (L, index, Name.get_buffer ());

   const Boolean Result = (lua_isfunction (L, -1) == 1);

   lua_pop (L, 1);

   return Result ? FunctionMask : EmptyMask;
}


int
obs_register (lua_State *L) {

   LUA_START_VALIDATE (L);

   int result (0);

   optr obs (obs_check (L, 1));
   EventType *type = lua_check_event_type (L, 2);
   luaL_checktype (L, 3, LUA_TTABLE);
   int data = (lua_isnoneornil (L, 4) ? 3 : 4);

   if (obs && type) {

      Mask cb;

      cb |= obs_has_func (L, 3, EventCreateFunc, EventCreateMask);
      cb |= obs_has_func (L, 3, EventCloseFunc, EventCloseMask);

      Mask unreg (EventAllMask);

      unreg ^= cb;
      obs->deactivate_event_callback (*type, unreg);

      lua_rawgeti (L, LUA_REGISTRYINDEX, obs->get_index ()); // get observer index table
      const int CBTable (lua_gettop (L));
      lua_create_handle (L, type->get_handle ()); // Push callback handle
      lua_createtable (L, 2, 0); // create table for callback and data tables
      const int Table (lua_gettop (L));
      lua_pushvalue (L, 3); // push callback table
      lua_rawseti (L, Table, 1); // store callback table
      lua_pushvalue (L, data); // push either callback or data table if it exists
      lua_rawseti (L, Table, 2); // store data table
      lua_rawset (L, CBTable); // store table of callback and data tables
      lua_pop (L, 1); // pop observer index table.

      obs->activate_event_callback (*type, cb);
   }

   LUA_END_VALIDATE (L, result);

   return result;
}


static int
obs_release (lua_State *L) {

   int result (0);

   optr obs (obs_check (L, 1));
   EventType *type (lua_check_event_type (L, 2));
   Mask *mask (lua_check_mask (L, 3));

   if (obs && type && mask) {

      obs->deactivate_event_callback (*type, *mask);
      lua_pushboolean (L, 1);
      result = 1;
   }

   return result;
}


static const luaL_Reg arrayMembers [] = {
   {"register", obs_register},
   {"release", obs_release},
   {"__gc", obs_delete},
   {NULL, NULL},
};


static int
obs_new (lua_State *L) { return lua_create_event_observer (L) ? 1 : 0; }


static const luaL_Reg arrayFunc [] = {
   {"new", obs_new},
   {NULL, NULL},
};

};


//! \cond
void
dmz::lua_init_event_observer (lua_State *L) {

   luaL_newmetatable (L, EventObserverName);
   luaL_register (L, NULL, arrayMembers);
   lua_pushvalue (L, -1);
   lua_setfield (L, -2, "__index");

   lua_create_dmz_namespace (L, "event_observer");
   luaL_register (L, NULL, arrayFunc);
   lua_make_readonly (L, -1);
   lua_pop (L, 2);
}


dmz::LuaExtEventObserver *
dmz::lua_create_event_observer (lua_State *L) {

   LuaExtEventObserver *result (0);

   LuaExtEventObserver **ptr =
      (LuaExtEventObserver **)lua_newuserdata (L, sizeof (LuaExtEventObserver));

   if (ptr) {

      Config empty;

      *ptr = new LuaExtEventObserver (L, empty);

      if (*ptr) {

         lua_register_event_observer (L, **ptr);
         luaL_getmetatable (L, EventObserverName);
         lua_setmetatable (L, -2);
      }

      result = *ptr;
   }

   return result;
}


dmz::LuaExtEventObserver *
dmz::lua_check_event_observer (lua_State *L, int index) {

    optr *obs = (optr *)luaL_checkudata (L, index, EventObserverName);

    return obs ? *obs : 0;
}


dmz::LuaExtEventObserver::LuaExtEventObserver (lua_State *TheState, Config &empty) :
      PluginInfo (
         "",
         PluginDeleteModeDoNotDelete,
         lua_get_runtime_context (TheState),
         0),
      EventObserverUtil (*((PluginInfo *)this), empty),
      _Handle ("LuaExtEventObserver", lua_get_runtime_context (TheState)),
      L (TheState),
      _index (0) {

   lua_newtable (L);
   _index = luaL_ref (L, LUA_REGISTRYINDEX);
}


dmz::LuaExtEventObserver::~LuaExtEventObserver () {;}


void
dmz::LuaExtEventObserver::create_event (
      const Handle EventHandle,
      const EventType &Type,
      const EventLocalityEnum Locality) {

   LUA_START_VALIDATE (L);

   const int Handler (obs_setup_cb (L, *this, Type, EventCreateFunc));

   if (Handler) {

      lua_create_handle (L, EventHandle);
      lua_create_event_type (L, &Type);
      lua_pushinteger (L, lua_event_locality_to_int (L, Locality));

      obs_do_cb (L, *this, 3, Handler, Type, EventCreateMask);
   }

   LUA_END_VALIDATE (L, 0);
}


void
dmz::LuaExtEventObserver::close_event (
      const Handle EventHandle,
      const EventType &Type,
      const EventLocalityEnum Locality) {

   LUA_START_VALIDATE (L);

   const int Handler (obs_setup_cb (L, *this, Type, EventCloseFunc));

   if (Handler) {

      lua_create_handle (L, EventHandle);
      lua_create_event_type (L, &Type);
      lua_pushinteger (L, lua_event_locality_to_int (L, Locality));

      obs_do_cb (L, *this, 3, Handler, Type, EventCloseMask);
   }

   LUA_END_VALIDATE (L, 0);
}

//! \endcond

