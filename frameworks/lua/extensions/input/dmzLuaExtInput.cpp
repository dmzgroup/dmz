#include "dmzLuaExtInput.h"
#include <dmzLuaKernel.h>
#include <dmzLuaKernelValidate.h>
#include <dmzLuaKernelErrorHandler.h>
#include <dmzInputConsts.h>
#include <dmzInputEventController.h>
#include <dmzInputEventKey.h>
#include <dmzInputEventMouse.h>
#include <dmzInputEventMasks.h>
#include <dmzInputModule.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzTypesMask.h>

#include <luacpp.h>

#include <qdb.h>
static dmz::qdb out;

/*!
         
\class dmz::LuaExtInput
\ingroup Lua
\brief Provides a Lua API for the Input Framework.
\details
\htmlonly See the <a href="dmzlua.html#dmz.input">Lua Input API</a>.
\endhtmlonly
         
*/       


using namespace dmz;

namespace {

static const char InputLuaName[] = "dmz.input";
static const char InputLuaKey = 'i';
static const char InputObserverLuaName[] = "dmz.input_observer";
static const char InputObserverLuaKey = 'o';


static inline LuaExtInput *
get_input (lua_State *L) {

   LUA_START_VALIDATE (L);

   LuaExtInput *result (0);
   lua_pushlightuserdata (L, (void *)&InputLuaKey);
   lua_rawget (L, LUA_REGISTRYINDEX);
   LuaExtInput **ptr = (LuaExtInput **)lua_touserdata (L, -1);
   lua_pop (L, 1); // pop LuaExtInput
   if (ptr) { result = *ptr; }

   LUA_END_VALIDATE (L, 0);

   return result;
}


static int
input_get_key_value (lua_State *L) {

   LUA_START_VALIDATE (L);

   const String Key = luaL_checkstring (L, 1);
   const UInt32 Value = string_to_key_value (Key);
   lua_pushnumber (L, (lua_Number)Value);

   LUA_END_VALIDATE (L, 1);

   return 1;
}


static int
input_get_key_string (lua_State *L) {

   LUA_START_VALIDATE (L);

   const UInt32 Value = (UInt32)luaL_checknumber (L, 1);
   const String Key  = key_value_to_string (Value);
   lua_pushstring (L, Key.get_buffer ());

   LUA_END_VALIDATE (L, 1);

   return 1;
}


static int
input_has_buttons_changed (lua_State *L) {

   LUA_START_VALIDATE (L);

   Boolean result (False);

   luaL_checktype (L, 1, LUA_TTABLE);

   lua_getfield (L, 1, "buttons");
   Mask *buttons (lua_check_mask (L, -1));
   lua_pop (L, 1);

   lua_getfield (L, 1, "previous_buttons");
   Mask *previousButtons (lua_check_mask (L, -1));
   lua_pop (L, 1);

   if (buttons && previousButtons) {

      const Mask NullMask;

      result = (*buttons ^ *previousButtons) != NullMask;
   }

   lua_pushboolean (L, result ? 1 : 0);

   LUA_END_VALIDATE (L, 1);

   return 1;
}


static int
input_is_button_changed (lua_State *L) {

   LUA_START_VALIDATE (L);

   Boolean result (False);
   const int ButtonNumber (luaL_checknumber (L, 1));

   luaL_checktype (L, 2, LUA_TTABLE);

   lua_getfield (L, 2, "buttons");
   Mask *buttons (lua_check_mask (L, -1));
   lua_pop (L, 1);

   lua_getfield (L, 2, "previous_buttons");
   Mask *previousButtons (lua_check_mask (L, -1));
   lua_pop (L, 1);

   if (ButtonNumber < 32 && buttons && previousButtons) {

      const Mask ButtonMask (0, 0x01 << (ButtonNumber - 1));

      const Mask CurMask (ButtonMask & *buttons);
      const Mask PrevMask (ButtonMask & *previousButtons);

      if (PrevMask != CurMask) { result = True; }
   }

   lua_pushboolean (L, result ? 1 : 0);

   LUA_END_VALIDATE (L, 1);

   return 1;
}


static int
input_is_button_pressed (lua_State *L) {

   LUA_START_VALIDATE (L);

   Boolean result (False);
   const int ButtonNumber (luaL_checknumber (L, 1));

   luaL_checktype (L, 2, LUA_TTABLE);

   lua_getfield (L, 2, "buttons");
   Mask *buttons (lua_check_mask (L, -1));
   lua_pop (L, 1);

   if (ButtonNumber < 32 && buttons) {

      const Mask ButtonMask (0, 0x01 << (ButtonNumber - 1));
      const Mask NullMask;

      result = (*buttons & ButtonMask) != NullMask;
   }

   lua_pushboolean (L, result ? 1 : 0);

   LUA_END_VALIDATE (L, 1);

   return 1;
}


static const luaL_Reg arrayFunc[] = {
   {"get_key_value", input_get_key_value},
   {"get_key_string", input_get_key_string},
   {"has_buttons_changed", input_has_buttons_changed},
   {"is_button_changed", input_is_button_changed},
   {"is_button_pressed", input_is_button_pressed},
   {NULL, NULL},
};


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


static int
mouse_event_is_button_pressed (lua_State *L) {

   LUA_START_VALIDATE (L);

   LUA_END_VALIDATE (L, 1);
   
   return 0;
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


static dmz::Boolean
obs_process_state (
      lua_State *L,
      const int InputTable,
      InputObserverLua *obs,
      const Handle Channel,
      const Boolean State) {

   Boolean retVal (True);
   LUA_START_VALIDATE (L);

   if (obs && obs->get_index ()) {

      lua_rawgeti (L, InputTable, obs->get_index ());

      if (lua_istable (L, -1)) {

         const int CallBackTable = lua_gettop (L);

         lua_pushcfunction (L, lua_error_handler);
         const int Handler (lua_gettop (L));

         lua_rawgeti (L, CallBackTable, 1);

         if (lua_isfunction (L, -1)) {

            lua_rawgeti (L, CallBackTable, 2);

            lua_createtable (L, 0, 2);
            const int Table (lua_gettop (L));

            lua_create_handle (L, Channel);
            lua_setfield (L, Table, "channel");

            lua_createtable (L, 0, 1);

            lua_pushboolean (L, State ? 1 : 0);
            lua_setfield (L, -2, "active");

            lua_setfield (L, Table, "state");

            if (lua_pcall (L, 2, 0, Handler)) {

               retVal = False;

               lua_pop (L, 1); // pop error message
            }
         }

         lua_pop (L, 1); // pop error handler function
      }

      lua_pop (L, 1); // pop the function table
   }

   LUA_END_VALIDATE (L, 0);
   return retVal;
}


inline InputObserverLua**
input_obs_check (lua_State *L, int index) {

   return (InputObserverLua **)luaL_checkudata (L, index, InputObserverLuaName);
}


static int
input_obs_new (lua_State *L) {

   LUA_START_VALIDATE (L);

   InputObserverLua *result (0);

   LuaExtInput *input (get_input (L));

   if (input) {

      InputObserverLua **ptr =
         (InputObserverLua **)lua_newuserdata (L, sizeof (InputObserverLua *));

      if (ptr) {

         *ptr = new InputObserverLua (*input);

         luaL_getmetatable (L, InputObserverLuaName);
         lua_setmetatable (L, -2);

         result = *ptr;
      }
   }

   LUA_END_VALIDATE (L, 1);

   return result ? 1 : 0;
}


static int
input_obs_is_a (lua_State *L) {

   LUA_START_VALIDATE (L);

   if (lua_is_object (L, 1, InputObserverLuaName)) { lua_pushvalue (L, 1); }
   else { lua_pushnil (L); }

   LUA_END_VALIDATE (L, 1);

   return 1;
}


static const luaL_Reg obsArrayFunc[] = {
   {"new", input_obs_new},
   {"is_a", input_obs_is_a},
   {NULL, NULL},
};


static int
input_obs_init_channels (lua_State *L) {

   LUA_START_VALIDATE (L);

   InputObserverLua **obs = input_obs_check (L, 1);
   Config *config = lua_check_config (L, 2);
   Mask *mask = lua_check_mask (L, 3);
   luaL_checktype (L, 4, LUA_TFUNCTION);
   luaL_checktype (L, 5, LUA_TTABLE);

   if (obs && *obs && config && mask) {

      lua_pushlightuserdata (L, (void *)&InputObserverLuaKey);
      lua_rawget (L, LUA_REGISTRYINDEX);

      const int InputTable (lua_gettop (L));

      lua_createtable (L, 2, 0);
      const int Table (lua_gettop (L));

      lua_pushvalue (L, 4);
      lua_rawseti (L, Table, 1);

      lua_pushvalue (L, 5);
      lua_rawseti (L, Table, 2);

      int index = luaL_ref (L, InputTable);
      lua_pop (L, 1); // pop InputTable

      (*obs)->init_input_channels (*config, *mask, index);
   }

   LUA_END_VALIDATE (L, 0);

   return 0;
}


static int
input_obs_register_channel (lua_State *L) {

   LUA_START_VALIDATE (L);

   InputObserverLua **obs = input_obs_check (L, 1);
   const String ChannelName = lua_tostring (L, 2);
   Mask *mask = lua_check_mask (L, 3);
   luaL_checktype (L, 4, LUA_TFUNCTION);
   luaL_checktype (L, 5, LUA_TTABLE);

   if (obs && *obs && ChannelName && mask) {

      lua_pushlightuserdata (L, (void *)&InputObserverLuaKey);
      lua_rawget (L, LUA_REGISTRYINDEX);

      const int InputTable (lua_gettop (L));

      lua_createtable (L, 2, 0);
      const int Table (lua_gettop (L));

      lua_pushvalue (L, 4);
      lua_rawseti (L, Table, 1);

      lua_pushvalue (L, 5);
      lua_rawseti (L, Table, 2);

      int index = luaL_ref (L, InputTable);
      lua_pop (L, 1); // pop InputTable

      (*obs)->register_channel (ChannelName, *mask, index);
   }

   LUA_END_VALIDATE (L, 0);

   return 0;
}


static int
input_obs_register_default_channel (lua_State *L) {

   LUA_START_VALIDATE (L);

   InputObserverLua **obs = input_obs_check (L, 1);
   Mask *mask = lua_check_mask (L, 2);
   luaL_checktype (L, 3, LUA_TFUNCTION);
   luaL_checktype (L, 4, LUA_TTABLE);

   if (obs && *obs && mask) {

      lua_pushlightuserdata (L, (void *)&InputObserverLuaKey);
      lua_rawget (L, LUA_REGISTRYINDEX);

      const int InputTable (lua_gettop (L));

      lua_createtable (L, 2, 0);
      const int Table (lua_gettop (L));

      lua_pushvalue (L, 3);
      lua_rawseti (L, Table, 1);

      lua_pushvalue (L, 4);
      lua_rawseti (L, Table, 2);

      int index = luaL_ref (L, InputTable);
      lua_pop (L, 1); // pop InputTable

      (*obs)->register_channel (InputChannelDefaultName, *mask, index);
   }

   LUA_END_VALIDATE (L, 0);

   return 0;
}


static int
input_obs_release_channel (lua_State *L) {

   LUA_START_VALIDATE (L);

   InputObserverLua **obs = input_obs_check (L, 1);
   const String ChannelName = luaL_checkstring (L, 2);
   if (obs && *obs && ChannelName) { (*obs)->release_channel (ChannelName); }

   LUA_END_VALIDATE (L, 0);

   return 0;
}


static int
input_obs_release_default_channel (lua_State *L) {

   LUA_START_VALIDATE (L);

   InputObserverLua **obs = input_obs_check (L, 1);
   if (obs && *obs) { (*obs)->release_channel (InputChannelDefaultName); }

   LUA_END_VALIDATE (L, 0);

   return 0;
}


static int
input_obs_release_all (lua_State *L) {

   LUA_START_VALIDATE (L);

   InputObserverLua **obs = input_obs_check (L, 1);
   if (obs && *obs) { (*obs)->release_all (); }

   LUA_END_VALIDATE (L, 0);

   return 0;
}


static int
input_obs_delete (lua_State *L) {

   LUA_START_VALIDATE (L);

   InputObserverLua **obs = input_obs_check (L, 1);
   if (obs && *obs) { delete (*obs); *obs = 0; }

   LUA_END_VALIDATE (L, 0);

   return 0;
}


static const luaL_Reg obsArrayMembers [] = {
   {"init_channels", input_obs_init_channels},
   {"register_channel", input_obs_register_channel},
   {"register_default_channel", input_obs_register_default_channel},
   {"release_channel", input_obs_release_channel},
   {"release_default_channel", input_obs_release_default_channel},
   {"release_all", input_obs_release_all},
   {"__gc", input_obs_delete},
   {NULL, NULL},
};

};


//! \cond
dmz::InputObserverLua::InputObserverLua (LuaExtInput &input) :
      _handle (input.get_plugin_name (), input.get_plugin_runtime_context ()),
      _input (input),
      _index (0) {

   _input.store_observer (this);
}


dmz::InputObserverLua::~InputObserverLua () {

   _input.release_observer_all (_handle.get_runtime_handle ());
   _input.remove_observer (_handle.get_runtime_handle ());
}


inline Handle
dmz::InputObserverLua::get_handle () const { return _handle.get_runtime_handle (); }


inline int
dmz::InputObserverLua::get_index () const { return _index; }


Boolean
dmz::InputObserverLua::init_input_channels (
      const Config &Data,
      const Mask &Event,
      const int Index) {

   Boolean retVal (False);

   if (!_index) {

      _index = Index;

      Config channels;

      if (Data.lookup_all_config ("input.channel", channels)) {

         ConfigIterator it;

         Config cd;

         Boolean found (channels.get_first_config (it, cd));

         while (found) {

            const String ChannelName (config_to_string ("name", cd));

            if (ChannelName) {

               _input.register_observer (ChannelName, Event, this);
            }

            found = channels.get_next_config (it, cd);
         }
      }
      else {

         _input.register_observer (InputChannelDefaultName, Event, this);
      }

      retVal = True;
   }

   return retVal;
}


Boolean
dmz::InputObserverLua::register_channel (
      const String &ChannelName,
      const Mask &Event,
      const int Index) {

   Boolean retVal (False);

   if (!_index) {

      _index = Index;

      _input.register_observer (ChannelName, Event, this);

      retVal = True;
   }

   return retVal;
}


void
dmz::InputObserverLua::release_channel (const String &ChannelName) {

   _input.release_observer (ChannelName, _handle.get_runtime_handle ());
}


void
dmz::InputObserverLua::release_all () {

   _input.release_observer_all (_handle.get_runtime_handle ());
}


dmz::LuaExtInput::LuaExtInput (const PluginInfo &Info, Config &local) :
      Plugin (Info),
      LuaExt (Info),
      InputObserverUtil (Info, local),
      L (0),
      _input (0),
      _log (Info),
      _defs (Info, &_log),
      _observerTable (),
      _channelTable () {

   _init (local);
}


dmz::LuaExtInput::~LuaExtInput () {

   _observerTable.clear ();
   _channelTable.empty ();
}


dmz::Boolean
dmz::LuaExtInput::store_observer (InputObserverLua *obs) {

   Boolean retVal (False);

   if (obs) {

      if (_observerTable.store (obs->get_handle (), obs)) {

         retVal = True;
      }
   }

   return retVal;
}


dmz::Boolean
dmz::LuaExtInput::remove_observer (const Handle ObsHandle) {

   Boolean retVal (False);

   release_observer_all (ObsHandle);

   if (_observerTable.remove (ObsHandle)) {

      retVal = True;
   }

   return retVal;
}


dmz::Handle
dmz::LuaExtInput::register_observer (
      const String &ChannelName,
      const Mask &EventMask,
      InputObserverLua *obs) {

   Handle retVal (0);

   if (obs) {

      retVal = _defs.create_named_handle (ChannelName);

      ChannelStruct *cs (_channelTable.lookup (retVal));

      if (!cs) {

         cs = new ChannelStruct (retVal);

         _channelTable.store (cs->Channel, cs);
      }

      activate_input_channel (ChannelName, EventMask);

      if (EventMask.contains (InputEventChannelStateMask)) {

         if (cs->stateTable.store (obs->get_handle (), obs)) {

            if (cs->active) {

               lua_pushlightuserdata (L, (void *)&InputObserverLuaKey);
               lua_rawget (L, LUA_REGISTRYINDEX);
               const int InputTable (lua_gettop (L));

               if (!obs_process_state (L, InputTable, obs, cs->Channel, cs->active)) {

                  cs->stateTable.remove (obs->get_handle ());
               }

               lua_pop (L, 1); // pop the input table
            }
         }
      }

      if (EventMask.contains (InputEventAxisMask)) {

         cs->axisTable.store (obs->get_handle (), obs);
      }

      if (EventMask.contains (InputEventButtonMask)) {

         cs->buttonTable.store (obs->get_handle (), obs);
      }

      if (EventMask.contains (InputEventSwitchMask)) {

         cs->switchTable.store (obs->get_handle (), obs);
      }

      if (EventMask.contains (InputEventKeyMask)) {

         cs->keyTable.store (obs->get_handle (), obs);
      }

      if (EventMask.contains (InputEventMouseMask)) {

         cs->mouseTable.store (obs->get_handle (), obs);
      }

      if (EventMask.contains (InputEventDataMask)) {

         cs->dataTable.store (obs->get_handle (), obs);
      }
   }

   return  retVal;
}


void
dmz::LuaExtInput::release_observer (const String &ChannelName, const Handle ObsHandle) {

   const Handle Channel (_defs.lookup_named_handle (ChannelName));
   if (Channel) { release_observer (Channel, ObsHandle); }
}


void
dmz::LuaExtInput::release_observer (const Handle Channel, const Handle ObsHandle) {

   ChannelStruct *cs (_channelTable.lookup (Channel));
   if (cs) {

      cs->remove (ObsHandle);
      _deactivate_check (*cs);
   }
}

void
dmz::LuaExtInput::release_observer_all (const Handle ObsHandle) {

   HashTableHandleIterator it;
   ChannelStruct *cs (_channelTable.get_first (it));

   while (cs) {

      cs->remove (ObsHandle);
      _deactivate_check (*cs);
      cs = _channelTable.get_next (it);
   }
}


// Plugin Interface
void
dmz::LuaExtInput::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      InputModule *channels (InputModule::cast (PluginPtr));

