#ifndef DMZ_LUA_KERNEL_RUNTIME_DOT_H
#define DMZ_LUA_KERNEL_RUNTIME_DOT_H

#include <dmzLuaKernelExport.h>
#include <dmzTypesBase.h>

struct lua_State;

namespace dmz {

   DMZ_LUA_KERNEL_LINK_SYMBOL int lua_find_empty_index (lua_State *L);
   DMZ_LUA_KERNEL_LINK_SYMBOL void lua_push_instance_table (lua_State *L);
};

#endif // DMZ_LUA_KERNEL_RUNTIME_DOT_H
