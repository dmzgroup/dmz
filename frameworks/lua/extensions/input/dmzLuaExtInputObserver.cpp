#include "dmzLuaExtInput.h"
#include "dmzLuaExtInputObserver.h"
#include <dmzLuaKernel.h>
#include <dmzLuaKernelErrorHandler.h>
#include <dmzLuaKernelValidate.h>
#include <dmzInputConsts.h>
#include <dmzInputEventController.h>
#include <dmzInputEventKey.h>
#include <dmzInputEventMouse.h>
#include <dmzInputEventMasks.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzTypesMask.h>

#include <luacpp.h>

#ifdef _WIN32
// This disables the warning regarding using the 'this' pointer as an
// argument in the constructor. The pointer isn't actually used for anything
// other than its value.
#pragma warning (disable : 4355)
#endif // _Win32

#include <qdb.h>
static dmz::qdb out;


using namespace dmz;

namespace {

typedef LuaExtInputObserver * optr;

static const char InputObserverName[] = "dmz.input.observer";

static const char InputUpdateChannelStateFunc[] = "update_channel_state";
static const char InputReceiveAxisEventFunc[]   = "receive_axis_event";
static const char InputReceiveButtonEventFunc[]   = "receive_button_event";
static const char InputReceiveSwitchEventFunc[]   = "receive_switch_event";
static const char InputReceiveKeyEventFunc[]   = "receive_key_event";
static const char InputReceiveMouseEventFunc[]   = "receive_mouse_event";
static const char InputReceiveDataEventFunc[]   = "receive_data_event";

static void
axis_event_to_table (lua_State *L, const InputEventAxis &Value) {

   LUA_START_VALIDATE (L);

   lua_createtable (L, 0, 5);
   const int Table = lua_gettop (L);

   lua_create_handle (L, Value.get_source_handle ());
   lua_setfield (L, Table, "source");

   lua_pushinteger (L, (int)Value.get_axis_id ());
   lua_setfield (L, Table, "which");

   lua_pushnumber (L, Value.get_axis_value ());
   lua_setfield (L, Table, "value");

   lua_pushnumber (L, Value.get_axis_previous_value ());
   lua_setfield (L, Table, "previous_value");

   lua_pushnumber (L, Value.get_axis_delta ());
   lua_setfield (L, Table, "delta");

   LUA_END_VALIDATE (L, 1);
}


static void
button_event_to_table (lua_State *L, const InputEventButton &Value) {

   LUA_START_VALIDATE (L);

   lua_createtable (L, 0, 5);
   const int Table = lua_gettop (L);

   lua_create_handle (L, Value.get_source_handle ());
   lua_setfield (L, Table, "source");

   lua_pushinteger (L, (int)Value.get_button_id ());
   lua_setfield (L, Table, "which");

   lua_pushboolean (L, (Value.get_button_value () ? 1 : 0));
   lua_setfield (L, Table, "value");

   lua_pushboolean (L, (Value.get_button_previous_value () ? 1 : 0));
   lua_setfield (L, Table, "previous_value");

   lua_pushboolean (L, (Value.get_button_changed () ? 1 : 0));
   lua_setfield (L, Table, "changed");

   LUA_END_VALIDATE (L, 1);
}

static void
switch_event_to_table (lua_State *L, const InputEventSwitch &Value) {

   LUA_START_VALIDATE (L);

   lua_createtable (L, 0, 5);
   const int Table = lua_gettop (L);

   lua_create_handle (L, Value.get_source_handle ());
   lua_setfield (L, Table, "source");

   lua_pushinteger (L, (int)Value.get_switch_id ());
   lua_setfield (L, Table, "which");

   lua_pushinteger (L, (int)Value.get_switch_value ());
   lua_setfield (L, Table, "value");

   lua_pushinteger (L, (int)Value.get_switch_previous_value ());
   lua_setfield (L, Table, "previous_value");

   lua_pushboolean (L, (Value.get_switch_changed () ? 1 : 0));
   lua_setfield (L, Table, "changed");

   LUA_END_VALIDATE (L, 1);
}


static void
key_event_to_table (lua_State *L, const InputEventKey &Value) {

   LUA_START_VALIDATE (L);

   lua_createtable (L, 0, 3);
   const int Table = lua_gettop (L);

   lua_create_handle (L, Value.get_source_handle ());
   lua_setfield (L, Table, "source");

   lua_pushinteger (L, (int)Value.get_key ());
   lua_setfield (L, Table, "value");

   lua_pushboolean (L, (Value.get_key_state () ? 1 : 0));
   lua_setfield (L, Table, "state");

   LUA_END_VALIDATE (L, 1);
}

static void
mouse_event_to_table (lua_State *L, const InputEventMouse &Value) {

   LUA_START_VALIDATE (L);

   lua_createtable (L, 0, 19);
   const int Table = lua_gettop (L);

   lua_create_handle (L, Value.get_source_handle ());
   lua_setfield (L, Table, "source");

   lua_pushinteger (L, (int)Value.get_window_size_x ());
   lua_setfield (L, Table, "window_size_x");

   lua_pushinteger (L, (int)Value.get_window_size_y ());
   lua_setfield (L, Table, "window_size_y");

   lua_pushinteger (L, (int)Value.get_mouse_x ());
   lua_setfield (L, Table, "x");

   lua_pushinteger (L, (int)Value.get_mouse_y ());
   lua_setfield (L, Table, "y");

   lua_pushinteger (L, (int)Value.get_previous_mouse_x ());
   lua_setfield (L, Table, "previous_x");

   lua_pushinteger (L, (int)Value.get_previous_mouse_y ());
   lua_setfield (L, Table, "previous_y");

   lua_pushinteger (L, (int)Value.get_mouse_delta_x ());
   lua_setfield (L, Table, "delta_x");

   lua_pushinteger (L, (int)Value.get_mouse_delta_y ());
   lua_setfield (L, Table, "delta_y");

   lua_pushinteger (L, (int)Value.get_mouse_screen_x ());
   lua_setfield (L, Table, "screen_x");

   lua_pushinteger (L, (int)Value.get_mouse_screen_y ());
   lua_setfield (L, Table, "screen_y");

   lua_pushinteger (L, (int)Value.get_previous_mouse_screen_x ());
   lua_setfield (L, Table, "screen_previous_x");

   lua_pushinteger (L, (int)Value.get_previous_mouse_screen_y ());
   lua_setfield (L, Table, "screen_previous_y");

   lua_pushinteger (L, (int)Value.get_mouse_screen_delta_x ());
   lua_setfield (L, Table, "screen_delta_x");

   lua_pushinteger (L, (int)Value.get_mouse_screen_delta_y ());
   lua_setfield (L, Table, "screen_delta_y");

   lua_pushinteger (L, (int)Value.get_scroll_delta_x ());
   lua_setfield (L, Table, "scroll_delta_x");

   lua_pushinteger (L, (int)Value.get_scroll_delta_y ());
   lua_setfield (L, Table, "scroll_delta_y");

   const Mask Buttons (0, Value.get_button_mask ());
   const Mask PreviousButtons (0, Value.get_previous_button_mask ());

   lua_create_mask (L, &Buttons);
   lua_setfield (L, Table, "buttons");

   lua_create_mask (L, &PreviousButtons);
   lua_setfield (L, Table, "previous_buttons");

   LUA_END_VALIDATE (L, 1);
}


static inline optr
obs_check (lua_State *L, int index) {

   optr *obs = (optr *)luaL_checkudata (L, index, InputObserverName);

   return obs ? *obs : 0;
}


static inline optr
obs_to (lua_State *L, int index) {

   optr *obs = (optr *)lua_is_object (L, index, InputObserverName);

   return obs ? *obs : 0;
}


static int
obs_delete (lua_State *L) {

   optr *obs = (optr *)luaL_checkudata (L, 1, InputObserverName);

   if (obs && *obs) {

      lua_release_input_observer (L, **obs);
      delete *obs; *obs = 0;
   }

   return 0;
}


static const int
obs_setup_cb (
      lua_State *L,
      LuaExtInputObserver &obs,
      const Handle Channel,
      const String &FuncName) {

   int result (0);

   Boolean error (False);

   lua_pushcfunction (L, lua_error_handler);
   result = lua_gettop (L);

   lua_rawgeti (L, LUA_REGISTRYINDEX, obs.get_index ());
   const int IndexTable = lua_gettop (L);

   if (lua_istable (L, IndexTable)) {

      lua_create_handle (L, Channel);
      lua_rawget (L, IndexTable);
      const int ChannelTable = lua_gettop (L);

      if (lua_istable (L, ChannelTable)) {

         lua_rawgeti (L, ChannelTable, 1);

         if (lua_istable (L, -1)) {

            lua_getfield (L, -1, FuncName.get_buffer ());

            if (lua_isfunction (L, -1)) { lua_rawgeti (L, ChannelTable, 2); }
         }
         else { error = True; }
      }
      else { error = True; }
   }
   else { error = True; }

   if (error) {

      lua_pop (L, lua_gettop (L) - (result - 1));
      result = 0;
   }

   return result;
}


static void
obs_do_cb (
      lua_State *L,
      LuaExtInputObserver &obs,
      const int ArgCount,
      const int Handler,
      const Handle Channel,
      const Mask &CBMask) {

   if (Handler) {

      if (lua_pcall (L, ArgCount + 1, 0, Handler)) {

         lua_pop (L, 1); // pop error message

         obs.deactivate_input_channel (Channel, CBMask);
      }

      lua_pop (L, lua_gettop (L) - (Handler - 1));
   }
   else {

      lua_pop (L, ArgCount);
      obs.deactivate_input_channel (Channel, CBMask);
   }
}


static const Mask &
obs_has_func (lua_State *L, int index, const String &Name, const Mask &FunctionMask) {

   static const Mask EmptyMask;

   if (index < 0) { index = lua_gettop (L) + index + 1; }

   lua_getfield (L, index, Name.get_buffer ());

   const Boolean Result = (lua_isfunction (L, -1) == 1);

   lua_pop (L, 1);

   return Result ? FunctionMask : EmptyMask;
}


int
obs_register (lua_State *L) {

   LUA_START_VALIDATE (L);

   int result (0);

   optr obs (obs_check (L, 1));
   Config *init = lua_to_config (L, 2);
   Handle *channelPtr (0);
   if (!init && !lua_isnil (L, 2)) { channelPtr = lua_check_handle (L, 2); }

   HandleContainer channelList;

   if (channelPtr) { channelList.add_handle (*channelPtr); }
   else if (init) {

      channelList = config_to_input_channels (*init, lua_get_runtime_context (L));
   }
   else {

      channelList.add_handle (
         Definitions (
            lua_get_runtime_context (L)).create_named_handle (InputChannelDefaultName));
   }

   luaL_checktype (L, 3, LUA_TTABLE);
   int data = (lua_isnoneornil (L, 4) ? 3 : 4);

   if (obs) {

      Mask cb;

      cb |= obs_has_func (L, 3, InputUpdateChannelStateFunc, InputEventChannelStateMask);
      cb |= obs_has_func (L, 3, InputReceiveAxisEventFunc, InputEventAxisMask);
      cb |= obs_has_func (L, 3, InputReceiveButtonEventFunc, InputEventButtonMask);
      cb |= obs_has_func (L, 3, InputReceiveSwitchEventFunc, InputEventSwitchMask);
      cb |= obs_has_func (L, 3, InputReceiveKeyEventFunc, InputEventKeyMask);
      cb |= obs_has_func (L, 3, InputReceiveMouseEventFunc, InputEventMouseMask);
      cb |= obs_has_func (L, 3, InputReceiveDataEventFunc, InputEventDataMask);

      Mask unreg (InputEventAllMask);
      unreg ^= cb;

      lua_rawgeti (L, LUA_REGISTRYINDEX, obs->get_index ()); // get observer index table
      const int CBTable (lua_gettop (L));

      Handle channel = channelList.get_first ();

      while (channel) {

         obs->deactivate_input_channel (channel, unreg);

         lua_create_handle (L, channel); // Push callback handle
         lua_createtable (L, 2, 0); // create table for callback and data tables
         const int Table (lua_gettop (L));
         lua_pushvalue (L, 3); // push callback table
         lua_rawseti (L, Table, 1); // store callback table
         lua_pushvalue (L, data); // push either callback or data table if it exists
         lua_rawseti (L, Table, 2); // store data table
         lua_rawset (L, CBTable); // store table of callback and data tables

         channel = channelList.get_next ();
      }

      lua_pop (L, 1); // pop observer index table.

      channel = channelList.get_first ();

      while (channel) {

         obs->activate_input_channel (channel, cb);

         channel = channelList.get_next ();
      }
   }

   LUA_END_VALIDATE (L, result);

   return result;
}


static int
obs_release (lua_State *L) {

   int result (0);

   optr obs (obs_check (L, 1));
   Handle *channelPtr (lua_check_handle (L, 2));
   Mask *mask (lua_check_mask (L, 3));

   if (obs && channelPtr && mask) {

      obs->deactivate_input_channel (*channelPtr, *mask);
      lua_pushboolean (L, 1);
      result = 1;
   }

   return result;
}


static int
obs_release_all (lua_State *L) {

   int result (0);

   optr obs (obs_check (L, 1));

   if (obs) {

      obs->deactivate_all_input_channels ();
      lua_pushboolean (L, 1);
      result = 1;
   }

   return result;
}


static const luaL_Reg arrayMembers [] = {
   {"register", obs_register},
   {"release", obs_release},
   {"release_all", obs_release_all},
   {"__gc", obs_delete},
   {NULL, NULL},
};


static int
obs_new (lua_State *L) { return lua_create_input_observer (L) ? 1 : 0; }


static const luaL_Reg arrayFunc [] = {
   {"new", obs_new},
   {NULL, NULL},
};

};


