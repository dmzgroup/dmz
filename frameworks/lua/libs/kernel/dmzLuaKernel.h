#ifndef DMZ_LUA_KERNEL_DOT_H
#define DMZ_LUA_KERNEL_DOT_H

#include <dmzLuaKernelExport.h>
#include <dmzTypesBase.h>

#include <luacpp.h>

namespace dmz {

   class Config;
   class Data;
   class EventType;
   class HandleContainer;
   class Log;
   class Mask;
   class Matrix;
   class Message;
   class ObjectType;
   class RuntimeContext;
   class Stream;
   class String;
   class Sphere;
   class UUID;
   class Vector;

   DMZ_LUA_KERNEL_LINK_SYMBOL void lua_dump_stack (lua_State *L, Stream &s);

   DMZ_LUA_KERNEL_LINK_SYMBOL RuntimeContext *lua_get_runtime_context (lua_State *L);

   DMZ_LUA_KERNEL_LINK_SYMBOL Boolean lua_create_dmz_namespace (
      lua_State *L,
      const String &Name);

   DMZ_LUA_KERNEL_LINK_SYMBOL void *
   lua_is_object (lua_State *L, int index, const char *name);

   DMZ_LUA_KERNEL_LINK_SYMBOL Config *lua_to_config (lua_State *L, int narg);
   DMZ_LUA_KERNEL_LINK_SYMBOL Config *lua_check_config (lua_State *L, int narg);

   DMZ_LUA_KERNEL_LINK_SYMBOL Config *lua_create_config (
      lua_State *L,
      const String &Name);

   DMZ_LUA_KERNEL_LINK_SYMBOL Data *lua_create_data (
      lua_State *L,
      const Data *Value = 0);

   DMZ_LUA_KERNEL_LINK_SYMBOL Data *lua_to_data (lua_State *L, int narg);
   DMZ_LUA_KERNEL_LINK_SYMBOL Data *lua_check_data (lua_State *L, int narg);

   DMZ_LUA_KERNEL_LINK_SYMBOL EventType *lua_create_event_type (
      lua_State *L,
      const EventType *Value = 0);

   DMZ_LUA_KERNEL_LINK_SYMBOL EventType *lua_to_event_type (lua_State *L, int narg);

   DMZ_LUA_KERNEL_LINK_SYMBOL EventType *lua_check_event_type (
      lua_State *L,
      int narg);

   DMZ_LUA_KERNEL_LINK_SYMBOL Handle *lua_to_handle (lua_State *L, int narg);
   DMZ_LUA_KERNEL_LINK_SYMBOL Handle *lua_check_handle (lua_State *L, int narg);

   DMZ_LUA_KERNEL_LINK_SYMBOL Handle *
   lua_create_handle (lua_State *L, const Handle Value);

   DMZ_LUA_KERNEL_LINK_SYMBOL void
   lua_handle_container_to_table (lua_State *L, const HandleContainer &Container);

   DMZ_LUA_KERNEL_LINK_SYMBOL Log *lua_to_log (lua_State *L, int narg);
   DMZ_LUA_KERNEL_LINK_SYMBOL Log *lua_check_log (lua_State *L, int narg);

   DMZ_LUA_KERNEL_LINK_SYMBOL Mask *lua_create_mask (
      lua_State *L,
      const Mask *Value = 0);

   DMZ_LUA_KERNEL_LINK_SYMBOL Mask *lua_to_mask (lua_State *L, int narg);
   DMZ_LUA_KERNEL_LINK_SYMBOL Mask *lua_check_mask (lua_State *L, int narg);

   DMZ_LUA_KERNEL_LINK_SYMBOL Matrix *lua_create_matrix (
      lua_State *L,
      const Matrix *Value = 0);

   DMZ_LUA_KERNEL_LINK_SYMBOL Matrix *lua_to_matrix (lua_State *L, int narg);
   DMZ_LUA_KERNEL_LINK_SYMBOL Matrix *lua_check_matrix (lua_State *L, int narg);

   DMZ_LUA_KERNEL_LINK_SYMBOL Message *lua_create_message (
      lua_State *L,
      const Message *Value = 0);

   DMZ_LUA_KERNEL_LINK_SYMBOL Message *lua_to_message (lua_State *L, int narg);

   DMZ_LUA_KERNEL_LINK_SYMBOL Message *lua_check_message (
      lua_State *L,
      int narg);

   DMZ_LUA_KERNEL_LINK_SYMBOL ObjectType *lua_create_object_type (
      lua_State *L,
      const ObjectType *Value = 0);

   DMZ_LUA_KERNEL_LINK_SYMBOL ObjectType *lua_to_object_type (lua_State *L, int narg);

   DMZ_LUA_KERNEL_LINK_SYMBOL ObjectType *
   lua_check_object_type (lua_State *L, int narg);

   DMZ_LUA_KERNEL_LINK_SYMBOL Sphere *lua_create_sphere (
      lua_State *L,
      const Sphere &Value);

   DMZ_LUA_KERNEL_LINK_SYMBOL Sphere *lua_to_sphere (lua_State *L, int narg);
   DMZ_LUA_KERNEL_LINK_SYMBOL Sphere *lua_check_sphere (lua_State *L, int narg);

   DMZ_LUA_KERNEL_LINK_SYMBOL UUID *lua_create_uuid (
      lua_State *L,
      const UUID &Value);

   DMZ_LUA_KERNEL_LINK_SYMBOL UUID *lua_to_uuid (lua_State *L, int narg);
   DMZ_LUA_KERNEL_LINK_SYMBOL UUID *lua_check_uuid (lua_State *L, int narg);

   DMZ_LUA_KERNEL_LINK_SYMBOL Vector *
   lua_create_vector (lua_State *L, const Vector *Value = 0);

   DMZ_LUA_KERNEL_LINK_SYMBOL Vector *lua_to_vector (lua_State *L, int narg);
   DMZ_LUA_KERNEL_LINK_SYMBOL Vector *lua_check_vector (lua_State *L, int narg);

   DMZ_LUA_KERNEL_LINK_SYMBOL int lua_create_enum (lua_State *L, const String &Name);

   DMZ_LUA_KERNEL_LINK_SYMBOL String
   lua_lookup_enum_name (lua_State *L, const int Value);

   DMZ_LUA_KERNEL_LINK_SYMBOL void
   lua_enum_error (lua_State *L, int index, const int Value);

   DMZ_LUA_KERNEL_LINK_SYMBOL void lua_make_readonly (lua_State *L, int index);

   DMZ_LUA_KERNEL_LINK_SYMBOL void
   lua_set_gc (lua_State *L, int index, lua_CFunction fn);

   DMZ_LUA_KERNEL_LINK_SYMBOL void
   lua_set_weak_table (lua_State *L, int index, const String &Mode);
};

#endif // DMZ_LUA_KERNEL_DOT_H