      if (PluginPtr && channels) {

         store_input_module_channels (PluginPtr->get_plugin_name (), *channels);
      }
   }
   else if (Mode == PluginDiscoverRemove) {

      InputModule *channels (InputModule::cast (PluginPtr));

      if (PluginPtr && channels) {

         remove_input_module_channels (PluginPtr->get_plugin_name (), *channels);
      }
   }
}


// LuaExt Interface
void
dmz::LuaExtInput::store_lua_module (LuaModule &module) {

}


void
dmz::LuaExtInput::open_lua_extension (lua_State *LuaState) {

   L = LuaState;

   LUA_START_VALIDATE (L);

   lua_create_dmz_namespace (L, "input");
   luaL_register (L, NULL, arrayFunc);

   const Mask InputChannelState (InputEventChannelStateMask);
   lua_create_mask (L, &InputChannelState);
   lua_setfield (L, -2, "ChannelState");

   const Mask InputAxis (InputEventAxisMask);
   lua_create_mask (L, &InputAxis);
   lua_setfield (L, -2, "Axis");

   const Mask InputButton (InputEventButtonMask);
   lua_create_mask (L, &InputButton);
   lua_setfield (L, -2, "Button");

   const Mask InputSwitch (InputEventSwitchMask);
   lua_create_mask (L, &InputSwitch);
   lua_setfield (L, -2, "Switch");

   const Mask InputKey (InputEventKeyMask);
   lua_create_mask (L, &InputKey);
   lua_setfield (L, -2, "Key");

   const Mask InputMouse (InputEventMouseMask);
   lua_create_mask (L, &InputMouse);
   lua_setfield (L, -2, "Mouse");

   const Mask InputData (InputEventDataMask);
   lua_create_mask (L, &InputData);
   lua_setfield (L, -2, "Data");

   const Mask InputAll (InputEventAllMask);
   lua_create_mask (L, &InputAll);
   lua_setfield (L, -2, "All");

   lua_make_readonly (L, -1);
   lua_pop (L, 1); // pop dmz.input table

   lua_pushlightuserdata (L, (void *)&InputLuaKey);
   _input = (LuaExtInput **)lua_newuserdata (L, sizeof (LuaExtInput *));

   if (_input) {

      *_input = this;
      lua_rawset (L, LUA_REGISTRYINDEX);
   }
   else { lua_pop (L, 1); } // pop light data

   lua_pushlightuserdata (L, (void *)&InputObserverLuaKey);
   lua_newtable (L);
   lua_rawset (L, LUA_REGISTRYINDEX);

   luaL_newmetatable (L, InputObserverLuaName);
   luaL_register (L, NULL, obsArrayMembers);
   lua_pushvalue (L, -1);
   lua_setfield (L, -2, "__index");
   lua_create_dmz_namespace (L, "input_observer");
   luaL_register (L, NULL, obsArrayFunc);

   lua_make_readonly (L, -1);
   lua_pop (L, 2); // pop meta table and dmz.input_observer table

   LUA_END_VALIDATE (L, 0);
}