//! \cond
void
dmz::lua_init_input_observer (lua_State *L) {

   luaL_newmetatable (L, InputObserverName);
   luaL_register (L, NULL, arrayMembers);
   lua_pushvalue (L, -1);
   lua_setfield (L, -2, "__index");

   lua_create_dmz_namespace (L, "input_observer");
   luaL_register (L, NULL, arrayFunc);
   lua_make_readonly (L, -1);
   lua_pop (L, 2);
}


dmz::LuaExtInputObserver *
dmz::lua_create_input_observer (lua_State *L) {

   LuaExtInputObserver *result (0);

   LuaExtInputObserver **ptr =
      (LuaExtInputObserver **)lua_newuserdata (L, sizeof (LuaExtInputObserver));

   if (ptr) {

      Config empty;

      *ptr = new LuaExtInputObserver (L, empty);

      if (*ptr) {

         lua_register_input_observer (L, **ptr);
         luaL_getmetatable (L, InputObserverName);
         lua_setmetatable (L, -2);
      }

      result = *ptr;
   }

   return result;
}


dmz::LuaExtInputObserver *
dmz::lua_check_input_observer (lua_State *L, int index) {

    optr *obs = (optr *)luaL_checkudata (L, index, InputObserverName);

    return obs ? *obs : 0;
}


