#include <dmzLuaKernel.h>
#include <dmzLuaKernelErrorHandler.h>
#include "dmzLuaKernelPrivate.h"
#include <dmzLuaKernelValidate.h>
#include <dmzRuntimeData.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimeTimeSlice.h>
#include <dmzTypesHashTableHandleTemplate.h>
#include <dmzTypesString.h>
#include <luacpp.h>

using namespace dmz;

namespace {

static const char TimeSliceLuaName[] = "dmz.runtime.time_slice";

static int LuaTimeSliceRuntime = 0;
static int LuaTimeSliceSystem = 0;
static int LuaTimeSliceSingle = 0;
static int LuaTimeSliceRepeating = 0;

static const char LuaTimeSliceKey = 's';

static TimeSliceModeEnum
to_time_slice_mode (lua_State *L, int index) {

   TimeSliceModeEnum result (TimeSliceModeSingle);

   const int LuaMode = luaL_checkinteger (L, index);
   if (LuaMode == LuaTimeSliceSingle) { result = TimeSliceModeSingle; }
   else if (LuaMode == LuaTimeSliceRepeating) { result = TimeSliceModeRepeating; }
   else { lua_enum_error (L, index, LuaMode); }

   return result;
}


static TimeSliceTypeEnum
to_time_slice_type (lua_State *L, int index) {

   TimeSliceTypeEnum result (TimeSliceTypeRuntime);

   const int LuaType = luaL_checkinteger (L, index);
   if (LuaType == LuaTimeSliceRuntime) { result = TimeSliceTypeRuntime; }
   else if (LuaType == LuaTimeSliceSystem) { result = TimeSliceTypeSystemTime; }
   else { lua_enum_error (L, index, LuaType); }

   return result;
}


static int
from_time_slice_mode (const TimeSliceModeEnum Value) {

   int result (0);
   if (Value == TimeSliceModeSingle) { result = LuaTimeSliceSingle; }
   else if (Value == TimeSliceModeRepeating) { result = LuaTimeSliceRepeating; }

   return result;
}


static int
from_time_slice_type (const TimeSliceTypeEnum Value) {

   int result (0);
   if (Value == TimeSliceTypeRuntime) { result = LuaTimeSliceRuntime; }
   else if (Value == TimeSliceTypeSystemTime) { result = LuaTimeSliceSystem; }

   return result;
}


class TimeSliceLuaInstance : public TimeSlice {

   public:
      TimeSliceLuaInstance (
         const int TheIndex,
         const TimeSliceTypeEnum Type,
         const TimeSliceModeEnum Mode,
         const Float64 TimeInterval,
         const String &Name,
         RuntimeContext *context,
         lua_State *LState);

      virtual ~TimeSliceLuaInstance ();

      // TimeSlice Interface.
      virtual void update_time_slice (const Float64 TimeDelta);

      Boolean inTimeSlice;

   protected:
      const int _Index;
      lua_State *L;
};


TimeSliceLuaInstance::TimeSliceLuaInstance (
      const int TheIndex,
      const TimeSliceTypeEnum Type,
      const TimeSliceModeEnum Mode,
      const Float64 TimeInterval,
      const String &Name,
      RuntimeContext *context,
      lua_State *LState) :
      TimeSlice (
         Name ? Definitions (context).create_named_handle (Name) : 0,
         Type,
         Mode,
         TimeInterval,
         context),
      inTimeSlice (False),
      _Index (TheIndex),
      L (LState) {

}


TimeSliceLuaInstance::~TimeSliceLuaInstance () {

   lua_pushlightuserdata (L, (void *)&LuaTimeSliceKey);
   lua_rawget (L, LUA_REGISTRYINDEX);
   luaL_unref (L, -1, _Index);
   lua_pop (L, 1); // pop time_slice table;
}


void
TimeSliceLuaInstance::update_time_slice (const Float64 TimeDelta) {

   inTimeSlice = True;

   LUA_START_VALIDATE (L);

   lua_pushlightuserdata (L, (void *)&LuaTimeSliceKey);
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
         lua_create_handle (L, get_time_slice_handle ());

         if (lua_pcall (L, 3, 0, Handler)) {

            lua_pop (L, 1); // pop error message

            luaL_unref (L, Table, _Index);
            stop_time_slice ();
         }
      }
      else { lua_pop (L, 1); } // pop rawgeti

      lua_pop (L, 1); // pop lua_error_handler
   }

   lua_pop (L, 2); // pop the function table and the time_slice table

   LUA_END_VALIDATE (L, 0);

   inTimeSlice = False;
}


