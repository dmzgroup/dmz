#include <dmzRenderModuleOverlay.h>
#include "dmzLuaExtOverlay.h"
#include <dmzLuaKernel.h>
#include <dmzLuaKernelValidate.h> 
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>

#include <luacpp.h>

/*!

\class dmz::LuaExtOverlay
\ingroup Lua
\brief Provides a Lua API for the Render Overlay Module.
\details
\htmlonly See the <a href="dmzlua.html#dmz.overlay">Lua Overlay API</a>.
\endhtmlonly

*/

using namespace dmz;

namespace {

static char OverlayKey = 'o';

static inline RenderModuleOverlay *
get_overlay (lua_State *L) {

  RenderModuleOverlay *result (0);

  lua_pushlightuserdata (L, (void *)&OverlayKey);
  lua_rawget (L, LUA_REGISTRYINDEX);
  RenderModuleOverlay **audio = (RenderModuleOverlay **)lua_touserdata (L, -1);
  if (audio) { result = *audio; }
  lua_pop (L, 1); // pop audio module

   return result;
}


static inline Handle
get_node (lua_State *L, RenderModuleOverlay *overlay, int index) {

   Handle result = 0;

   if (overlay) {

     Handle *ptr = lua_to_handle (L, index);

     if (ptr) { result = *ptr; }
     else {

        if (lua_isstring (L, index)) {

           const String Name = lua_tostring (L, 1);

           result = overlay->lookup_node_handle (Name);
        }
     }
  }

  return result;
}


static int
overlay_lookup_handle (lua_State *L) {

   LUA_START_VALIDATE (L);

   int result (0);

   RenderModuleOverlay *overlay (get_overlay (L));
   const String Name (luaL_checkstring (L, 1));

   if (overlay) {

      lua_create_handle (L, overlay->lookup_node_handle (Name));
      result = 1;
   }

   LUA_END_VALIDATE (L, result);

   return result;
}


static int
overlay_lookup_clone_sub_handle (lua_State *L) {

   LUA_START_VALIDATE (L);

   int result (0);

   RenderModuleOverlay *overlay (get_overlay (L));
   const Handle Node (get_node (L, overlay, 1));
   const String Name (luaL_checkstring (L, 2));

   if (Node && overlay) {

      lua_create_handle (L, overlay->lookup_node_clone_sub_handle (Node, Name));
      result = 1;
   }

   LUA_END_VALIDATE (L, result);

   return result;
}


static int
overlay_lookup_name (lua_State *L) {

   LUA_START_VALIDATE (L);

   int result (0);

   RenderModuleOverlay *overlay (get_overlay (L));
   const Handle Node (get_node (L, overlay, 1));

   if (Node && overlay) {

      const String Value = overlay->lookup_node_name (Node);
      lua_pushstring (L, Value.get_buffer ());
      result = 1;
   }

   LUA_END_VALIDATE (L, result);

   return result;
}


static int
overlay_clone_template (lua_State *L) {

   LUA_START_VALIDATE (L);

   int result (0);

   RenderModuleOverlay *overlay (get_overlay (L));
   const String Name (luaL_checkstring (L, 1));

   if (Name && overlay) {

      lua_create_handle (L, overlay->clone_template (Name));
      result = 1;
   }

   LUA_END_VALIDATE (L, result);

   return result;
}


static int
overlay_destroy_node (lua_State *L) {

   LUA_START_VALIDATE (L);

   int result (0);

   RenderModuleOverlay *overlay (get_overlay (L));
   const Handle Node (get_node (L, overlay, 1));

   if (Node && overlay) {

      const Boolean Value = overlay->destroy_node (Node);
      lua_pushboolean (L, Value ? 1 : 0);
      result = 1;
   }

   LUA_END_VALIDATE (L, result);

   return result;
}


static int
overlay_text (lua_State *L) {

   LUA_START_VALIDATE (L);

   int result (0);

   RenderModuleOverlay *overlay (get_overlay (L));
   const Handle Node (get_node (L, overlay, 1));

   if (lua_isnoneornil (L, 2)) {

      String value;

      if (overlay && overlay->lookup_text (Node, value)) {

         if (value) { lua_pushstring (L, value.get_buffer ()); result = 1; }
      }
   }
   else {

      const String Value = luaL_checkstring (L, 2);

      if (overlay && overlay->store_text (Node, Value)) {

         if (Value) { lua_pushstring (L, Value.get_buffer ()); result = 1; }
      }
   }

   LUA_END_VALIDATE (L, result);

   return result;
}


static int
overlay_add_child (lua_State *L) {

   LUA_START_VALIDATE (L);

   int result (0);

   RenderModuleOverlay *overlay (get_overlay (L));
   const Handle Parent (get_node (L, overlay, 1));
   const Handle Child (get_node (L, overlay, 2));

   if (Parent && Child && overlay) {

      lua_pushboolean (L, overlay->add_group_child (Parent, Child) ? 1 : 0);
      result = 1;
   }

   LUA_END_VALIDATE (L, result);

   return result;
}


static int
overlay_remove_child (lua_State *L) {

   LUA_START_VALIDATE (L);

   int result (0);

   RenderModuleOverlay *overlay (get_overlay (L));
   const Handle Parent (get_node (L, overlay, 1));
   const Handle Child (get_node (L, overlay, 2));

   if (Parent && Child && overlay) {

      lua_pushboolean (L, overlay->remove_group_child (Parent, Child) ? 1 : 0);
      result = 1;
   }

   LUA_END_VALIDATE (L, result);

   return result;
}


static int
overlay_switch_state (lua_State *L) {

   LUA_START_VALIDATE (L);

   int result (0);

   RenderModuleOverlay *overlay (get_overlay (L));
   const Handle Node = get_node (L, overlay, 1);
   const int Which = luaL_checkint (L, 2);
   Boolean set = False;
   if (!lua_isnoneornil (L, 3)) { set = True; } 

   if (overlay && Node && (Which >= 0)) {

      Boolean get = True;

      if (set) {

         const Boolean Value = lua_toboolean (L, 3) ? True : False;

         get = overlay->store_switch_state (Node, (const UInt32)Which, Value);
      }

      if (get) {

         lua_pushboolean (
            L,
            overlay->lookup_switch_state (Node, (const UInt32)Which) ? 1 : 0);

         result = 1;
      }
   }

   LUA_END_VALIDATE (L, result);

   return result;
}


static int
overlay_all_switch_state (lua_State *L) {

   LUA_START_VALIDATE (L);

   int result (0);

   RenderModuleOverlay *overlay (get_overlay (L));
   const Handle Node = get_node (L, overlay, 1);
   const Boolean Value = lua_toboolean (L, 2) ? True : False;

   if (overlay && Node) {

      lua_pushboolean (
         L,
         overlay->store_switch_state_all (Node, Value) ? 1 : 0);

      result = 1;
   }

   LUA_END_VALIDATE (L, result);

   return result;
}


static int
overlay_enable_switch_state_single (lua_State *L) {

   LUA_START_VALIDATE (L);

   int result (0);

   RenderModuleOverlay *overlay (get_overlay (L));
   const Handle Node (get_node (L, overlay, 1));
   const int Which (luaL_checkint (L, 2));

   if (Node && (Which >= 0) && overlay) {

      lua_pushboolean (
         L,
         overlay->enable_switch_state_single (Node, (const UInt32)Which) ? 1 : 0);

      result = 1;
   }

   LUA_END_VALIDATE (L, result);

   return result;
}


static int
overlay_position (lua_State *L) {

   LUA_START_VALIDATE (L);

   int result (0);

   RenderModuleOverlay *overlay (get_overlay (L));
   const Handle Node = get_node (L, overlay, 1);
   Boolean set = False;
   if (!lua_isnoneornil (L, 2)) { set = True; } 

   if (overlay && Node) {

      Boolean get = True;

      if (set) {

         const Float64 TheX = luaL_checknumber (L, 2);
         const Float64 TheY = luaL_checknumber (L, 3);

         get = overlay->store_transform_position (Node, TheX, TheY);
      }

      if (get) {

         Float64 theX (0.0), theY (0.0);

         if (overlay->lookup_transform_position (Node, theX, theY)) {

            lua_pushnumber (L, theX);
            lua_pushnumber (L, theY);

            result = 2;
         }
      }
   }

   LUA_END_VALIDATE (L, result);

   return result;
}


static int
overlay_rotation (lua_State *L) {

   LUA_START_VALIDATE (L);

   int result (0);

   RenderModuleOverlay *overlay (get_overlay (L));
   const Handle Node = get_node (L, overlay, 1);
   Boolean set = False;
   if (!lua_isnoneornil (L, 2)) { set = True; } 

   if (overlay && Node) {

      Boolean get = True;

      if (set) {

         const Float64 Rot = luaL_checknumber (L, 2);

         get = overlay->store_transform_rotation (Node, Rot);
      }

      if (get) {

         Float64 rot (0.0);

         if (overlay->lookup_transform_rotation (Node, rot)) {

            lua_pushnumber (L, rot);

            result = 1;
         }
      }
   }

   LUA_END_VALIDATE (L, result);

   return result;
}


static int
overlay_scale (lua_State *L) {

   LUA_START_VALIDATE (L);

   int result (0);

   RenderModuleOverlay *overlay (get_overlay (L));
   const Handle Node = get_node (L, overlay, 1);
   Boolean set = False;
   if (!lua_isnoneornil (L, 2)) { set = True; } 

   if (overlay && Node) {

      Boolean get = True;

      if (set) {

         const Float64 TheX = luaL_checknumber (L, 2);
         const Float64 TheY = lua_isnoneornil (L, 3) ? TheX : luaL_checknumber (L, 3);

         get = overlay->store_transform_scale (Node, TheX, TheY);
      }

      if (get) {

         Float64 theX (0.0), theY (0.0);

         if (overlay->lookup_transform_scale (Node, theX, theY)) {

            lua_pushnumber (L, theX);
            lua_pushnumber (L, theY);

            result = 2;
         }
      }
   }

   LUA_END_VALIDATE (L, result);

   return result;
}


static const luaL_Reg arrayFunc[] = {
   {"lookup_handle", overlay_lookup_handle},
   {"lookup_clone_sub_handle", overlay_lookup_clone_sub_handle},
   {"lookup_name", overlay_lookup_name},
   {"clone_template", overlay_clone_template},
   {"destroy_node", overlay_destroy_node},
   {"text", overlay_text},
   {"add_child", overlay_add_child},
   {"remove_child", overlay_remove_child},
   {"switch_state", overlay_switch_state},
   {"switch_state_all", overlay_all_switch_state},
   {"enable_switch_state_single", overlay_enable_switch_state_single},
   {"position", overlay_position},
   {"rotation", overlay_rotation},
   {"scale", overlay_scale},
   {NULL, NULL},
};

}


