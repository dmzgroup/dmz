#include <dmzLuaKernel.h>
#include <dmzLuaKernelErrorHandler.h>
#include <dmzLuaKernelInit.h>
#include "dmzLuaKernelPrivate.h"
#include "dmzLuaKernelValidate.h"
#include <dmzSystemStream.h>
#include <dmzTypesHashTableStringTemplate.h>
#include <dmzTypesHashTableUInt32Template.h>
#include <dmzTypesString.h>

/*!

\class dmz::LuaErrorHandler
\ingroup Lua
\brief Provides an class interface for Lua error handling.
\details Used by the dmz::LuaModuleBase to handle errors when encountered in executing
scripts.

\fn int dmz::LuaErrorHandler::handle_lua_error (lua_State *L)
\brief Pure virtual function called when a Lua execution error is encountered.

*/

using namespace dmz;

namespace {

static const char DMZName[] = "dmz";
static const char RuntimeContextName[] = "dmz.runtime.context";
static const char ErrorHandlerName[] = "dmz.error.handler";

class enumMap {

   public:
      enumMap ();
      ~enumMap ();

      UInt32 create_enum (const String &Name);
      String lookup_enum_name (const UInt32 Value);

   protected:
      UInt32 _count;
      HashTableStringTemplate<UInt32> _handleTable;
      HashTableUInt32Template<String> _nameTable;
};


enumMap::enumMap () : _count (1) {;}


enumMap::~enumMap () { _handleTable.empty (); _nameTable.empty (); }


UInt32
enumMap::create_enum (const String &Name) {

   UInt32 result (0);

   UInt32 *ptr (_handleTable.lookup (Name));

   if (ptr) { result = *ptr; }
   else {

      ptr = new UInt32 (_count);
      if (ptr) {

         if (_handleTable.store (Name, ptr)) {

            result = *ptr;
            _count++;
            String *str (new String (Name));

            if (!_nameTable.store (*ptr, str)) {

               delete str; str = 0;
            }
         }
         else { delete ptr; ptr = 0; }
      }
   }

   return result;
}


String
enumMap::lookup_enum_name (const UInt32 Value) {

   String result;

   String *ptr (_nameTable.lookup (Value));

   if (ptr) { result = *ptr; }

   return result;
}


int
enum_delete (lua_State *L) {

   enumMap **ptr = (enumMap **)lua_touserdata (L, 1);

   if (ptr && *ptr) { delete (*ptr); *ptr = 0; }

   return 0;
}


int
table_readonly (lua_State *L) {

   lua_pushliteral (L, "Attempting to write to read only table.");
   lua_error (L);

   return 0;
}


static char EnumMapLightUserData = 'r';

void
enum_setup (lua_State *L) {

   LUA_START_VALIDATE (L);

   lua_pushlightuserdata (L, (void *)&EnumMapLightUserData);
   enumMap **ptr = (enumMap **)lua_newuserdata (L, sizeof (enumMap *));

   if (ptr) {

      *ptr = 0;
      lua_set_gc (L, -1, enum_delete);

      *ptr = new enumMap;

      lua_rawset (L, LUA_REGISTRYINDEX);
   }
   else { lua_pop (L, 1); } // pop light user data

   LUA_END_VALIDATE (L, 0);
}

}

//! \addtogroup Lua
//! @{

//! Opens the DMZ Kernel Lua Libraries in the provided lua_State.
void
dmz::open_lua_kernel_libs (Config &global, lua_State *L, RuntimeContext *context) {

   LUA_START_VALIDATE (L);

   enum_setup (L);

   lua_pushstring (L, RuntimeContextName);

   RuntimeContext **ptr =
      (RuntimeContext **)lua_newuserdata (L, sizeof (RuntimeContext *));

   if (ptr) { *ptr = context; }

   lua_rawset (L, LUA_REGISTRYINDEX);

   open_lua_kernel_config_lib (L, global);
   open_lua_kernel_data_lib (L);
   open_lua_kernel_definitions_lib (L);
   open_lua_kernel_event_type_lib (L);
   open_lua_kernel_handle_lib (L);
   open_lua_kernel_log_lib (L);
   open_lua_kernel_vector_lib (L);
   open_lua_kernel_matrix_lib (L);
   open_lua_kernel_mask_lib (L);
   open_lua_kernel_math_lib (L);
   open_lua_kernel_message_observer_lib (L);
   open_lua_kernel_message_lib (L);
   open_lua_kernel_object_type_lib (L);
   open_lua_kernel_runtime_lib (L);
   open_lua_kernel_resources_lib (L);
   open_lua_kernel_sphere_lib (L);
   open_lua_kernel_runtime_exit_lib (L);
   open_lua_kernel_time_slice_lib (L);
   open_lua_kernel_time_lib (L);
   open_lua_kernel_uuid_lib (L);
   open_lua_kernel_undo_lib (L);

   LUA_END_VALIDATE (L, 0);
}


