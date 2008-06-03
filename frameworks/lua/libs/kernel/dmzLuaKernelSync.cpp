#include <dmzLuaKernel.h>
#include <dmzLuaKernelErrorHandler.h>
#include "dmzLuaKernelPrivate.h"
#include <dmzLuaKernelValidate.h>
#include <dmzRuntimeData.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimeSync.h>
#include <dmzTypesHashTableHandleTemplate.h>
#include <dmzTypesString.h>
#include <luacpp.h>

using namespace dmz;

namespace {

static const char SyncLuaName[] = "dmz.runtime.sync";

static int LuaSyncRuntime = 0;
static int LuaSyncSystem = 0;
static int LuaSyncSingle = 0;
static int LuaSyncRepeating = 0;

static const char LuaSyncKey = 's';

static SyncModeEnum
to_sync_mode (lua_State *L, int index) {

   SyncModeEnum result (SyncModeSingle);

   const int LuaMode = luaL_checkinteger (L, index);
   if (LuaMode == LuaSyncSingle) { result = SyncModeSingle; }
   else if (LuaMode == LuaSyncRepeating) { result = SyncModeRepeating; }
   else { lua_enum_error (L, index, LuaMode); }

   return result;
}


static SyncTypeEnum
to_sync_type (lua_State *L, int index) {

   SyncTypeEnum result (SyncTypeRuntime);

   const int LuaType = luaL_checkinteger (L, index);
   if (LuaType == LuaSyncRuntime) { result = SyncTypeRuntime; }
   else if (LuaType == LuaSyncSystem) { result = SyncTypeSystemTime; }
   else { lua_enum_error (L, index, LuaType); }

   return result;
}


static int
from_sync_mode (const SyncModeEnum Value) {

   int result (0);
   if (Value == SyncModeSingle) { result = LuaSyncSingle; }
   else if (Value == SyncModeRepeating) { result = LuaSyncRepeating; }

   return result;
}


static int
from_sync_type (const SyncTypeEnum Value) {

   int result (0);
   if (Value == SyncTypeRuntime) { result = LuaSyncRuntime; }
   else if (Value == SyncTypeSystemTime) { result = LuaSyncSystem; }

   return result;
}


class SyncLuaInstance : public Sync {

   public:
      SyncLuaInstance (
         const int TheIndex,
         const SyncTypeEnum Type,
         const SyncModeEnum Mode,
         const Float64 TimeInterval,
         const String &Name,
         RuntimeContext *context,
         lua_State *LState);

      virtual ~SyncLuaInstance ();

      // Sync Interface.
      virtual void update_sync (const Float64 TimeDelta);

      Boolean inSync;

   protected:
      const int _Index;
      lua_State *L;
};


SyncLuaInstance::SyncLuaInstance (
      const int TheIndex,
      const SyncTypeEnum Type,
      const SyncModeEnum Mode,
      const Float64 TimeInterval,
      const String &Name,
      RuntimeContext *context,
      lua_State *LState) :
      Sync (
         Name ? Definitions (context).create_named_handle (Name) : 0,
         Type,
         Mode,
         TimeInterval,
         context),
      inSync (False),
      _Index (TheIndex),
      L (LState) {

}


SyncLuaInstance::~SyncLuaInstance () {

   lua_pushlightuserdata (L, (void *)&LuaSyncKey);
   lua_rawget (L, LUA_REGISTRYINDEX);
   luaL_unref (L, -1, _Index);
   lua_pop (L, 1); // pop sync table;
}


void
SyncLuaInstance::update_sync (const Float64 TimeDelta) {

   inSync = True;

   LUA_START_VALIDATE (L);

   lua_pushlightuserdata (L, (void *)&LuaSyncKey);
   lua_rawget (L, LUA_REGISTRYINDEX);
   const int Table (lua_gettop (L));
   lua_rawgeti (L, Table, _Index);

   if (lua_istable (L, -1)) {

      const int CallBackTable = lua_gettop (L);
      lua_pushcfunction (L, lua_error_handler);
      const int Handler (lua_gettop (L));

      lua_rawgeti (L, CallBackTable, 1);

      if (lua_isfunction (L, -1)) {

         lua_rawgeti (L, CallBackTable, 2);
         lua_pushnumber (L, TimeDelta);
         lua_create_handle (L, get_sync_handle ());

         if (lua_pcall (L, 3, 0, Handler)) {

            lua_pop (L, 1); // pop error message

            luaL_unref (L, Table, _Index);
            stop_sync ();
         }
      }
      else { lua_pop (L, 1); } // pop rawgeti

      lua_pop (L, 1); // pop lua_error_handler
   }

   lua_pop (L, 2); // pop the function table and the sync table

   LUA_END_VALIDATE (L, 0);

   inSync = False;
}


class SyncLua {

