#ifndef DMZ_LUA_MODULE_DOT_H
#define DMZ_LUA_MODULE_DOT_H

#include <dmzRuntimePlugin.h>
#include <dmzRuntimeRTTI.h>
#include <dmzTypesBase.h>

struct lua_State;

namespace dmz {

   //! \cond
   const char LuaModuleInterfaceName[] = "LuaModuleInterface";
   //! \endcond

   class LuaObserver;

   class LuaModule {

      public:
         static LuaModule *cast (const Plugin *PluginPtr, const String &PluginName = "");

         // LuaModule Interface
         virtual void add_lua_path (const String &Path) = 0;

         virtual void register_lua_observer (
            const UInt32 CallbackMask,
            LuaObserver &observer) = 0;

         virtual void release_lua_observer (
            const UInt32 CallbackMask,
            LuaObserver &observer) = 0;

         virtual void reset_lua () = 0;

         virtual void set_hook_count (const Int32 Count) = 0;
         virtual Int32 get_hook_count () = 0;

      protected:
         LuaModule (const PluginInfo &Info);
         ~LuaModule ();

      private:
         LuaModule ();
         LuaModule (const LuaModule &);
         LuaModule &operator= (const LuaModule &);

         const PluginInfo &__Info;
   };
};


inline dmz::LuaModule *
dmz::LuaModule::cast (const Plugin *PluginPtr, const String &PluginName) {

   return (LuaModule *)lookup_rtti_interface (
      LuaModuleInterfaceName,
      PluginName,
      PluginPtr);
}


inline
dmz::LuaModule::LuaModule (const PluginInfo &Info) :
      __Info (Info) {

   store_rtti_interface (LuaModuleInterfaceName, __Info, (void *)this);
}


inline
dmz::LuaModule::~LuaModule () {

   remove_rtti_interface (LuaModuleInterfaceName, __Info);
}

#endif // DMZ_LUA_MODULE_DOT_H
