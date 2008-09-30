#ifndef DMZ_LUA_EXT_DOT_H
#define DMZ_LUA_EXT_DOT_H

#include <dmzRuntimePlugin.h>
#include <dmzRuntimeRTTI.h>
#include <dmzTypesBase.h>

struct lua_State;

namespace dmz {

   //! \cond
   const char LuaExtInterfaceName[] = "LuaExtInterface";
   //! \endcond

   class LuaModule;

   class LuaExt {

      public:
         static LuaExt *cast (
            const Plugin *PluginPtr,
            const String &PluginName = "");

         Handle get_lua_extension_handle ();
         String get_lua_extension_name ();

         // LuaExt Interface
         virtual void store_lua_module (LuaModule &module) = 0;
         virtual void open_lua_extension (lua_State *L) = 0;
         virtual void close_lua_extension (lua_State *L) = 0;
         virtual void remove_lua_module (LuaModule &module) = 0;

      protected:
         LuaExt (const PluginInfo &Info);
         ~LuaExt ();

      private:
         LuaExt ();
         LuaExt (const LuaExt &);
         LuaExt &operator= (const LuaExt &);

         const PluginInfo &__Info;
   };
};


inline dmz::LuaExt *
dmz::LuaExt::cast (const Plugin *PluginPtr, const String &PluginName) {

   return (LuaExt *)lookup_rtti_interface (
      LuaExtInterfaceName,
      PluginName,
      PluginPtr);
}


inline dmz::Handle
dmz::LuaExt::get_lua_extension_handle () { return __Info.get_handle (); }


inline dmz::String
dmz::LuaExt::get_lua_extension_name () { return __Info.get_name (); }


inline
dmz::LuaExt::LuaExt (const PluginInfo &Info) :
      __Info (Info) {

   store_rtti_interface (LuaExtInterfaceName, __Info, (void *)this);
}


inline
dmz::LuaExt::~LuaExt () {

   remove_rtti_interface (LuaExtInterfaceName, __Info);
}

#endif // DMZ_LUA_EXT_DOT_H
