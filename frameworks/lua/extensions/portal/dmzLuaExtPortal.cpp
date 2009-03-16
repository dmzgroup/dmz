#include "dmzLuaExtPortal.h"
#include <dmzLuaKernel.h>
#include <dmzLuaKernelValidate.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzTypesMatrix.h>
#include <dmzTypesVector.h>

#include <luacpp.h>

#include <qdb.h>
static dmz::qdb out;

/*!
         
\class dmz::LuaExtPortal
\ingroup Lua
\brief Provides a Lua API for the Portals.
\details
This API updates both render and audio portals.
\htmlonly See the <a href="dmzlua.html#dmz.portal">Lua Portal API</a>.
\endhtmlonly
         
*/

using namespace dmz;

namespace {

static char PortalKey = 'p';

static inline LuaExtPortal *
get_portal (lua_State *L) {

   LuaExtPortal *result (0);
   lua_pushlightuserdata (L, (void *)&PortalKey);
   lua_rawget (L, LUA_REGISTRYINDEX);
   LuaExtPortal **ptr = (LuaExtPortal **)lua_touserdata (L, -1);
   if (ptr) { result = *ptr; }
   lua_pop (L, 1); // pop portal ext

   return result;
}


static int
portal_set_view (lua_State *L) {

   LuaExtPortal *portal = get_portal (L);
   Vector *pos = lua_check_vector (L, 1);
   Matrix *ori = lua_check_matrix (L, 2);
   String name;
   if (!lua_isnone (L, 3)) { name = lua_tostring (L, 3); }

   if (portal && pos && ori) { portal->set_view (name, *pos, *ori); }

   return 0;
}


static int
portal_get_view (lua_State *L) {

   int result (0);

   LuaExtPortal *portal = get_portal (L);
   String name;
   if (!lua_isnone (L, 1)) { name = lua_tostring (L, 1); }

   if (portal) {

      Vector pos;
      Matrix ori;
      portal->get_view (name, pos, ori);

      lua_create_vector (L, &pos);
      lua_create_matrix (L, &ori);
      result = 2;
   }

   return result;
}

static const luaL_Reg arrayFunc[] = {
   {"set_view", portal_set_view},
   {"get_view", portal_get_view},
   {NULL, NULL},
};

};


//! \cond
dmz::LuaExtPortal::LuaExtPortal (const PluginInfo &Info, Config &local) :
      Plugin (Info),
      LuaExt (Info),
      _audioMaster (0),
      _renderMaster (0),
      _log (Info) {
}


void
dmz::LuaExtPortal::set_view (const String &Name, const Vector &Pos, const Matrix &Ori) {

   if (!Name) {

      if (_audioMaster) { _audioMaster->set_view (Pos, Ori, Vector (0.0, 0.0, 0.0)); }
      if (_renderMaster) { _renderMaster->set_view (Pos, Ori); }
   }
}


void
dmz::LuaExtPortal::get_view (const String &Name, Vector &pos, Matrix &ori) {

   if (!Name) {

      if (_renderMaster) { _renderMaster->get_view (pos, ori); }
      else if (_audioMaster) { Vector v; _audioMaster->get_view (pos, ori, v); }
   }
}


// Plugin Interface
void
dmz::LuaExtPortal::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      AudioModulePortal *audioPortal (AudioModulePortal::cast (PluginPtr));

      if (audioPortal) { // && audioPortal->is_master_portal ()) {

         _audioMaster = audioPortal;
      }

      RenderModulePortal *renderPortal (RenderModulePortal::cast (PluginPtr));

      if (renderPortal) { //  && renderPortal->is_master_portal ()) {

         _renderMaster = renderPortal;
      }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_audioMaster && (_audioMaster == AudioModulePortal::cast (PluginPtr))) {

         _audioMaster = 0;
      }

      if (_renderMaster && (_renderMaster == RenderModulePortal::cast (PluginPtr))) {

         _renderMaster = 0;
      }
   }
}


// LuaExt Interface
void
dmz::LuaExtPortal::open_lua_extension (lua_State *L) {

   LUA_START_VALIDATE (L);

   lua_pushlightuserdata (L, (void *)&PortalKey);
   LuaExtPortal **ptr = (LuaExtPortal **)lua_newuserdata (L, sizeof (LuaExtPortal *));

   if (ptr) { *ptr = this; lua_rawset (L, LUA_REGISTRYINDEX); }
   else { lua_pop (L, 1); } // pop light data

   lua_create_dmz_namespace (L, "portal");
   luaL_register (L, NULL, arrayFunc);

   lua_make_readonly (L, -1);
   lua_pop (L, 1); // pop dmz.portal table

   LUA_END_VALIDATE (L, 0);
}


void
dmz::LuaExtPortal::close_lua_extension (lua_State *L) {

   lua_pushlightuserdata (L, (void *)&PortalKey);
   lua_pushnil (L);
   lua_rawset (L, LUA_REGISTRYINDEX);
}
//! \endcond


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzLuaExtPortal (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::LuaExtPortal (Info, local);
}

};