dmz::LuaExtInputObserver::LuaExtInputObserver (lua_State *TheState, Config &empty) :
      PluginInfo (
         "",
         PluginDeleteModeDoNotDelete,
         lua_get_runtime_context (TheState),
         0),
      InputObserverUtil (*((PluginInfo *)this), empty),
      _Handle ("LuaExtInputObserver", lua_get_runtime_context (TheState)),
      L (TheState),
      _index (0) {

   lua_newtable (L);
   _index = luaL_ref (L, LUA_REGISTRYINDEX);
}


dmz::LuaExtInputObserver::~LuaExtInputObserver () {;}


void
dmz::LuaExtInputObserver::update_channel_state (
      const Handle Channel,
      const Boolean State) {

   LUA_START_VALIDATE (L);

   const int Handler (obs_setup_cb (L, *this, Channel, InputUpdateChannelStateFunc));

   if (Handler) {

      lua_create_handle (L, Channel);
      lua_pushboolean (L, State ? 1 : 0);

      obs_do_cb (L, *this, 2, Handler, Channel, InputEventChannelStateMask);
   }

   LUA_END_VALIDATE (L, 0);
}


void
dmz::LuaExtInputObserver::receive_axis_event (
      const Handle Channel,
      const InputEventAxis &Value) {

   LUA_START_VALIDATE (L);

   const int Handler (obs_setup_cb (L, *this, Channel, InputReceiveAxisEventFunc));

   if (Handler) {

      lua_create_handle (L, Channel);
      axis_event_to_table (L, Value);

      obs_do_cb (L, *this, 2, Handler, Channel, InputEventAxisMask);
   }

   LUA_END_VALIDATE (L, 0);
}