void
dmz::LuaExtInput::close_lua_extension (lua_State *LuaState) {

   if (L == LuaState) {

      LUA_START_VALIDATE (L);

      if (_input) { *_input = 0; }

      lua_pushlightuserdata (L, (void *)&InputLuaKey);
      lua_pushnil (L);
      lua_rawset (L, LUA_REGISTRYINDEX);

      lua_pushlightuserdata (L, (void *)&InputObserverLuaKey);
      lua_pushnil (L);
      lua_rawset (L, LUA_REGISTRYINDEX);

      LUA_END_VALIDATE (L, 0);

      L = 0;
   }
}


void
dmz::LuaExtInput::remove_lua_module (LuaModule &module) {

}


// Input Observer Interface
void
dmz::LuaExtInput::update_channel_state (const UInt32 Channel, const Boolean State) {

   ChannelStruct *cs (_channelTable.lookup (Channel));

   if (cs && L) {

      cs->active = State;

      LUA_START_VALIDATE (L);

      lua_pushlightuserdata (L, (void *)&InputObserverLuaKey);
      lua_rawget (L, LUA_REGISTRYINDEX);
      const int InputTable (lua_gettop (L));

      HashTableHandleIterator it;
      InputObserverLua *obs (cs->stateTable.get_first (it));

      while (obs) {

         if (!obs_process_state (L, InputTable, obs, cs->Channel, State)) {

            cs->stateTable.remove (obs->get_handle ());
         }

         obs = cs->stateTable.get_next (it);
      }

      lua_pop (L, 1); // pop the input table

      LUA_END_VALIDATE (L, 0);
   }
}