class TimeSliceLua {

   public:
      TimeSliceLua (
         RuntimeContext *context,
         lua_State *LState);

      ~TimeSliceLua ();

      Handle create_time_slice (
         const int TheIndex,
         const TimeSliceTypeEnum Type,
         const TimeSliceModeEnum Mode,
         const Float64 TimeInterval,
         const String &Name);

      Boolean start_time_slice (const Handle TimeSliceHandle);
      Boolean stop_time_slice (const Handle TimeSliceHandle);
      Boolean destroy_time_slice (const Handle TimeSliceHandle);

      Boolean set_time_slice_interval (
         const Handle TimeSliceHandle,
         const Float64 Interval);

      Float64 get_time_slice_interval (const Handle TimeSliceHandle);

      Boolean get_time_slice_type (
         const Handle TimeSliceHandle,
         TimeSliceTypeEnum &value);

      Boolean set_time_slice_mode (
         const Handle TimeSliceHandle,
         const TimeSliceModeEnum Value);

      Boolean get_time_slice_mode (
         const Handle TimeSliceHandle,
         TimeSliceModeEnum &value);

   protected:
      void _clean_table ();

      RuntimeContext *_context;
      lua_State *L;
      HashTableHandleTemplate<TimeSliceLuaInstance> _table;
      HashTableHandleTemplate<TimeSliceLuaInstance> _cleanTable;
};


TimeSliceLua::TimeSliceLua (
      RuntimeContext *context,
      lua_State *LState) :
      _context (context),
      L (LState) {;}


TimeSliceLua::~TimeSliceLua () { _table.empty (); _cleanTable.empty (); }


Handle
TimeSliceLua::create_time_slice (
      const int TheIndex,
      const TimeSliceTypeEnum Type,
      const TimeSliceModeEnum Mode,
      const Float64 TimeInterval,
      const String &Name) {

   _cleanTable.empty ();

   Handle result (0);

   TimeSliceLuaInstance *ts (
      new TimeSliceLuaInstance (TheIndex, Type, Mode, TimeInterval, Name, _context, L));

   if (ts) {

      result = ts->get_time_slice_handle ();

      if (!_table.store (result, ts)) {

         delete ts; ts = 0;
         result = 0;
      }
   }

   return result;
}


Boolean
TimeSliceLua::start_time_slice (const Handle TimeSliceHandle) {

   _cleanTable.empty ();

   Boolean result (False);

   TimeSliceLuaInstance *ts (_table.lookup (TimeSliceHandle));

   if (ts) { ts->start_time_slice (); result = True; }

   return result;
}


Boolean
TimeSliceLua::stop_time_slice (const Handle TimeSliceHandle) {

   _cleanTable.empty ();

   Boolean result (False);

   TimeSliceLuaInstance *ts (_table.lookup (TimeSliceHandle));

   if (ts) { ts->stop_time_slice (); result = True; }

   return result;
}


Boolean
TimeSliceLua::destroy_time_slice (const Handle TimeSliceHandle) {

   _cleanTable.empty ();

   Boolean result (False);

   TimeSliceLuaInstance *ts (_table.remove (TimeSliceHandle));

   if (ts) {

      result = True;

      if (ts->inTimeSlice) {

         ts->stop_time_slice ();
         _cleanTable.store (TimeSliceHandle, ts);
      }
      else { delete ts; ts = 0; }
   }

   return result;
}


Boolean
TimeSliceLua::set_time_slice_interval (
      const Handle TimeSliceHandle,
      const Float64 Interval) {

   _cleanTable.empty ();

   Boolean result (False);

   TimeSliceLuaInstance *ts (_table.lookup (TimeSliceHandle));

   if (ts) { ts->set_time_slice_interval (Interval); result = True; }

   return result;
}


Float64
TimeSliceLua::get_time_slice_interval (const Handle TimeSliceHandle) {

   _cleanTable.empty ();

   Float64 result (0.0);

   TimeSliceLuaInstance *ts (_table.lookup (TimeSliceHandle));

   if (ts) { result = ts->get_time_slice_interval (); }

   return result;
}


Boolean
TimeSliceLua::get_time_slice_type (
      const Handle TimeSliceHandle,
      TimeSliceTypeEnum &value) {

   _cleanTable.empty ();

   Boolean result (False);

   TimeSliceLuaInstance *ts (_table.lookup (TimeSliceHandle));

   if (ts) { value = ts->get_time_slice_type (); result = True; }

   return result;
}


