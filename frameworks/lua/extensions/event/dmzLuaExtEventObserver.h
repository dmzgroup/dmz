#ifndef DMZ_LUA_EXT_EVENT_OBSERVER_DOT_H
#define DMZ_LUA_EXT_EVENT_OBSERVER_DOT_H

#include "dmzLuaExtEventObserver.h"
#include <dmzEventObserverUtil.h>
#include <dmzRuntimeHandle.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzTypesBase.h>
#include <dmzTypesString.h>

struct lua_State;

namespace dmz {

   //! \cond
   class LuaExtEventObserver;

   void lua_init_event_observer (lua_State *L);

   LuaExtEventObserver *lua_create_event_observer (lua_State *L);

   LuaExtEventObserver *lua_check_event_observer (lua_State *L, int index);

   class LuaExtEventObserver : public PluginInfo, public EventObserverUtil {

      public:
         LuaExtEventObserver (lua_State *TheState, Config &empty);
         ~LuaExtEventObserver ();

         int get_index () const;

         virtual void create_event (
            const Handle EventHandle,
            const EventType &Type,
            const EventLocalityEnum Locality);

         virtual void close_event (
            const Handle EventHandle,
            const EventType &Type,
            const EventLocalityEnum Locality);

      protected:
         const RuntimeHandle _Handle;
         lua_State *L;
         int _index;
         //! \endcond

      private:
         LuaExtEventObserver ();
         LuaExtEventObserver (const EventObserver &);
         LuaExtEventObserver &operator= (const EventObserver &);
   };
};


//! \cond
inline int
dmz::LuaExtEventObserver::get_index () const { return _index; }
//! \endcond

#endif // DMZ_LUA_EXT_EVENT_OBSERVER_DOT_H