   public:
      SyncLua (
         RuntimeContext *context,
         lua_State *LState);

      ~SyncLua ();

      Handle create_sync (
         const int TheIndex,
         const SyncTypeEnum Type,
         const SyncModeEnum Mode,
         const Float64 TimeInterval,
         const String &Name);

      Boolean start_sync (const Handle SyncHandle);
      Boolean stop_sync (const Handle SyncHandle);
      Boolean destroy_sync (const Handle SyncHandle);
      Boolean set_sync_interval (const Handle SyncHandle, const Float64 Interval);
      Float64 get_sync_interval (const Handle SyncHandle);
      Boolean get_sync_type (const Handle SyncHandle, SyncTypeEnum &value);
      Boolean set_sync_mode (const Handle SyncHandle, const SyncModeEnum Value);
      Boolean get_sync_mode (const Handle SyncHandle, SyncModeEnum &value);

   protected:
      void _clean_table ();

      RuntimeContext *_context;
      lua_State *L;
      HashTableHandleTemplate<SyncLuaInstance> _table;
      HashTableHandleTemplate<SyncLuaInstance> _cleanTable;
};


SyncLua::SyncLua (
      RuntimeContext *context,
      lua_State *LState) :
      _context (context),
      L (LState) {;}


SyncLua::~SyncLua () { _table.empty (); _cleanTable.empty (); }


Handle
SyncLua::create_sync (
      const int TheIndex,
      const SyncTypeEnum Type,
      const SyncModeEnum Mode,
      const Float64 TimeInterval,
      const String &Name) {

   _cleanTable.empty ();

   Handle result (0);

   SyncLuaInstance *sync (
      new SyncLuaInstance (TheIndex, Type, Mode, TimeInterval, Name, _context, L));

   if (sync) {

      result = sync->get_sync_handle ();

      if (!_table.store (result, sync)) {

         delete sync; sync = 0;
         result = 0;
      }
   }

   return result;
}


Boolean
SyncLua::start_sync (const Handle SyncHandle) {

   _cleanTable.empty ();

   Boolean result (False);

   SyncLuaInstance *sync (_table.lookup (SyncHandle));

   if (sync) { sync->start_sync (); result = True; }

   return result;
}


Boolean
SyncLua::stop_sync (const Handle SyncHandle) {

   _cleanTable.empty ();

   Boolean result (False);

   SyncLuaInstance *sync (_table.lookup (SyncHandle));

   if (sync) { sync->stop_sync (); result = True; }

   return result;
}


Boolean
SyncLua::destroy_sync (const Handle SyncHandle) {

   _cleanTable.empty ();

   Boolean result (False);

   SyncLuaInstance *sync (_table.remove (SyncHandle));

   if (sync) {

      result = True;

      if (sync->inSync) {

         sync->stop_sync ();
         _cleanTable.store (SyncHandle, sync);
      }
      else { delete sync; sync = 0; }
   }

   return result;
}


Boolean
SyncLua::set_sync_interval (const Handle SyncHandle, const Float64 Interval) {

   _cleanTable.empty ();

   Boolean result (False);

   SyncLuaInstance *sync (_table.lookup (SyncHandle));

   if (sync) { sync->set_sync_interval (Interval); result = True; }

   return result;
}


Float64
SyncLua::get_sync_interval (const Handle SyncHandle) {

   _cleanTable.empty ();

   Float64 result (0.0);

   SyncLuaInstance *sync (_table.lookup (SyncHandle));

   if (sync) { result = sync->get_sync_interval (); }

   return result;
}


Boolean
SyncLua::get_sync_type (const Handle SyncHandle, SyncTypeEnum &value) {

   _cleanTable.empty ();

   Boolean result (False);

   SyncLuaInstance *sync (_table.lookup (SyncHandle));

   if (sync) { value = sync->get_sync_type (); result = True; }

   return result;
}


Boolean
SyncLua::set_sync_mode (const Handle SyncHandle, const SyncModeEnum Value) {

   _cleanTable.empty ();

   Boolean result (False);

   SyncLuaInstance *sync (_table.lookup (SyncHandle));

   if (sync) { sync->set_sync_mode (Value); result = True; }

   return result;
}


Boolean
SyncLua::get_sync_mode (const Handle SyncHandle, SyncModeEnum &value) {

   _cleanTable.empty ();

   Boolean result (False);

   SyncLuaInstance *sync (_table.lookup (SyncHandle));

   if (sync) { value = sync->get_sync_mode (); result = True; }

   return result;
}


inline SyncLua**
sync_check (lua_State *L, int index) {

   return (SyncLua **)luaL_checkudata (L, index, SyncLuaName);
}


static int
sync_new (lua_State *L) {

   SyncLua *result (0);

   RuntimeContext *context (lua_get_runtime_context (L));

   SyncLua **ptr =
      (SyncLua **)lua_newuserdata (L, sizeof (SyncLua *));

   if (ptr) {

      *ptr = new SyncLua (context, L);

      luaL_getmetatable (L, SyncLuaName);
      lua_setmetatable (L, -2);

      result = *ptr;
   }

   return result ? 1 : 0;
}


static int
sync_is_a (lua_State *L) {

   if (lua_is_object (L, 1, SyncLuaName)) { lua_pushvalue (L, 1); }
   else { lua_pushnil (L); }

   return 1;
}


static const luaL_Reg arrayFunc [] = {
   {"new", sync_new},
   {"is_a", sync_is_a},
   {NULL, NULL},
};


static int
sync_create (lua_State *L) {

   int result (0);

   SyncLua **sync = sync_check (L, 1);
   luaL_checktype (L, 2, LUA_TFUNCTION);
   luaL_checktype (L, 3, LUA_TTABLE);
   String name;
   Float64 interval (0.0);
   SyncTypeEnum type (SyncTypeRuntime);
   SyncModeEnum mode (SyncModeRepeating);

   if (!lua_isnoneornil (L, 4)) { name = luaL_checkstring (L, 4); }
   if (!lua_isnoneornil (L, 5)) { interval = luaL_checknumber (L, 5); }
   if (!lua_isnoneornil (L, 6)) { mode = to_sync_mode (L, 6); }
   if (!lua_isnoneornil (L, 7)) { type = to_sync_type (L, 7); }

   if (sync && *sync) {

      lua_pushlightuserdata (L, (void *)&LuaSyncKey);
      lua_rawget (L, LUA_REGISTRYINDEX);
      const int SyncTable (lua_gettop (L));

      lua_createtable (L, 2, 0);
      const int Table = lua_gettop (L);

      lua_pushvalue (L, 2);
      lua_rawseti (L, Table, 1);

      lua_pushvalue (L, 3);
      lua_rawseti (L, Table, 2);

      int index = luaL_ref (L, SyncTable);
      lua_pop (L, 1); // pop sync table

      lua_create_handle (L, (*sync)->create_sync (index, type, mode, interval, name));
      result = 1;
   }

   return result;
}


static int
sync_destroy (lua_State *L) {

   int result (0);

   SyncLua **sync = sync_check (L, 1);
   Handle *ptr = lua_check_handle (L, 2);

   if (sync && *sync && ptr) {

      lua_pushboolean (L, (*sync)->destroy_sync (*ptr) ? 1 : 0);
      result = 1;
   }

   return result;
}

static int
sync_start (lua_State *L) {

   int result (0);

   SyncLua **sync = sync_check (L, 1);
   Handle *ptr = lua_check_handle (L, 2);

   if (sync && *sync && ptr) {

      lua_pushboolean (L, (*sync)->start_sync (*ptr) ? 1 : 0);
      result = 1;
   }

   return result;
}


static int
sync_stop (lua_State *L) {

   int result (0);

   SyncLua **sync = sync_check (L, 1);
   Handle *ptr = lua_check_handle (L, 2);

   if (sync && *sync && ptr) {

      lua_pushboolean (L, (*sync)->stop_sync (*ptr) ? 1 : 0);
      result = 1;
   }

   return result;
}


static int
sync_set_interval (lua_State *L) {

   int result (0);

   SyncLua **sync = sync_check (L, 1);
   Handle *ptr = lua_check_handle (L, 2);
   const Float64 Interval = luaL_checknumber (L, 3);

   if (sync && *sync && ptr) {

      lua_pushboolean (L, (*sync)->set_sync_interval (*ptr, Interval) ? 1 : 0);
      result = 1;
   }

   return result;
}


static int
sync_get_interval (lua_State *L) {

   int result (0);

   SyncLua **sync = sync_check (L, 1);
   Handle *ptr = lua_check_handle (L, 2);

   if (sync && *sync && ptr) {

      lua_pushnumber (L, (*sync)->get_sync_interval (*ptr));
      result = 1;
   }

   return result;
}

static int
sync_get_type (lua_State *L) {

   int result (0);
 
   SyncLua **sync = sync_check (L, 1);
   Handle *ptr = lua_check_handle (L, 2);

   if (sync && *sync && ptr) {

      SyncTypeEnum value (SyncTypeRuntime);
      (*sync)->get_sync_type (*ptr, value);
      lua_pushinteger (L, from_sync_type (value));
      result = 1;
   }

   return result;
}


static int
sync_set_mode (lua_State *L) {

   int result (0);
 
   SyncLua **sync = sync_check (L, 1);
   Handle *ptr = lua_check_handle (L, 2);

   if (sync && *sync && ptr) {

      SyncModeEnum value (to_sync_mode (L, 3));
      lua_pushboolean (L, (*sync)->set_sync_mode (*ptr, value) ? 1 : 0);
      result = 1;
   }

   return result;
}


static int
sync_get_mode (lua_State *L) {

   int result (0);
 
   SyncLua **sync = sync_check (L, 1);
   Handle *ptr = lua_check_handle (L, 2);

   if (sync && *sync && ptr) {

      SyncModeEnum value (SyncModeRepeating);
      (*sync)->get_sync_mode (*ptr, value);
      lua_pushinteger (L, from_sync_mode (value));
      result = 1;
   }

   return result;
}


static int
sync_delete (lua_State *L) {

   SyncLua **sync = sync_check (L, 1);
   if (sync && *sync) { delete (*sync); *sync = 0; }

   return 0;
}


static const luaL_Reg arrayMembers [] = {
   {"create", sync_create},
   {"start", sync_start},
   {"stop", sync_stop},
   {"set_interval", sync_set_interval},
   {"get_interval", sync_get_interval},
   {"get_type", sync_get_type},
   {"set_mode", sync_set_mode},
   {"get_mode", sync_get_mode},
   {"destroy", sync_destroy},
   {"__gc", sync_delete},
   {NULL, NULL},
};

};