Boolean
TimeSliceLua::set_time_slice_mode (
      const Handle TimeSliceHandle,
      const TimeSliceModeEnum Value) {

   _cleanTable.empty ();

   Boolean result (False);

   TimeSliceLuaInstance *ts (_table.lookup (TimeSliceHandle));

   if (ts) { ts->set_time_slice_mode (Value); result = True; }

   return result;
}


Boolean
TimeSliceLua::get_time_slice_mode (
      const Handle TimeSliceHandle,
      TimeSliceModeEnum &value) {

   _cleanTable.empty ();

   Boolean result (False);

   TimeSliceLuaInstance *ts (_table.lookup (TimeSliceHandle));

   if (ts) { value = ts->get_time_slice_mode (); result = True; }

   return result;
}


inline TimeSliceLua**
time_slice_check (lua_State *L, int index) {

   return (TimeSliceLua **)luaL_checkudata (L, index, TimeSliceLuaName);
}


static int
time_slice_new (lua_State *L) {

   TimeSliceLua *result (0);

   RuntimeContext *context (lua_get_runtime_context (L));

   TimeSliceLua **ptr =
      (TimeSliceLua **)lua_newuserdata (L, sizeof (TimeSliceLua *));

   if (ptr) {

      *ptr = new TimeSliceLua (context, L);

      luaL_getmetatable (L, TimeSliceLuaName);
      lua_setmetatable (L, -2);

      result = *ptr;
   }

   return result ? 1 : 0;
}


static int
time_slice_is_a (lua_State *L) {

   if (lua_is_object (L, 1, TimeSliceLuaName)) { lua_pushvalue (L, 1); }
   else { lua_pushnil (L); }

   return 1;
}


static const luaL_Reg arrayFunc [] = {
   {"new", time_slice_new},
   {"is_a", time_slice_is_a},
   {NULL, NULL},
};


static int
time_slice_create (lua_State *L) {

   int result (0);

   TimeSliceLua **ts = time_slice_check (L, 1);
   luaL_checktype (L, 2, LUA_TFUNCTION);
   luaL_checktype (L, 3, LUA_TTABLE);
   String name;
   Float64 interval (0.0);
   TimeSliceTypeEnum type (TimeSliceTypeRuntime);
   TimeSliceModeEnum mode (TimeSliceModeRepeating);

   if (!lua_isnoneornil (L, 4)) { name = luaL_checkstring (L, 4); }
   if (!lua_isnoneornil (L, 5)) { interval = luaL_checknumber (L, 5); }
   if (!lua_isnoneornil (L, 6)) { mode = to_time_slice_mode (L, 6); }
   if (!lua_isnoneornil (L, 7)) { type = to_time_slice_type (L, 7); }

   if (ts && *ts) {

      lua_pushlightuserdata (L, (void *)&LuaTimeSliceKey);
      lua_rawget (L, LUA_REGISTRYINDEX);
      const int TimeSliceTable (lua_gettop (L));

      lua_createtable (L, 2, 0);
      const int Table = lua_gettop (L);

      lua_pushvalue (L, 2);
      lua_rawseti (L, Table, 1);

      lua_pushvalue (L, 3);
      lua_rawseti (L, Table, 2);

      int index = luaL_ref (L, TimeSliceTable);
      lua_pop (L, 1); // pop ts table

      lua_create_handle (L, (*ts)->create_time_slice (index, type, mode, interval, name));
      result = 1;
   }

   return result;
}


static int
time_slice_destroy (lua_State *L) {

   int result (0);

   TimeSliceLua **ts = time_slice_check (L, 1);
   Handle *ptr = lua_check_handle (L, 2);

   if (ts && *ts && ptr) {

      lua_pushboolean (L, (*ts)->destroy_time_slice (*ptr) ? 1 : 0);
      result = 1;
   }

   return result;
}

static int
time_slice_start (lua_State *L) {

   int result (0);

   TimeSliceLua **ts = time_slice_check (L, 1);
   Handle *ptr = lua_check_handle (L, 2);

   if (ts && *ts && ptr) {

      lua_pushboolean (L, (*ts)->start_time_slice (*ptr) ? 1 : 0);
      result = 1;
   }

   return result;
}


static int
time_slice_stop (lua_State *L) {

   int result (0);

   TimeSliceLua **ts = time_slice_check (L, 1);
   Handle *ptr = lua_check_handle (L, 2);

   if (ts && *ts && ptr) {

      lua_pushboolean (L, (*ts)->stop_time_slice (*ptr) ? 1 : 0);
      result = 1;
   }

   return result;
}


