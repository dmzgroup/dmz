#include "dmzLuaExtTest.h"
#include <dmzLuaKernel.h>
#include <dmzLuaKernelValidate.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>

#include <luacpp.h>

/*!
         
\class dmz::LuaExtTest
\ingroup Lua
\brief Provides a Lua API for unit tests.
\details
\htmlonly See the <a href="dmzlua.html#dmz.test">Lua Test API</a>.
\endhtmlonly
         
*/

using namespace dmz;

namespace {

static char TestKey = 't';


static inline LuaExtTest *
get_test (lua_State *L) {

  LuaExtTest *result (0);
  lua_pushlightuserdata (L, (void *)&TestKey);
  lua_rawget (L, LUA_REGISTRYINDEX);
  LuaExtTest **test = (LuaExtTest **)lua_touserdata (L, -1);
  if (test) { result = *test; }
  lua_pop (L, 1); // pop test module
  return result;
}


static int
test_start (lua_State *L) {

   LUA_START_VALIDATE (L);

   LuaExtTest *test = get_test (L);
   const String Name = luaL_checkstring (L, 1);

   if (test) { test->start_test (Name); }

   LUA_END_VALIDATE (L, 0);

   return 0;
}


static int
test_validate (lua_State *L) {

   LUA_START_VALIDATE (L);

   LuaExtTest *test = get_test (L);
   const Boolean Value = (lua_toboolean (L, 1) == 1);
   const String Message = luaL_checkstring (L, 2);

   if (test) { test->validate (Value, Message); }

   LUA_END_VALIDATE (L, 0);

   return 0;
}


static int
test_stop (lua_State *L) {

   LUA_START_VALIDATE (L);

   LuaExtTest *test = get_test (L);

   if (test) { test->stop_test (); }

   LUA_END_VALIDATE (L, 0);

   return 0;
}


static const luaL_Reg arrayFunc[] = {
   {"start", test_start},
   {"validate", test_validate},
   {"stop", test_stop},
   {NULL, NULL},
};

};


//! \cond
dmz::LuaExtTest::LuaExtTest (const PluginInfo &Info, Config &local) :
      Plugin (Info),
      LuaExt (Info),
      _failed (False),
      _exit (Info.get_context ()),
      _testPtr (0),
      _testLog ("", Info.get_context ()),
      _log (Info) {;}


// LuaExt Interface
void
dmz::LuaExtTest::open_lua_extension (lua_State *L) {

   LUA_START_VALIDATE (L);

   lua_pushlightuserdata (L, (void *)&TestKey);
   _testPtr = (LuaExtTest **)lua_newuserdata (L, sizeof (LuaExtTest *));

   if (_testPtr) {

      *_testPtr = this;
      lua_rawset (L, LUA_REGISTRYINDEX);
   }
   else { lua_pop (L, 1); } // pop light data

   lua_create_dmz_namespace (L, "test");
   luaL_register (L, NULL, arrayFunc);

   lua_make_readonly (L, -1);
   lua_pop (L, 1); // pop dmz.test table

   LUA_END_VALIDATE (L, 0);
}


void
dmz::LuaExtTest::close_lua_extension (lua_State *L) {

   if (_testPtr) { *_testPtr = 0; }
   lua_pushlightuserdata (L, (void *)&TestKey);
   lua_pushnil (L);
   lua_rawset (L, LUA_REGISTRYINDEX);
}


void
dmz::LuaExtTest::start_test (const String &Name) {

   _testLog.out
      << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-" << endl
      << "Running Lua " << Name << " unit test." << endl
      << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-" << endl;
}


void
dmz::LuaExtTest::validate (const Boolean Value, const String &Message) {

   if (Value) { _testLog.out << "PASSED: "; }
   else { _testLog.out << "FAILED: "; _failed = True; }
   _testLog.out << Message << endl;
}


void
dmz::LuaExtTest::stop_test () {

   const ExitStatusEnum Status = _failed ? ExitStatusError : ExitStatusNormal;
   _exit.request_exit (Status, "End of unit test.");
}
//! \endcond


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzLuaExtTest (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::LuaExtTest (Info, local);
}

};