//! Dumps the current Lua call stack stored in the lua_State to the provided Stream.
void
dmz::lua_dump_stack (lua_State *L, Stream &s) {

   int top = lua_gettop (L);

   for (int ix = top; ix > 0; ix--) {

      s << Int32 (ix) << " " << Int32 (ix - (top + 1)) << " " << luaL_typename (L, ix)
         << endl;
   }
}


//! Gets the current RuntimeContext stored with the lua_State
dmz::RuntimeContext *
dmz::lua_get_runtime_context (lua_State *L) {

   LUA_START_VALIDATE (L);

   RuntimeContext *result (0);

   lua_pushstring (L, RuntimeContextName);
   lua_rawget (L, LUA_REGISTRYINDEX);
   RuntimeContext **ptr = (RuntimeContext **)lua_touserdata (L, -1);

   if (ptr) { result = *ptr; }

   lua_pop (L, 1);

   LUA_END_VALIDATE (L, 0);

   return result;
}


//! Creates a new namespace within the Lua dmz namespace.
dmz::Boolean
dmz::lua_create_dmz_namespace (lua_State *L, const String &Name) {

   LUA_START_VALIDATE (L);

   Boolean result (False);

   lua_getglobal (L, DMZName);

   if (lua_isnil (L, -1)) {

      lua_pop (L, 1);
      lua_newtable (L);
      lua_setglobal (L, DMZName);
      lua_getglobal (L, DMZName);
   }

   if (lua_istable (L, -1)) {

      lua_getfield (L, -1, Name.get_buffer ());

      if (!lua_istable (L, -1)) {

         lua_pop (L, 1);
         lua_newtable (L);
         lua_pushvalue (L, -1);
         lua_setfield (L, -3, Name.get_buffer ());
      }

      lua_remove (L, -2);

      if (lua_getmetatable (L, -1)) {

         lua_pushliteral (L, "__newindex");
         lua_pushnil (L);
         lua_rawset (L, -3);
         lua_pop (L, 1); // pop meta table;
      }

      result = True;
   }

   LUA_END_VALIDATE (L, 1);

   return result;
}


//! Determines if the specified Lua user data object on the stack is a explicit type.
void *
dmz::lua_is_object (lua_State *L, int index, const char *name) {

   LUA_START_VALIDATE (L);

   void *result = lua_touserdata (L, index);

   if (result) {

      if (lua_getmetatable(L, index)) {

         lua_getfield(L, LUA_REGISTRYINDEX, name);

         if (!lua_rawequal(L, -1, -2)) { result = 0; }

         lua_pop(L, 2);
      }
   }

   LUA_END_VALIDATE (L, 0);

   return result;
}


//! Registers an error handler to invoke if the executing script encounters an error.
dmz::Boolean
dmz::lua_register_error_handler (lua_State *L, LuaErrorHandler *handler) {

   Boolean result (False);

   if (!lua_get_error_handler (L)) {

      lua_pushstring (L, ErrorHandlerName);

      LuaErrorHandler **ptr =
         (LuaErrorHandler **)lua_newuserdata (L, sizeof (RuntimeContext *));

      if (ptr) { *ptr = handler; result = True; }

      lua_rawset (L, LUA_REGISTRYINDEX);
   }

   return result;
}


//! Releases the registered error handler.
dmz::Boolean
dmz::lua_release_error_handler (lua_State *L, LuaErrorHandler *handler) {

   Boolean result (False);

   if (handler && (handler == lua_get_error_handler (L))) {

      lua_pushstring (L, ErrorHandlerName);
      lua_pushnil (L);
      lua_rawset (L, LUA_REGISTRYINDEX);

      result = True;
   }

   return result;
}


//! Gets the currently registered error handler.
dmz::LuaErrorHandler *
dmz::lua_get_error_handler (lua_State *L) {

   LUA_START_VALIDATE (L);

   LuaErrorHandler *result (0);

   lua_pushstring (L, ErrorHandlerName);
   lua_rawget (L, LUA_REGISTRYINDEX);
   LuaErrorHandler **ptr = (LuaErrorHandler **)lua_touserdata (L, -1);

   if (ptr) { result = *ptr; }

   lua_pop (L, 1);

   LUA_END_VALIDATE (L, 0);

   return result;
}


