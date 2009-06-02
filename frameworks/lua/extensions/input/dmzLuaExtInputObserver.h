#ifndef DMZ_LUA_EXT_EVENT_OBSERVER_DOT_H
#define DMZ_LUA_EXT_EVENT_OBSERVER_DOT_H

#include "dmzLuaExtInputObserver.h"
#include <dmzInputObserverUtil.h>
#include <dmzRuntimeHandle.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzTypesBase.h>
#include <dmzTypesString.h>

struct lua_State;

namespace dmz {

   //! \cond
   class LuaExtInputObserver;

   void lua_init_input_observer (lua_State *L);

   LuaExtInputObserver *lua_create_input_observer (lua_State *L);

   LuaExtInputObserver *lua_check_input_observer (lua_State *L, int index);

   class LuaExtInputObserver : public PluginInfo, public InputObserverUtil {

      public:
         LuaExtInputObserver (lua_State *TheState, Config &empty);
         ~LuaExtInputObserver ();

         int get_index () const;

         // Input Observer Interface
         virtual void update_channel_state (const Handle Channel, const Boolean State);

         virtual void receive_axis_event (
            const Handle Channel,
            const InputEventAxis &Value);

         virtual void receive_button_event (
            const Handle Channel,
            const InputEventButton &Value);

         virtual void receive_switch_event (
            const Handle Channel,
            const InputEventSwitch &Value);

         virtual void receive_key_event (
            const Handle Channel,
            const InputEventKey &Value);

         virtual void receive_mouse_event (
            const Handle Channel,
            const InputEventMouse &Value);

         virtual void receive_data_event (
            const Handle Channel,
            const Handle Source,
            const Data &Value);

      protected:
         const RuntimeHandle _Handle;
         lua_State *L;
         int _index;
         //! \endcond

      private:
         LuaExtInputObserver ();
         LuaExtInputObserver (const InputObserver &);
         LuaExtInputObserver &operator= (const InputObserver &);
   };
};


//! \cond
inline int
dmz::LuaExtInputObserver::get_index () const { return _index; }
//! \endcond

#endif // DMZ_LUA_EXT_EVENT_OBSERVER_DOT_H

