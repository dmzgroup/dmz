#ifndef DMZ_LUA_EXT_TEST_DOT_H
#define DMZ_LUA_EXT_TEST_DOT_H

#include <dmzLuaExt.h>
#include <dmzRuntimeExit.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>

namespace dmz {

   class LuaExtTest :
         public Plugin,
         public LuaExt {

      public:
         //! \cond
         LuaExtTest (const PluginInfo &Info, Config &local);
         ~LuaExtTest () {;}

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level) {;}

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr) {;}

         // LuaExt Interface
         virtual void store_lua_module (LuaModule &module) {;}
         virtual void open_lua_extension (lua_State *L);
         virtual void close_lua_extension (lua_State *L);
         virtual void remove_lua_module (LuaModule &module) {;}

         // LuaExtTest Interface
         void start_test (const String &Name);
         void validate (const Boolean Value, const String &Message);
         void stop_test ();

      protected:

         Boolean _failed;

         Exit _exit;

         LuaExtTest **_testPtr;

         Log _testLog;
         Log _log;
         //! \endcond

      private:
         LuaExtTest ();
         LuaExtTest (const LuaExtTest &);
         LuaExtTest &operator= (const LuaExtTest &);

   };
};

#endif // DMZ_LUA_EXT_TEST_DOT_H