void
dmz::LuaExtInput::receive_axis_event (
      const Handle Channel,
      const InputEventAxis &Value) {

   ChannelStruct *cs (_channelTable.lookup (Channel));

   if (cs && L) {

      LUA_START_VALIDATE (L);

      lua_pushlightuserdata (L, (void *)&InputObserverLuaKey);
      lua_rawget (L, LUA_REGISTRYINDEX);
      const int InputTable (lua_gettop (L));

      HashTableHandleIterator it;
      InputObserverLua *obs (cs->axisTable.get_first (it));

      while (obs && obs->get_index ()) {

         lua_rawgeti (L, InputTable, obs->get_index ());

         if (lua_istable (L, -1)) {

            const int CallBackTable = lua_gettop (L);

            lua_pushcfunction (L, lua_error_handler);
            const int Handler (lua_gettop (L));

            lua_rawgeti (L, CallBackTable, 1);

            if (lua_isfunction (L, -1)) {

               lua_rawgeti (L, CallBackTable, 2);

               lua_createtable (L, 0, 2);
               const int Table (lua_gettop (L));

               lua_create_handle (L, Channel);
               lua_setfield (L, Table, "channel");

               axis_event_to_table (L, Value);
               lua_setfield (L, Table, "axis");

               if (lua_pcall (L, 2, 0, Handler)) {

                  cs->axisTable.remove (obs->get_handle ());

                  lua_pop (L, 1); // pop error message
               }
            }

            lua_pop (L, 1); // pop error handler function
         }

         lua_pop (L, 1); // pop the function table

         obs = cs->axisTable.get_next (it);
      }

      lua_pop (L, 1); // pop the input table

      LUA_END_VALIDATE (L, 0);
   }
}


