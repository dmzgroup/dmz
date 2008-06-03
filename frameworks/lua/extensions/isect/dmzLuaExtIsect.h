#ifndef DMZ_LUA_EXT_ISECT_DOT_H
#define DMZ_LUA_EXT_ISECT_DOT_H

#include <dmzLuaExt.h>
#include <dmzRenderModuleIsect.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>

namespace dmz {

   class LuaExtIsect :
         public Plugin,
         public LuaExt {

      public:
         struct IsectStruct {

            RenderModuleIsect *isect;
            int rayTest;
            int segmentTest;
            int firstPoint;
            int closestPoint;
            int allPoints;

            IsectStruct () :
                  isect (0),
                  rayTest (0),
                  segmentTest (0),
                  firstPoint (0),
                  closestPoint (0),
                  allPoints (0) {;}
         };

         LuaExtIsect (const PluginInfo &Info, Config &local);
         ~LuaExtIsect ();

         // Plugin Interface
         virtual void discover_plugin (const Plugin *PluginPtr);
         virtual void start_plugin ();
         virtual void stop_plugin ();
         virtual void shutdown_plugin ();
         virtual void remove_plugin (const Plugin *PluginPtr);

         // LuaExt Interface
         virtual void store_lua_module (LuaModule &module);
         virtual void open_lua_extension (lua_State *L);
         virtual void close_lua_extension (lua_State *L);
         virtual void remove_lua_module (LuaModule &module);

      protected:

         void _init (Config &local);

         IsectStruct _isect;
         IsectStruct **_isectPtr;

         Log _log;

      private:
         LuaExtIsect ();
         LuaExtIsect (const LuaExtIsect &);
         LuaExtIsect &operator= (const LuaExtIsect &);

   };
};

#endif // DMZ_LUA_EXT_ISECT_DOT_H
