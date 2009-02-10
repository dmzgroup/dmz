#include "dmzLuaExtIsect.h"
#include <dmzLuaKernel.h>
#include <dmzLuaKernelValidate.h>
#include <dmzRenderIsect.h>
#include <dmzRenderIsectUtil.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzTypesMask.h>
#include <dmzTypesVector.h>

#include <luacpp.h>

#include <qdb.h>
static dmz::qdb out;

/*!
         
\class dmz::LuaExtIsect
\ingroup Lua
\brief Provides a Lua API for the Render Isect Module.
\details
\htmlonly See the <a href="dmzlua.html#dmz.isect">Lua Isect API</a>.
\endhtmlonly
         
*/ 

using namespace dmz;

namespace {

typedef LuaExtIsect::IsectStruct istruct;

static char IsectKey = 'i';


static inline RenderModuleIsect *
get_isect (lua_State *L) {

  RenderModuleIsect *result (0);
  lua_pushlightuserdata (L, (void *)&IsectKey);
  lua_rawget (L, LUA_REGISTRYINDEX);
  istruct **is = (istruct **)lua_touserdata (L, -1);
  if (is && *is) { result = (*is)->isect; }
  lua_pop (L, 1); // pop isect module
  return result;
}


static inline istruct *
get_isect_struct (lua_State *L) {

  istruct *result (0);
  lua_pushlightuserdata (L, (void *)&IsectKey);
  lua_rawget (L, LUA_REGISTRYINDEX);
  istruct **is = (istruct **)lua_touserdata (L, -1);
  if (is) { result = *is; }
  lua_pop (L, 1); // pop isect module
  return result;
}


static void
table_to_test (
      lua_State *L,
      istruct &is,
      int index,
      UInt32 handle,
      IsectTestContainer &test) {

   IsectTestTypeEnum type (IsectRayTest);

   if (index < 0) { index = lua_gettop (L) + index + 1; }

   lua_getfield (L, index, "type");

   if (!lua_isnil (L, -1)) {

      int IntType = luaL_checkinteger (L, -1);

      if (IntType == is.rayTest) { type = IsectRayTest; }
      else if (IntType == is.segmentTest) { type = IsectSegmentTest; }
      else { lua_enum_error (L, 0, IntType); }
   }

   lua_pop (L, 1); // pop type

   lua_getfield (L, index, "start");
   lua_getfield (L, index, "vector");
   Vector *vec1 (lua_check_vector (L, -2));
   Vector *vec2 (lua_check_vector (L, -1));

   if (vec1 && vec2) { test.set_test (handle, type, *vec1, *vec2); }

   lua_pop (L, 2); // pop both vectors
}


static void
table_to_params (lua_State *L, istruct &is, int index, IsectParameters &params) {

   if (index < 0) { index = lua_gettop (L) + index + 1; }

   lua_getfield (L, index, "type");
   if (!lua_isnil (L, -1)) {

      IsectTestResultTypeEnum type (params.get_test_result_type ());

      int IntType = luaL_checkinteger (L, -1);

      if (IntType == is.firstPoint) { type = IsectFirstPoint; }
      else if (IntType == is.closestPoint) { type = IsectClosestPoint; }
      else if (IntType == is.allPoints) { type = IsectAllPoints; }
      else { lua_enum_error (L, 0, IntType); }

      params.set_test_result_type (type);
   }
   lua_pop (L, 1); // pop type

   lua_getfield (L, index, "normal");
   if (!lua_isnil (L, -1)) {

      params.set_calculate_normal (lua_toboolean (L, -1) != 0);
   }
   lua_pop (L, 1); // pop normal

   lua_getfield (L, index, "object_handle");
   if (!lua_isnil (L, -1)) {

      params.set_calculate_object_handle (lua_toboolean (L, -1) != 0);
   }
   lua_pop (L, 1); // pop object_handle

   lua_getfield (L, index, "distance");
   if (!lua_isnil (L, -1)) {

      params.set_calculate_distance (lua_toboolean (L, -1) != 0);
   }
   lua_pop (L, 1); // pop distance

   lua_getfield (L, index, "cull");
   if (!lua_isnil (L, -1)) {

      params.set_calculate_cull_mode (lua_toboolean (L, -1) != 0);
   }
   lua_pop (L, 1); // pop cull
}


static int
result_to_table (lua_State *L, istruct &is, IsectResultContainer &iresults) {

   const int Count = (int)iresults.get_result_count ();

   lua_createtable (L, Count, 0);
   const int Table = lua_gettop (L);
   int index (1);

   IsectResult value;
   Boolean found (iresults.get_first (value));

   while (found) {

      lua_createtable (L, 0, 8);
      const int ValueTable = lua_gettop (L);

      lua_pushinteger (L, (int)value.get_isect_test_id ());
      lua_setfield (L, ValueTable, "test");

      Vector vec;
      if (value.get_point (vec)) {

         lua_create_vector (L, &vec);
         lua_setfield (L, ValueTable, "point");
      }

      if (value.get_normal (vec)) {

         lua_create_vector (L, &vec);
         lua_setfield (L, ValueTable, "normal");
      }

      Handle objHandle (0);
      if (value.get_object_handle (objHandle)) {

         lua_create_handle (L, objHandle);
         lua_setfield (L, ValueTable, "object");
      }

      Float64 distance (0.0);
      if (value.get_distance (distance)) {

         lua_pushnumber (L, distance);
         lua_setfield (L, ValueTable, "distance");
      }

      UInt32 cull (0);
      if (value.get_cull_mode (cull)) {

         const Mask CullMask (0, cull);
         lua_create_mask (L, &CullMask);
         lua_setfield (L, ValueTable, "cull");
      }

      lua_rawseti (L, Table, index);
      index++;
      found = iresults.get_next (value);
   }

   return 1;
}


static int
isect_clamp (lua_State *L) {

   LUA_START_VALIDATE (L);

   int result (0);

   RenderModuleIsect *isect (get_isect (L));
   Vector *vec = lua_check_vector (L, 1);

   if (isect && vec) {

      Vector point, normal;

      if (isect_clamp_point (*vec, *isect, point, normal)) {

         lua_create_vector (L, &point);
         lua_create_vector (L, &normal);

         result = 2;
      }
   }
   else { lua_pushnil (L); result = 1; }

   LUA_END_VALIDATE (L, result);

   return result;
}


static int
isect_enable_isect (lua_State *L) {

   RenderModuleIsect *isect (get_isect (L));
   Handle *handle (lua_check_handle (L, 1));

   if (isect && handle) {

      lua_pushboolean (L, isect->enable_isect (*handle) ? 1 : 0);
   }
   else { lua_pushboolean (L, 0); }

   return 1;
}


static int
isect_disable_isect (lua_State *L) {

   RenderModuleIsect *isect (get_isect (L));
   Handle *handle (lua_check_handle (L, 1));

   if (isect && handle) {

      lua_pushboolean (L, isect->disable_isect (*handle) ? 1 : 0);
   }
   else { lua_pushboolean (L, 0); }

   return 1;
}


static int
isect_do_isect (lua_State *L) {

   int result (0);

   istruct *is (get_isect_struct (L));
   luaL_checktype (L, 1, LUA_TTABLE);

   if (is && is->isect) {

      IsectTestContainer tests;
      IsectParameters params;
      IsectResultContainer iresults;
      table_to_test (L, *is, 1, 1, tests);
      if (!lua_isnoneornil (L, 2)) { table_to_params (L, *is, 2, params); }

      if (is->isect->do_isect (params, tests, iresults)) {

         result = result_to_table (L, *is, iresults);
      }
   }

   return result;
}


static int
isect_do_batch_isect (lua_State *L) {

   LUA_START_VALIDATE (L);

   int result (0);

   istruct *is (get_isect_struct (L));
   luaL_checktype (L, 1, LUA_TTABLE);

   if (is && is->isect) {

      IsectTestContainer tests;
      IsectParameters params;
      IsectResultContainer iresults;

      const int Length = lua_objlen (L, 1);

      for (int ix = 1; ix <= Length; ix++) {

         lua_rawgeti (L, 1, ix);

         if (lua_istable (L, -1)) { table_to_test (L, *is, -1, (UInt32)ix, tests); }

         lua_pop (L, 1); // pop table from rawget
      }

      if (!lua_isnoneornil (L, 2)) { table_to_params (L, *is, 2, params); }

      if (is->isect->do_isect (params, tests, iresults)) {

         result = result_to_table (L, *is, iresults);
      }
   }

   LUA_END_VALIDATE (L, result);

   return result;
}


static const luaL_Reg arrayFunc[] = {
   {"clamp", isect_clamp},
   {"enable_isect", isect_enable_isect},
   {"disable_isect", isect_disable_isect},
   {"do_isect", isect_do_isect},
   {"do_batch_isect", isect_do_batch_isect},
   {NULL, NULL},
};

};