void
dmz::LuaExtInput::receive_button_event (
      const Handle Channel,
      const InputEventButton &Value) {

   ChannelStruct *cs (_channelTable.lookup (Channel));

   if (cs && L) {

      LUA_START_VALIDATE (L);

      lua_pushlightuserdata (L, (void *)&InputObserverLuaKey);
      lua_rawget (L, LUA_REGISTRYINDEX);
      const int InputTable (lua_gettop (L));

      HashTableHandleIterator it;
      InputObserverLua *obs (cs->buttonTable.get_first (it));

      while (obs && obs->get_index ()) {

         lua_rawgeti (L, InputTable, obs->get_index ());

         if (lua_istable (L, -1)) {

            const int CallBackTable = lua_gettop (L);

            lua_pushcfunction (L, lua_error_handler);
            const int Handler (lua_gettop (L));

            lua_rawgeti (L, CallBackTable, 1);

            if (lua_isfunction (L, -1)) {

               lua_rawgeti (L, CallBackTable, 2);

               lua_createtable (L, 0, 2);
               const int Table (lua_gettop (L));

               lua_create_handle (L, Channel);
               lua_setfield (L, Table, "channel");

               button_event_to_table (L, Value);
               lua_setfield (L, Table, "button");

               if (lua_pcall (L, 2, 0, Handler)) {

                  cs->buttonTable.remove (obs->get_handle ());

                  lua_pop (L, 1); // pop error message
               }
            }

            lua_pop (L, 1); // pop error handler function
         }

         lua_pop (L, 1); // pop the function table

         obs = cs->buttonTable.get_next (it);
      }

      lua_pop (L, 1); // pop the input table

      LUA_END_VALIDATE (L, 0);
   }
}


