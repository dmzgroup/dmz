#ifndef DMZ_LUA_KERNEL_INIT_DOT_H
#define DMZ_LUA_KERNEL_INIT_DOT_H

#include <dmzLuaKernelExport.h>

struct lua_State;

namespace dmz {

   class Config;
   class RuntimeContext;

   DMZ_LUA_KERNEL_LINK_SYMBOL void open_lua_kernel_libs (
      Config &global,
      lua_State *L,
      RuntimeContext *context);
};

#endif // DMZ_LUA_KERNEL_INIT_DOT_H
