#include "dmzLuaExtPick.h"
#include <dmzLuaKernel.h>
#include <dmzLuaKernelValidate.h>
#include <dmzRenderModulePick.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzTypesVector.h>

#include <luacpp.h>

#include <qdb.h>
static dmz::qdb out;

/*!
         
\class dmz::LuaExtPick
\ingroup Lua
\brief Provides a Lua API for the Render Pick Module.
\details
\htmlonly See the <a href="dmzlua.html#dmz.pick">Lua Pick API</a>.
\endhtmlonly
         
*/

using namespace dmz;

namespace {

static char PickKey = 'p';


static inline dmz::RenderModulePick *
get_pick (lua_State *L) {

  RenderModulePick *result (0);
  lua_pushlightuserdata (L, (void *)&PickKey);
  lua_rawget (L, LUA_REGISTRYINDEX);
  RenderModulePick **pick = (RenderModulePick **)lua_touserdata (L, -1);
  if (pick) { result = *pick; }
  lua_pop (L, 1); // pop pick module

  return result;
}


static int
pick_screen_to_world (lua_State *L) {

   LUA_START_VALIDATE (L);

   int result (0);

   RenderModulePick *pick (get_pick (L));
   Handle *sourceHandle (lua_check_handle (L, 1));
   const int ScreenPosX (luaL_checkinteger (L, 2));
   const int ScreenPosY (luaL_checkinteger (L, 3));

   if (pick && sourceHandle) {

      Vector worldPosition, normal;
      Handle objectHandle;

      if (pick->screen_to_world (
               *sourceHandle,
               ScreenPosX,
               ScreenPosY,
               worldPosition,
               normal,
               objectHandle)) {

         lua_create_vector (L, &worldPosition);
         lua_create_vector (L, &normal);
         lua_create_handle (L, objectHandle);

         result = 3;
      }
   }

   LUA_END_VALIDATE (L, result);

   return result;
}


static int
pick_world_to_screen (lua_State *L) {

   LUA_START_VALIDATE (L);

   int result (0);

   RenderModulePick *pick (get_pick (L));
   Handle *sourceHandle (lua_check_handle (L, 1));
   Vector *worldPosition (lua_check_vector (L, 2));

   if (pick && worldPosition && sourceHandle) {

      Int32 screenPosX (0);
      Int32 screenPosY (0);

      if (pick->world_to_screen (*sourceHandle, *worldPosition, screenPosX, screenPosY)) {

         lua_pushnumber (L, screenPosX);
         lua_pushnumber (L, screenPosY);

         result = 2;
      }
   }

   LUA_END_VALIDATE (L, result);

   return result;
}


static int
pick_source_to_world (lua_State *L) {

   LUA_START_VALIDATE (L);

   int result (0);

   RenderModulePick *pick (get_pick (L));
   Handle *sourceHandle (lua_check_handle (L, 1));
   const int SourcePosX (luaL_checkinteger (L, 2));
   const int SourcePosY (luaL_checkinteger (L, 3));

   if (pick && sourceHandle) {

      Vector worldPosition, normal;
      Handle objectHandle;

      if (pick->source_to_world (
               *sourceHandle,
               SourcePosX,
               SourcePosY,
               worldPosition,
               normal,
               objectHandle)) {

         lua_create_vector (L, &worldPosition);
         lua_create_vector (L, &normal);
         lua_create_handle (L, objectHandle);

         result = 3;
      }
   }

   LUA_END_VALIDATE (L, result);

   return result;
}


static int
pick_world_to_source (lua_State *L) {

   LUA_START_VALIDATE (L);

   int result (0);

   RenderModulePick *pick (get_pick (L));
   Handle *sourceHandle (lua_check_handle (L, 1));
   Vector *worldPosition (lua_check_vector (L, 2));

   if (pick && worldPosition && sourceHandle) {

      Int32 sourcePosX (0);
      Int32 sourcePosY (0);

      if (pick->world_to_source (*sourceHandle, *worldPosition, sourcePosX, sourcePosY)) {

         lua_pushnumber (L, sourcePosX);
         lua_pushnumber (L, sourcePosY);

         result = 2;
      }
   }

   LUA_END_VALIDATE (L, result);

   return result;
}


static const luaL_Reg arrayFunc[] = {
   {"screen_to_world", pick_screen_to_world},
   {"world_to_screen", pick_world_to_screen},
   {"source_to_world", pick_source_to_world},
   {"world_to_source", pick_world_to_source},
   {NULL, NULL},
};

};


//! \cond
dmz::LuaExtPick::LuaExtPick (const PluginInfo &Info, Config &local) :
      Plugin (Info),
      LuaExt (Info),
      _pick (0),
      _pickPtr (0),
      _log (Info) {

   _init (local);
}


dmz::LuaExtPick::~LuaExtPick () {

}


// Plugin Interface
void
dmz::LuaExtPick::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      if (!_pick) {

         _pick = RenderModulePick::cast (PluginPtr);

         if (_pick && _pickPtr && !(*_pickPtr)) { *_pickPtr = _pick; }
      }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_pick && (_pick == RenderModulePick::cast (PluginPtr))) {

         _pick = 0;
         if (_pickPtr) { *_pickPtr = 0; }
      }
   }
}


// LuaExt Interface
void
dmz::LuaExtPick::store_lua_module (LuaModule &module) {

}


void
dmz::LuaExtPick::open_lua_extension (lua_State *L) {

   LUA_START_VALIDATE (L);

   lua_pushlightuserdata (L, (void *)&PickKey);
   _pickPtr = (RenderModulePick **)lua_newuserdata (L, sizeof (RenderModulePick *));

   if (_pickPtr) {

      *_pickPtr = _pick;
      lua_rawset (L, LUA_REGISTRYINDEX);
   }
   else { lua_pop (L, 1); } // pop light data

   lua_create_dmz_namespace (L, "pick");
   luaL_register (L, NULL, arrayFunc);

   lua_make_readonly (L, -1);
   lua_pop (L, 1); // pop dmz.pick table

   LUA_END_VALIDATE (L, 0);
}


void
dmz::LuaExtPick::close_lua_extension (lua_State *L) {

   if (_pickPtr) { *_pickPtr = 0; _pickPtr = 0; }
   lua_pushlightuserdata (L, (void *)&PickKey);
   lua_pushnil (L);
   lua_rawset (L, LUA_REGISTRYINDEX);
}


void
dmz::LuaExtPick::remove_lua_module (LuaModule &module) {

}


void
dmz::LuaExtPick::_init (Config &local) {

}
//! \endcond


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzLuaExtPick (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::LuaExtPick (Info, local);
}

};