//! \cond
dmz::LuaExtOverlay::LuaExtOverlay (const PluginInfo &Info, Config &local) :
      Plugin (Info),
      LuaExt (Info),
      _log (Info),
      L (0),
      _overlay (0),
      _overlayPtr (0) {

   _init (local);
}


dmz::LuaExtOverlay::~LuaExtOverlay () {

}


// Plugin Interface
void
dmz::LuaExtOverlay::update_plugin_state (
      const PluginStateEnum State,
      const UInt32 Level) {

   if (State == PluginStateInit) {

   }
   else if (State == PluginStateStart) {

   }
   else if (State == PluginStateStop) {

   }
   else if (State == PluginStateShutdown) {

   }
}


void
dmz::LuaExtOverlay::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      if (!_overlay) {

         _overlay = RenderModuleOverlay::cast (PluginPtr);

         if (_overlay && _overlayPtr && !(*_overlayPtr)) { *_overlayPtr = _overlay; }
      }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_overlay && (_overlay == RenderModuleOverlay::cast (PluginPtr))) {

         _overlay = 0;

         if (_overlayPtr) { *_overlayPtr = 0; }
      }
   }
}


// LuaExt Interface
void
dmz::LuaExtOverlay::store_lua_module (LuaModule &module) {

}


void
dmz::LuaExtOverlay::open_lua_extension (lua_State *L) {

   LUA_START_VALIDATE (L);

   lua_pushlightuserdata (L, (void *)&OverlayKey);
   _overlayPtr =
      (RenderModuleOverlay **)lua_newuserdata (L, sizeof (RenderModuleOverlay *));

   if (_overlayPtr) {

      *_overlayPtr = _overlay;
      lua_rawset (L, LUA_REGISTRYINDEX);
   }
   else { lua_pop (L, 1); } // pop light data

   lua_create_dmz_namespace (L, "overlay");
   luaL_register (L, NULL, arrayFunc);

   lua_make_readonly (L, -1);
   lua_pop (L, 1); // pop dmz.audio table

   LUA_END_VALIDATE (L, 0);
}


void
dmz::LuaExtOverlay::close_lua_extension (lua_State *L) {

   if (_overlayPtr) { *_overlayPtr = 0; _overlayPtr = 0; }
   lua_pushlightuserdata (L, (void *)&OverlayKey);
   lua_pushnil (L);
   lua_rawset (L, LUA_REGISTRYINDEX);
}

void
dmz::LuaExtOverlay::remove_lua_module (LuaModule &module) {

}


void
dmz::LuaExtOverlay::_init (Config &local) {

}
//! \endcond


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzLuaExtOverlay (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::LuaExtOverlay (Info, local);
}

};
