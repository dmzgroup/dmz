#ifndef DMZ_LUA_KERNEL_VALIDATE_DOT_H
#define DMZ_LUA_KERNEL_VALIDATE_DOT_H

#include <dmzLuaKernel.h>
#include <dmzRuntimeLog.h>
#include <luacpp.h>

#define DMZ_LUA_VALIDATE_STACK

#ifdef DMZ_LUA_VALIDATE_STACK
#   define LUA_START_VALIDATE(L) const int LuaValidateStackTop__ = lua_gettop (L)
#   define LUA_END_VALIDATE(L, ix) \
if ((lua_gettop (L) - LuaValidateStackTop__) != ix) { \
   dmz::Log log (__FILE__, dmz::lua_get_runtime_context (L)); \
   log.error << "Stack error in: " << __FUNCTION__ << " on line: " \
      << dmz::Int32 (__LINE__) << dmz::endl; \
   log.error << "Was: " << dmz::Int32 (LuaValidateStackTop__) \
      << " is: " << dmz::Int32 (lua_gettop (L)) \
      << " difference should be: " << dmz::Int32 (ix) << dmz::endl; \
   lua_dump_stack (L, log.error); \
}
#else
#   define LUA_START_VALIDATE(L) // Do nothing
#   define LUA_END_VALIDATE(L, ix) // Do nothing
#endif

#endif // DMZ_LUA_KERNEL_VALIDATE_DOT_H
