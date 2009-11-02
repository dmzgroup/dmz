#include "dmzLuaExtInput.h"
#include "dmzLuaExtInputObserver.h"
#include <dmzLuaKernelValidate.h>
#include <dmzInputEventKey.h>
#include <dmzInputEventMasks.h>
#include <dmzInputModule.h>
#include <dmzRuntimeData.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>

#include <luacpp.h>

/*!

\class dmz::LuaExtInput
\ingroup Lua
\brief Provides a Lua API the Input Framework.
\details
\htmlonly See the <a href="dmzlua.html#dmz.input">Lua Input API</a>.
\endhtmlonly

*/

using namespace dmz;

namespace {

typedef LuaExtInput::InputStruct istruct;

static const char InputKey = 'i';
static const char InputNamespace[] = "input";


static inline InputModule *
get_input_module (lua_State *L) {

  InputModule *result (0);
  lua_pushlightuserdata (L, (void *)&InputKey);
  lua_rawget (L, LUA_REGISTRYINDEX);
  istruct **es = (istruct **)lua_touserdata (L, -1);
  if (es && *es) { result = (*es)->inputMod; }
  lua_pop (L, 1); // pop istruct
  return result;
}


static inline istruct *
get_input_struct (lua_State *L) {

  istruct *result (0);
  lua_pushlightuserdata (L, (void *)&InputKey);
  lua_rawget (L, LUA_REGISTRYINDEX);
  istruct **es = (istruct **)lua_touserdata (L, -1);
  if (es) { result = *es; }
  lua_pop (L, 1); // pop istruct
  return result;
}


static int
input_channel (lua_State *L) {

   int result (0);

   Handle *channelPtr (lua_check_handle (L, 1));
   InputModule *module = get_input_module (L);

   if (channelPtr && module) {

      if (lua_isnone (L, 2) == 0) {

         module->set_channel_state (*channelPtr, lua_toboolean (L, 2) == 1);
      }

      lua_pushboolean (L, module->get_channel_state (*channelPtr) ? 1 : 0);
      result = 1;
   }

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
   const int ButtonNumber (luaL_checkint (L, 1));

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
   const int ButtonNumber (luaL_checkint (L, 1));

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
   {"channel", input_channel},
   {"get_key_value", input_get_key_value},
   {"get_key_string", input_get_key_string},
   {"has_buttons_changed", input_has_buttons_changed},
   {"is_button_changed", input_is_button_changed},
   {"is_button_pressed", input_is_button_pressed},
   {NULL, NULL},
};

};


//! \cond
void
dmz::lua_register_input_observer (lua_State *L, InputObserver &obs) {

   istruct *es (get_input_struct (L));

   if (es && es->ext) { es->ext->register_input_observer (obs); }
}


void
dmz::lua_release_input_observer (lua_State *L, InputObserver &obs) {

   istruct *es = get_input_struct (L);

   if (es && es->ext) { es->ext->release_input_observer (obs); }
}


dmz::LuaExtInput::LuaExtInput (const PluginInfo &Info, Config &local) :
      Plugin (Info),
      LuaExt (Info),
      L (0),
      _inputPtr (0),
      _log (Info) {

   _input.ext = this;
   _init (local);
}


dmz::LuaExtInput::~LuaExtInput () {

   _obsTable.clear ();
}


void
dmz::LuaExtInput::register_input_observer (InputObserver &obs) {

   if (_obsTable.store (obs.get_input_observer_handle (), &obs)) {

      if (_input.inputMod) { obs.store_input_module ("", *(_input.inputMod)); }
   }
}


void
dmz::LuaExtInput::release_input_observer (InputObserver &obs) {

   if (_obsTable.remove (obs.get_input_observer_handle ())) {

      if (_input.inputMod) { obs.remove_input_module ("", *(_input.inputMod)); }
   }
}


// Plugin Interface
void
dmz::LuaExtInput::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      if (!_input.inputMod) {

         _input.inputMod = InputModule::cast (PluginPtr);

         if (_input.inputMod) {

            HashTableHandleIterator it;

            InputObserver *obs (_obsTable.get_first (it));

            while (obs) {

               obs->store_input_module ("", *(_input.inputMod));

               obs = _obsTable.get_next (it);
            }
         }
      }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_input.inputMod && (_input.inputMod == InputModule::cast (PluginPtr))) {

         HashTableHandleIterator it;

         InputObserver *obs (_obsTable.get_first (it));

         while (obs) {

            obs->remove_input_module ("", *(_input.inputMod));

            obs = _obsTable.get_next (it);
         }
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

   lua_init_input_observer (L);

   lua_create_dmz_namespace (L, InputNamespace);
   luaL_register (L, NULL, arrayFunc);

   Definitions defs (get_plugin_runtime_context (), &_log);

   lua_pushlightuserdata (L, (void *)&InputKey);
   _inputPtr = (InputStruct **)lua_newuserdata (L, sizeof (InputStruct *));

   if (_inputPtr) {

      *_inputPtr = &_input;
      lua_rawset (L, LUA_REGISTRYINDEX);
   }
   else { lua_pop (L, 1); } // pop light data

   lua_make_readonly (L, -1); // make input read only
   lua_pop (L, 1); // pop input table

   LUA_END_VALIDATE (L, 0);
}


void
dmz::LuaExtInput::close_lua_extension (lua_State *LuaState) {

   HashTableHandleIterator it;
   InputObserver *obs (0);

   while (_obsTable.get_next (it, obs)) { release_input_observer (*obs); }

   _obsTable.clear ();

   L = 0;
}


void
dmz::LuaExtInput::remove_lua_module (LuaModule &module) {

}


void
dmz::LuaExtInput::_init (Config &local) {

   Definitions defs (get_plugin_runtime_context (), &_log);
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