void
dmz::LuaExtInput::receive_switch_event (
      const Handle Channel,
      const InputEventSwitch &Value) {

   ChannelStruct *cs (_channelTable.lookup (Channel));

   if (cs && L) {

      LUA_START_VALIDATE (L);

      lua_pushlightuserdata (L, (void *)&InputObserverLuaKey);
      lua_rawget (L, LUA_REGISTRYINDEX);
      const int InputTable (lua_gettop (L));

      HashTableHandleIterator it;
      InputObserverLua *obs (cs->switchTable.get_first (it));

      while (obs && obs->get_index ()) {

         lua_rawgeti (L, InputTable, obs->get_index ());

         if (lua_istable (L, -1)) {

            const int CallBackTable = lua_gettop (L);

            lua_pushcfunction (L, lua_error_handler);
            const int Handler (lua_gettop (L));

            lua_rawgeti (L, CallBackTable, 1);

            if (lua_isfunction (L, -1)) {

               lua_rawgeti (L, CallBackTable, 2);

               lua_createtable (L, 0, 2);
               const int Table (lua_gettop (L));

               lua_create_handle (L, Channel);
               lua_setfield (L, Table, "channel");

               switch_event_to_table (L, Value);
               lua_setfield (L, Table, "switch");

               if (lua_pcall (L, 2, 0, Handler)) {

                  cs->switchTable.remove (obs->get_handle ());

                  lua_pop (L, 1); // pop error message
               }
            }

            lua_pop (L, 1); // pop error handler function
         }

         lua_pop (L, 1); // pop the function table

         obs = cs->switchTable.get_next (it);
      }

      lua_pop (L, 1); // pop the input table

      LUA_END_VALIDATE (L, 0);
   }
}