void
dmz::open_lua_kernel_sync_lib (lua_State *L) {

   LUA_START_VALIDATE (L);

   lua_pushlightuserdata (L, (void *)&LuaSyncKey);
   lua_newtable (L);
   lua_rawset (L, LUA_REGISTRYINDEX);

   luaL_newmetatable (L, SyncLuaName);
   luaL_register (L, NULL, arrayMembers);
   lua_pushvalue (L, -1);
   lua_setfield (L, -2, "__index");
   lua_create_dmz_namespace (L, "sync");
   luaL_register (L, NULL, arrayFunc);

   LuaSyncRuntime = lua_create_enum (L, "dmzSyncTypeRuntime");
   LuaSyncSystem = lua_create_enum (L, "dmzSyncTypeSystemTime");
   LuaSyncSingle = lua_create_enum (L, "dmzSyncModeSingle");
   LuaSyncRepeating = lua_create_enum (L, "dmzSyncModeRepeating");

   lua_pushinteger (L, LuaSyncSystem);
   lua_setfield (L, -2, "System");
   lua_pushinteger (L, LuaSyncRuntime);
   lua_setfield (L, -2, "Runtime");
   lua_pushinteger (L, LuaSyncRepeating);
   lua_setfield (L, -2, "Repeating");
   lua_pushinteger (L, LuaSyncSingle);
   lua_setfield (L, -2, "Single");

   lua_make_readonly (L, -1); // make sync read only.
   lua_pop (L, 2); // pops meta table and dmz.sync table.

   LUA_END_VALIDATE (L, 0);
}

