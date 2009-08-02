#ifndef DMZ_LUA_KERNEL_PRIVATE_DOT_H
#define DMZ_LUA_KERNEL_PRIVATE_DOT_H

struct lua_State;

namespace dmz {

   class Config;

//! \cond
   void open_lua_kernel_config_lib (lua_State *L, Config &global);
   void open_lua_kernel_data_lib (lua_State *L);
   void open_lua_kernel_definitions_lib (lua_State *L);
   void open_lua_kernel_event_type_lib (lua_State *L);
   void open_lua_kernel_handle_lib (lua_State *L);
   void open_lua_kernel_log_lib (lua_State *L);
   void open_lua_kernel_mask_lib (lua_State *L);
   void open_lua_kernel_math_lib (lua_State *L);
   void open_lua_kernel_matrix_lib (lua_State *L);
   void open_lua_kernel_message_observer_lib (lua_State *L);
   void open_lua_kernel_message_lib (lua_State *L);
   void open_lua_kernel_object_type_lib (lua_State *L);
   void open_lua_kernel_resources_lib (lua_State *L);
   void open_lua_kernel_runtime_lib (lua_State *L);
   void open_lua_kernel_runtime_exit_lib (lua_State *L);
   void open_lua_kernel_sphere_lib (lua_State *L);
   void open_lua_kernel_time_lib (lua_State *L);
   void open_lua_kernel_time_slice_lib (lua_State *L);
   void open_lua_kernel_undo_lib (lua_State *L);
   void open_lua_kernel_uuid_lib (lua_State *L);
   void open_lua_kernel_vector_lib (lua_State *L);
//! \endcond
};

#endif //  DMZ_LUA_KERNEL_PRIVATE_DOT_H