//! Default error handler.
int
dmz::lua_error_handler (lua_State *L) {

  int result (0);

  LUA_START_VALIDATE (L);

  dmz::LuaErrorHandler *handler = dmz::lua_get_error_handler (L);

  if (handler) { result = handler->handle_lua_error (L); }
  else { result = 1; }

  LUA_END_VALIDATE (L, result);

  return result;
}


//! Creates a new enum with the specified name.
int
dmz::lua_create_enum (lua_State *L, const String &Name) {

   LUA_START_VALIDATE (L);

   int result (0);

   lua_pushlightuserdata (L, (void *)&EnumMapLightUserData);
   lua_rawget (L, LUA_REGISTRYINDEX);

   enumMap **ptr = (enumMap **)lua_touserdata (L, -1);

   if (ptr && *ptr) {

      result = (int) (*ptr)->create_enum (Name);
   }

   lua_pop (L, 1); // pop enumMap

   LUA_END_VALIDATE (L, 0);

   return result;
}


//! Gets the enums name based on the integer value of the enum
dmz::String
dmz::lua_lookup_enum_name (lua_State *L, const int Value) {

   LUA_START_VALIDATE (L);

   String result;

   lua_pushlightuserdata (L, (void *)&EnumMapLightUserData);
   lua_rawget (L, LUA_REGISTRYINDEX);

   enumMap **ptr = (enumMap **)lua_touserdata (L, -1);

   if (ptr && *ptr) {

      result = (*ptr)->lookup_enum_name ((UInt32)Value);
   }

   lua_pop (L, 1); // pop enumMap

   LUA_END_VALIDATE (L, 0);

   return result;

}


//! Error handler for unknown enum values.
void
dmz::lua_enum_error (lua_State *L, int index, const int Value) {

   if (index < 0) { index = lua_gettop (L) + index + 1; }

   String error ("Illegal Lua enum: ");
   const String Name (lua_lookup_enum_name (L, Value));
   error << (Name ? Name : "<Unknown>") << " [" << (Int32)Value << "]";
   if (index > 0) { error << " parameter #" << index; }
   lua_pushstring (L, error.get_buffer ());
   lua_error (L);
}


//! Creates a metatable for the specified object on the stack.
static inline void
local_create_mt (lua_State *L, int index) {

   LUA_START_VALIDATE (L);

   if (index < 0) { index = lua_gettop (L) + index + 1; }

   if (!lua_getmetatable (L, index)) {

      lua_newtable (L);
      lua_pushvalue (L, -1);
      lua_setmetatable (L, index);
   }

   LUA_END_VALIDATE (L, 1);
}


//! Makes the specified table on the stack read only.
void
dmz::lua_make_readonly (lua_State *L, int index) {

   LUA_START_VALIDATE (L);

   if (index < 0) { index = lua_gettop (L) + index + 1; }

   local_create_mt (L, index);

   lua_pushliteral (L, "__newindex");
   lua_pushcfunction (L, table_readonly);
   lua_rawset (L, -3);
   lua_pop (L, 1); // pop meta table;

   LUA_END_VALIDATE (L, 0);
}


//! Adds a garbage collection function to clean up when a user data object is collected.
void
dmz::lua_set_gc (lua_State *L, int index, lua_CFunction fn) {

   LUA_START_VALIDATE (L);

   if (index < 0) { index = lua_gettop (L) + index + 1; }

   local_create_mt (L, index);

   lua_pushliteral (L, "__gc");
   lua_pushcfunction (L, fn);
   lua_rawset (L, -3);
   lua_pop (L, 1); // pop meta table;

   LUA_END_VALIDATE (L, 0);
}


//! Turns a table into a weak table.
void
dmz::lua_set_weak_table (lua_State *L, int index, const String &Mode) {

   LUA_START_VALIDATE (L);

   if (index < 0) { index = lua_gettop (L) + index + 1; }

   local_create_mt (L, index);

   lua_pushliteral (L, "__mode");
   lua_pushstring (L, Mode.get_buffer ());
   lua_rawset (L, -3);
   lua_pop (L, 1); // pop meta table;

   LUA_END_VALIDATE (L, 0);
}

//! @}
