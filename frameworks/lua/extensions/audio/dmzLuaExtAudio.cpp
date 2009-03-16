#include <dmzAudioModule.h>
#include <dmzAudioSoundAttributes.h>
#include <dmzAudioSoundInit.h>
#include "dmzLuaExtAudio.h"
#include <dmzLuaKernel.h>
#include <dmzLuaKernelValidate.h> 
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>

#include <luacpp.h>

/*!

\class dmz::LuaExtAudio
\ingroup Lua
\brief Provides a Lua API for the Audio Framework.
\details
\htmlonly See the <a href="dmzlua.html#dmz.audio">Lua Audio API</a>.
\endhtmlonly

*/

using namespace dmz;

namespace {

static char AudioKey = 'a';

static inline AudioModule *
get_audio (lua_State *L) {

   AudioModule *result (0);

  lua_pushlightuserdata (L, (void *)&AudioKey);
  lua_rawget (L, LUA_REGISTRYINDEX);
  AudioModule **audio = (AudioModule **)lua_touserdata (L, -1);
  if (audio) { result = *audio; }
  lua_pop (L, 1); // pop audio module

   return result;
}


static SoundInit
table_to_sound_init (lua_State *L, int index) {

   SoundInit result;

   if (index < 0) { index = lua_gettop (L) + index + 1; }

   if (!lua_isnoneornil (L, index)) {

      lua_getfield (L, index, "looped");

      if (!lua_isnil (L, -1)) {

         result.set (SoundLooped, lua_toboolean (L, -1) != 0);
      }

      lua_pop (L, 1); // pop looped

      lua_getfield (L, index, "relative");

      if (!lua_isnil (L, -1)) {

         result.set (SoundRelative, lua_toboolean (L, -1) != 0);
      }

      lua_pop (L, 1); // pop relative
   }

   return result;
}


static SoundAttributes
table_to_sound_attr (lua_State *L, int index) {

   SoundAttributes result;

   if (index < 0) { index = lua_gettop (L) + index + 1; }

   if (!lua_isnoneornil (L, index)) {

      lua_getfield (L, index, "position");

      if (!lua_isnil (L, -1)) {

         Vector *ptr = lua_check_vector (L, -1);

         if (ptr) { result.set_position (*ptr); }
      }

      lua_pop (L, 1); // pop position

      lua_getfield (L, index, "velocity");

      if (!lua_isnil (L, -1)) {

         Vector *ptr = lua_check_vector (L, -1);

         if (ptr) { result.set_velocity (*ptr); }
      }

      lua_pop (L, 1); // pop velocity

      lua_getfield (L, index, "gain");

      if (!lua_isnil (L, -1)) {

         result.set_gain_scale (luaL_checknumber (L, -1));
      }

      lua_pop (L, 1); // pop gain

      lua_getfield (L, index, "pitch");

      if (!lua_isnil (L, -1)) {

         result.set_pitch_scale (luaL_checknumber (L, -1));
      }

      lua_pop (L, 1); // pop gain
   }

   return result;
}


static int
audio_create_handle (lua_State *L) {

   LUA_START_VALIDATE (L);

   int result (0);

   AudioModule *audio (get_audio (L));
   const String FileName (luaL_checkstring (L, 1));

   if (audio) {

      lua_create_handle (L, audio->create_sound (FileName));
      result = 1;
   }

   LUA_END_VALIDATE (L, result);

   return result;
}


static int
audio_destroy_handle (lua_State *L) {

   LUA_START_VALIDATE (L);

   int result (0);

   AudioModule *audio (get_audio (L));
   const Handle *AudioHandle (lua_check_handle (L, 1));

   if (audio && *AudioHandle) {

      lua_pushboolean (L, audio->destroy_sound (*AudioHandle) ? 1 : 0);
      result = 1;
   }

   LUA_END_VALIDATE (L, result);

   return result;
}


static int
audio_play_sound (lua_State *L) {

   LUA_START_VALIDATE (L);

   int result (0);

   AudioModule *audio (get_audio (L));
   const Handle *AudioHandle (lua_check_handle (L, 1));
   const SoundInit Init = table_to_sound_init (L, 2);
   const SoundAttributes Attr = table_to_sound_attr (L, 3);

   if (audio && *AudioHandle) {

      if (lua_create_handle (L, audio->play_sound (*AudioHandle, Init, Attr))) {

         result = 1;
      }
   }

   LUA_END_VALIDATE (L, result);

   return result;
}


static int
audio_update_sound (lua_State *L) {

   LUA_START_VALIDATE (L);

   int result (0);

   AudioModule *audio (get_audio (L));
   const Handle *AudioHandle (lua_check_handle (L, 1));
   const SoundAttributes Attr = table_to_sound_attr (L, 2);

   if (audio && *AudioHandle) {

      lua_pushboolean (L, audio->update_sound (*AudioHandle, Attr) ? 1 : 0);
      result = 1;
   }

   LUA_END_VALIDATE (L, result);

   return result;
}


static int
audio_stop_sound (lua_State *L) {

   LUA_START_VALIDATE (L);

   int result (0);

   AudioModule *audio (get_audio (L));
   const Handle *AudioHandle (lua_check_handle (L, 1));

   if (audio && *AudioHandle) {

      lua_pushboolean (L, audio->stop_sound (*AudioHandle) ? 1 : 0);
      result = 1;
   }

   LUA_END_VALIDATE (L, result);

   return result;
}

static const luaL_Reg arrayFunc[] = {
   {"create", audio_create_handle},
   {"destroy", audio_destroy_handle},
   {"play", audio_play_sound},
   {"update", audio_update_sound},
   {"stop", audio_stop_sound},
   {NULL, NULL},
};

}