void
dmz::LuaExtInput::receive_key_event (
      const Handle Channel,
      const InputEventKey &Value) {

   ChannelStruct *cs (_channelTable.lookup (Channel));

   if (cs && L) {

      LUA_START_VALIDATE (L);

      lua_pushlightuserdata (L, (void *)&InputObserverLuaKey);
      lua_rawget (L, LUA_REGISTRYINDEX);
      const int InputTable (lua_gettop (L));

      HashTableHandleIterator it;
      InputObserverLua *obs (cs->keyTable.get_first (it));

      while (obs && obs->get_index ()) {

         lua_rawgeti (L, InputTable, obs->get_index ());

         if (lua_istable (L, -1)) {

            const int CallBackTable = lua_gettop (L);

            lua_pushcfunction (L, lua_error_handler);
            const int Handler (lua_gettop (L));

            lua_rawgeti (L, CallBackTable, 1);

            if (lua_isfunction (L, -1)) {

               lua_rawgeti (L, CallBackTable, 2);

               lua_createtable (L, 0, 2);
               const int Table (lua_gettop (L));

               lua_create_handle (L, Channel);
               lua_setfield (L, Table, "channel");

               key_event_to_table (L, Value);
               lua_setfield (L, Table, "key");

               if (lua_pcall (L, 2, 0, Handler)) {

                  cs->keyTable.remove (obs->get_handle ());

                  lua_pop (L, 1); // pop error message
               }
            }

            lua_pop (L, 1); // pop error handler function
         }

         lua_pop (L, 1); // pop the function table

         obs = cs->keyTable.get_next (it);
      }

      lua_pop (L, 1); // pop the input table

      LUA_END_VALIDATE (L, 0);
   }
}


