#ifndef DMZ_LUA_OBSERVER_DOT_H
#define DMZ_LUA_OBSERVER_DOT_H

#include <dmzRuntimePlugin.h>
#include <dmzRuntimeRTTI.h>
#include <dmzTypesBase.h>

struct lua_State;
struct lua_Debug;

namespace dmz {

   //! \cond
   const char LuaObserverInterfaceName[] = "LuaObserverInterface";
   //! \endcond

   class LuaModule;

   //! \addtogroup Lua
   //! @{
   const UInt32 LuaHookCallMask        = 0x0001; //!< Call hook mask.
   const UInt32 LuaHookReturnMask      = 0x0002; //!< Return hook mask.
   const UInt32 LuaHookLineMask        = 0x0004; //!< Line hook mask.
   const UInt32 LuaHookCountMask       = 0x0008; //!< Count hook mask.
   const UInt32 LuaHookCountUpdateMask = 0x0010; //!< Count hook update mask.
   const UInt32 LuaHookAllMask         = 0x0FFF; //!< All hooks mask.
   const UInt32 LuaScriptUpdateMask    = 0x1000; //!< Script update mask.
   const UInt32 LuaErrorMask           = 0x2000; //!< Lua error mask.
   const UInt32 LuaAllCallbacksMask    = 0xFFFF; //!< All callbacks mask.
   //! @}

   class LuaObserver {

      public:
         static LuaObserver *cast (
            const Plugin *PluginPtr,
            const String &PluginName = "");

         String get_lua_observer_name ();
         Handle get_lua_observer_handle ();

         // LuaObserver Interface
         virtual void store_lua_module (const String &ModuleName, LuaModule &module) = 0;
         virtual void remove_lua_module (const String &ModuleName, LuaModule &module) = 0;

         virtual void add_lua_script (lua_State *L, const String &Path) = 0;
         virtual void remove_lua_script (lua_State *L, const String &Path) = 0;
         virtual void update_lua_hook_count (lua_State *L, const Int32 Count) = 0;
         virtual void lua_hook (lua_State *L, lua_Debug *ar) = 0;
         virtual void lua_parse_error (lua_State *L) = 0;
         virtual void lua_runtime_error (lua_State *L) = 0;

      protected:
         LuaObserver (const PluginInfo &Info);
         ~LuaObserver ();

      private:
         LuaObserver ();
         LuaObserver (const LuaObserver &);
         LuaObserver &operator= (const LuaObserver &);

         const PluginInfo &__Info;
   };
};


inline dmz::LuaObserver *
dmz::LuaObserver::cast (const Plugin *PluginPtr, const String &PluginName) {

   return (LuaObserver *)lookup_rtti_interface (
      LuaObserverInterfaceName,
      PluginName,
      PluginPtr);
}


inline
dmz::LuaObserver::LuaObserver (const PluginInfo &Info) :
      __Info (Info) {

   store_rtti_interface (LuaObserverInterfaceName, __Info, (void *)this);
}


inline
dmz::LuaObserver::~LuaObserver () {

   remove_rtti_interface (LuaObserverInterfaceName, __Info);
}


inline dmz::String
dmz::LuaObserver::get_lua_observer_name () { return __Info.get_name (); }


inline dmz::Handle
dmz::LuaObserver::get_lua_observer_handle () { return __Info.get_handle (); }

#endif // DMZ_LUA_OBSERVER_DOT_H