void
dmz::LuaExtInputObserver::receive_button_event (
      const Handle Channel,
      const InputEventButton &Value) {

   LUA_START_VALIDATE (L);

   const int Handler (obs_setup_cb (L, *this, Channel, InputReceiveButtonEventFunc));

   if (Handler) {

      lua_create_handle (L, Channel);
      button_event_to_table (L, Value);

      obs_do_cb (L, *this, 2, Handler, Channel, InputEventButtonMask);
   }

   LUA_END_VALIDATE (L, 0);
}


void
dmz::LuaExtInputObserver::receive_switch_event (
      const Handle Channel,
      const InputEventSwitch &Value) {

   LUA_START_VALIDATE (L);

   const int Handler (obs_setup_cb (L, *this, Channel, InputReceiveSwitchEventFunc));

   if (Handler) {

      lua_create_handle (L, Channel);
      switch_event_to_table (L, Value);

      obs_do_cb (L, *this, 2, Handler, Channel, InputEventSwitchMask);
   }

   LUA_END_VALIDATE (L, 0);
}


void
dmz::LuaExtInputObserver::receive_key_event (
      const Handle Channel,
      const InputEventKey &Value) {

   LUA_START_VALIDATE (L);

   const int Handler (obs_setup_cb (L, *this, Channel, InputReceiveKeyEventFunc));

   if (Handler) {

      lua_create_handle (L, Channel);
      key_event_to_table (L, Value);

      obs_do_cb (L, *this, 2, Handler, Channel, InputEventKeyMask);
   }

   LUA_END_VALIDATE (L, 0);
}


void
dmz::LuaExtInputObserver::receive_mouse_event (
      const Handle Channel,
      const InputEventMouse &Value) {

   LUA_START_VALIDATE (L);

   const int Handler (obs_setup_cb (L, *this, Channel, InputReceiveMouseEventFunc));

   if (Handler) {

      lua_create_handle (L, Channel);
      mouse_event_to_table (L, Value);

      obs_do_cb (L, *this, 2, Handler, Channel, InputEventMouseMask);
   }

   LUA_END_VALIDATE (L, 0);
}


void
dmz::LuaExtInputObserver::receive_data_event (
      const Handle Channel,
      const Handle Source,
      const Data &Value) {

   LUA_START_VALIDATE (L);

   const int Handler (obs_setup_cb (L, *this, Channel, InputReceiveDataEventFunc));

   if (Handler) {

      lua_create_handle (L, Channel);
      lua_create_handle (L, Source);
      lua_create_data (L, &Value);

      obs_do_cb (L, *this, 3, Handler, Channel, InputEventDataMask);
   }

   LUA_END_VALIDATE (L, 0);
}
//! \endcond