static int
time_slice_set_interval (lua_State *L) {

   int result (0);

   TimeSliceLua **ts = time_slice_check (L, 1);
   Handle *ptr = lua_check_handle (L, 2);
   const Float64 Interval = luaL_checknumber (L, 3);

   if (ts && *ts && ptr) {

      lua_pushboolean (L, (*ts)->set_time_slice_interval (*ptr, Interval) ? 1 : 0);
      result = 1;
   }

   return result;
}


static int
time_slice_get_interval (lua_State *L) {

   int result (0);

   TimeSliceLua **ts = time_slice_check (L, 1);
   Handle *ptr = lua_check_handle (L, 2);

   if (ts && *ts && ptr) {

      lua_pushnumber (L, (*ts)->get_time_slice_interval (*ptr));
      result = 1;
   }

   return result;
}

static int
time_slice_get_type (lua_State *L) {

   int result (0);

   TimeSliceLua **ts = time_slice_check (L, 1);
   Handle *ptr = lua_check_handle (L, 2);

   if (ts && *ts && ptr) {

      TimeSliceTypeEnum value (TimeSliceTypeRuntime);
      (*ts)->get_time_slice_type (*ptr, value);
      lua_pushinteger (L, from_time_slice_type (value));
      result = 1;
   }

   return result;
}


static int
time_slice_set_mode (lua_State *L) {

   int result (0);

   TimeSliceLua **ts = time_slice_check (L, 1);
   Handle *ptr = lua_check_handle (L, 2);

   if (ts && *ts && ptr) {

      TimeSliceModeEnum value (to_time_slice_mode (L, 3));
      lua_pushboolean (L, (*ts)->set_time_slice_mode (*ptr, value) ? 1 : 0);
      result = 1;
   }

   return result;
}


static int
time_slice_get_mode (lua_State *L) {

   int result (0);

   TimeSliceLua **ts = time_slice_check (L, 1);
   Handle *ptr = lua_check_handle (L, 2);

   if (ts && *ts && ptr) {

      TimeSliceModeEnum value (TimeSliceModeRepeating);
      (*ts)->get_time_slice_mode (*ptr, value);
      lua_pushinteger (L, from_time_slice_mode (value));
      result = 1;
   }

   return result;
}


static int
time_slice_delete (lua_State *L) {

   TimeSliceLua **ts = time_slice_check (L, 1);
   if (ts && *ts) { delete (*ts); *ts = 0; }

   return 0;
}


static const luaL_Reg arrayMembers [] = {
   {"create", time_slice_create},
   {"start", time_slice_start},
   {"stop", time_slice_stop},
   {"set_interval", time_slice_set_interval},
   {"get_interval", time_slice_get_interval},
   {"get_type", time_slice_get_type},
   {"set_mode", time_slice_set_mode},
   {"get_mode", time_slice_get_mode},
   {"destroy", time_slice_destroy},
   {"__gc", time_slice_delete},
   {NULL, NULL},
};

};


//! \cond
void
dmz::open_lua_kernel_time_slice_lib (lua_State *L) {

   LUA_START_VALIDATE (L);

   lua_pushlightuserdata (L, (void *)&LuaTimeSliceKey);
   lua_newtable (L);
   lua_rawset (L, LUA_REGISTRYINDEX);

   luaL_newmetatable (L, TimeSliceLuaName);
   luaL_register (L, NULL, arrayMembers);
   lua_pushvalue (L, -1);
   lua_setfield (L, -2, "__index");
   lua_create_dmz_namespace (L, "time_slice");
   luaL_register (L, NULL, arrayFunc);

   LuaTimeSliceRuntime = lua_create_enum (L, "dmzTimeSliceTypeRuntime");
   LuaTimeSliceSystem = lua_create_enum (L, "dmzTimeSliceTypeSystemTime");
   LuaTimeSliceSingle = lua_create_enum (L, "dmzTimeSliceModeSingle");
   LuaTimeSliceRepeating = lua_create_enum (L, "dmzTimeSliceModeRepeating");

   lua_pushinteger (L, LuaTimeSliceSystem);
   lua_setfield (L, -2, "System");
   lua_pushinteger (L, LuaTimeSliceRuntime);
   lua_setfield (L, -2, "Runtime");
   lua_pushinteger (L, LuaTimeSliceRepeating);
   lua_setfield (L, -2, "Repeating");
   lua_pushinteger (L, LuaTimeSliceSingle);
   lua_setfield (L, -2, "Single");

   lua_make_readonly (L, -1); // make time_slice read only.
   lua_pop (L, 2); // pops meta table and dmz.time_slice table.

   LUA_END_VALIDATE (L, 0);
}
//! \endcond