//! \cond
dmz::LuaExtAudio::LuaExtAudio (const PluginInfo &Info, Config &local) :
      Plugin (Info),
      LuaExt (Info),
      _log (Info),
      L (0),
      _audio (0),
      _audioPtr (0) {

   _init (local);
}


dmz::LuaExtAudio::~LuaExtAudio () {

}


// Plugin Interface
void
dmz::LuaExtAudio::update_plugin_state (
      const PluginStateEnum State,
      const UInt32 Level) {

   if (State == PluginStateInit) {

   }
   else if (State == PluginStateStart) {

   }
   else if (State == PluginStateStop) {

   }
   else if (State == PluginStateShutdown) {

   }
}


void
dmz::LuaExtAudio::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      if (!_audio) {

         _audio = AudioModule::cast (PluginPtr);

         if (_audio && _audioPtr && !(*_audioPtr)) { *_audioPtr = _audio; }
      }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_audio && (_audio == AudioModule::cast (PluginPtr))) {

         _audio = 0;

         if (_audioPtr) { *_audioPtr = 0; }
      }
   }
}


// LuaExt Interface
void
dmz::LuaExtAudio::store_lua_module (LuaModule &module) {

}


void
dmz::LuaExtAudio::open_lua_extension (lua_State *L) {

   LUA_START_VALIDATE (L);

   lua_pushlightuserdata (L, (void *)&AudioKey);
   _audioPtr = (AudioModule **)lua_newuserdata (L, sizeof (AudioModule *));

   if (_audioPtr) {

      *_audioPtr = _audio;
      lua_rawset (L, LUA_REGISTRYINDEX);
   }
   else { lua_pop (L, 1); } // pop light data

   lua_create_dmz_namespace (L, "audio");
   luaL_register (L, NULL, arrayFunc);

   lua_make_readonly (L, -1);
   lua_pop (L, 1); // pop dmz.audio table

   LUA_END_VALIDATE (L, 0);
}


void
dmz::LuaExtAudio::close_lua_extension (lua_State *L) {

   if (_audioPtr) { *_audioPtr = 0; _audioPtr = 0; }
   lua_pushlightuserdata (L, (void *)&AudioKey);
   lua_pushnil (L);
   lua_rawset (L, LUA_REGISTRYINDEX);
}

void
dmz::LuaExtAudio::remove_lua_module (LuaModule &module) {

}


void
dmz::LuaExtAudio::_init (Config &local) {

}
//! \endcond


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzLuaExtAudio (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::LuaExtAudio (Info, local);
}

};
