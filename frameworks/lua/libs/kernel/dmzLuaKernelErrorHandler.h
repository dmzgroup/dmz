#ifndef DMZ_LUA_KERNEL_ERROR_HANDLER_DOT_H
#define DMZ_LUA_KERNEL_ERROR_HANDLER_DOT_H

#include <dmzLuaKernelExport.h>
#include <dmzTypesString.h>

struct lua_State;

namespace dmz {

   class LuaErrorHandler {

      public:
         virtual int handle_lua_error (lua_State *L) = 0;

      protected:
         LuaErrorHandler () {;}
         ~LuaErrorHandler () {;}
   };

   DMZ_LUA_KERNEL_LINK_SYMBOL Boolean
   lua_register_error_handler (lua_State *L, LuaErrorHandler *handler);

   DMZ_LUA_KERNEL_LINK_SYMBOL Boolean
   lua_release_error_handler (lua_State *L, LuaErrorHandler *handler);

   DMZ_LUA_KERNEL_LINK_SYMBOL LuaErrorHandler *
   lua_get_error_handler (lua_State *L);

   DMZ_LUA_KERNEL_LINK_SYMBOL int
   lua_error_handler (lua_State *L);
};

#endif // DMZ_LUA_KERNEL_ERROR_HANDLER_DOT_H