//! \cond
dmz::LuaExtIsect::LuaExtIsect (const PluginInfo &Info, Config &local) :
      Plugin (Info),
      LuaExt (Info),
      _isectPtr (0),
      _log (Info) {

   _init (local);
}


dmz::LuaExtIsect::~LuaExtIsect () {

}


// Plugin Interface
void
dmz::LuaExtIsect::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      if (!_isect.isect) { _isect.isect = RenderModuleIsect::cast (PluginPtr); }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_isect.isect && (_isect.isect == RenderModuleIsect::cast (PluginPtr))) {

         _isect.isect = 0;
      }
   }
}


// LuaExt Interface
void
dmz::LuaExtIsect::store_lua_module (LuaModule &module) {

}


void
dmz::LuaExtIsect::open_lua_extension (lua_State *L) {

   LUA_START_VALIDATE (L);

   lua_pushlightuserdata (L, (void *)&IsectKey);
   _isectPtr = (IsectStruct **)lua_newuserdata (L, sizeof (IsectStruct *));

   if (_isectPtr) {

      *_isectPtr = &_isect;
      lua_rawset (L, LUA_REGISTRYINDEX);
   }
   else { lua_pop (L, 1); } // pop light data

   lua_create_dmz_namespace (L, "isect");
   luaL_register (L, NULL, arrayFunc);

   _isect.rayTest = lua_create_enum (L, "dmzIsectRayTest");
   _isect.segmentTest = lua_create_enum (L, "dmzIsectSegmentTest");
   _isect.firstPoint = lua_create_enum (L, "dmzIsectFirstPoint");
   _isect.closestPoint = lua_create_enum (L, "dmzIsectClosestPoint");
   _isect.allPoints = lua_create_enum (L, "dmzIsectAllPoints");

   lua_pushinteger (L, _isect.rayTest);
   lua_setfield (L, -2, "RayTest");

   lua_pushinteger (L, _isect.segmentTest);
   lua_setfield (L, -2, "SegmentTest");

   lua_pushinteger (L, _isect.firstPoint);
   lua_setfield (L, -2, "FirstPoint");

   lua_pushinteger (L, _isect.closestPoint);
   lua_setfield (L, -2, "ClosestPoint");

   lua_pushinteger (L, _isect.allPoints);
   lua_setfield (L, -2, "AllPoints");

   const Mask BackCull (0, IsectPolygonBackCulledMask);
   const Mask FrontCull (0, IsectPolygonFrontCulledMask);
   const Mask Invisible (0, IsectPolygonInvisibleMask);

   lua_create_mask (L, &BackCull);
   lua_setfield (L, -2, "BackCull");

   lua_create_mask (L, &FrontCull);
   lua_setfield (L, -2, "FrontCull");

   lua_create_mask (L, &Invisible);
   lua_setfield (L, -2, "Invisible");

   lua_make_readonly (L, -1);
   lua_pop (L, 1); // pop dmz.isect table

   LUA_END_VALIDATE (L, 0);
}


void
dmz::LuaExtIsect::close_lua_extension (lua_State *L) {

   if (_isectPtr) { *_isectPtr = 0; }
   lua_pushlightuserdata (L, (void *)&IsectKey);
   lua_pushnil (L);
   lua_rawset (L, LUA_REGISTRYINDEX);
}


void
dmz::LuaExtIsect::remove_lua_module (LuaModule &module) {

}


void
dmz::LuaExtIsect::_init (Config &local) {

}
//! \endcond


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzLuaExtIsect (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::LuaExtIsect (Info, local);
}

};