void
dmz::LuaExtInput::receive_mouse_event (
      const Handle Channel,
      const InputEventMouse &Value) {

   ChannelStruct *cs (_channelTable.lookup (Channel));

   if (cs && L) {

      LUA_START_VALIDATE (L);

      lua_pushlightuserdata (L, (void *)&InputObserverLuaKey);
      lua_rawget (L, LUA_REGISTRYINDEX);
      const int InputTable (lua_gettop (L));

      HashTableHandleIterator it;
      InputObserverLua *obs (cs->mouseTable.get_first (it));

      while (obs && obs->get_index ()) {

         lua_rawgeti (L, InputTable, obs->get_index ());

         if (lua_istable (L, -1)) {

            const int CallBackTable = lua_gettop (L);

            lua_pushcfunction (L, lua_error_handler);
            const int Handler (lua_gettop (L));

            lua_rawgeti (L, CallBackTable, 1);

            if (lua_isfunction (L, -1)) {

               lua_rawgeti (L, CallBackTable, 2);

               lua_createtable (L, 0, 2);
               const int Table (lua_gettop (L));

               lua_create_handle (L, Channel);
               lua_setfield (L, Table, "channel");

               mouse_event_to_table (L, Value);
               lua_setfield (L, Table, "mouse");

               if (lua_pcall (L, 2, 0, Handler)) {

                  cs->mouseTable.remove (obs->get_handle ());

                  lua_pop (L, 1); // pop error message
               }
            }

            lua_pop (L, 1); // pop error handler function
         }

         lua_pop (L, 1); // pop the function table

         obs = cs->mouseTable.get_next (it);
      }

      lua_pop (L, 1); // pop the input table

      LUA_END_VALIDATE (L, 0);
   }
}


void
dmz::LuaExtInput::receive_data_event (
      const Handle Channel,
      const Handle Source,
      const Data &Value) {

   ChannelStruct *cs (_channelTable.lookup (Channel));

   if (cs && L) {

      LUA_START_VALIDATE (L);

      lua_pushlightuserdata (L, (void *)&InputObserverLuaKey);
      lua_rawget (L, LUA_REGISTRYINDEX);
      const int InputTable (lua_gettop (L));

      HashTableHandleIterator it;
      InputObserverLua *obs (cs->dataTable.get_first (it));

      while (obs && obs->get_index ()) {

         lua_rawgeti (L, InputTable, obs->get_index ());

         if (lua_istable (L, -1)) {

            const int CallBackTable = lua_gettop (L);

            lua_pushcfunction (L, lua_error_handler);
            const int Handler (lua_gettop (L));

            lua_rawgeti (L, CallBackTable, 1);

            if (lua_isfunction (L, -1)) {

               lua_rawgeti (L, CallBackTable, 2);

               lua_createtable (L, 0, 2);
               const int Table (lua_gettop (L));

               lua_create_handle (L, Channel);
               lua_setfield (L, Table, "channel");

               lua_create_handle (L, Source);
               lua_setfield (L, Table, "source");

               lua_create_data (L, &Value);
               lua_setfield (L, Table, "data");

               if (lua_pcall (L, 2, 0, Handler)) {

                  cs->dataTable.remove (obs->get_handle ());

                  lua_pop (L, 1); // pop error message
               }
            }

            lua_pop (L, 1); // pop error handler function
         }

         lua_pop (L, 1); // pop the function table

         obs = cs->dataTable.get_next (it);
      }

      lua_pop (L, 1); // pop the input table

      LUA_END_VALIDATE (L, 0);
   }
}


void
dmz::LuaExtInput::_deactivate_check (ChannelStruct &cs) {

   Mask eventMask;

   if (!cs.stateTable.get_count ()) { eventMask |= InputEventChannelStateMask; }
   if (!cs.axisTable.get_count ()) { eventMask |= InputEventAxisMask; }
   if (!cs.buttonTable.get_count ()) { eventMask |= InputEventButtonMask; }
   if (!cs.switchTable.get_count ()) { eventMask |= InputEventSwitchMask; }
   if (!cs.keyTable.get_count ()) { eventMask |= InputEventKeyMask; }
   if (!cs.mouseTable.get_count ()) { eventMask |= InputEventMouseMask; }
   if (!cs.dataTable.get_count ()) { eventMask |= InputEventDataMask; }

   if (eventMask.is_set ()) { deactivate_input_channel (cs.Channel, eventMask); }
}


void
dmz::LuaExtInput::_init (Config &local) {

}
//! \endcond


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzLuaExtInput (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::LuaExtInput (Info, local);
}

};
