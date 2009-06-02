#include <dmzInputEventKey.h>
#include <dmzInputEventMasks.h>
#include "dmzLuaPluginReset.h"
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>

/*!

\class dmz::LuaPluginReset
\ingroup Lua
\brief Resets the Lua runtime.
\details Resets the Lua runtime when a specified key is pressed. The default key is 'R'.
\code
<dmz>
<dmzLuaPluginReset>
   <key value="Key Value"/>
</dmzLuaPluginReset>
</dmz>
\endcode

*/


//! \cond
dmz::LuaPluginReset::LuaPluginReset (const PluginInfo &Info, Config &local) :
      Plugin (Info),
      InputObserverUtil (Info, local),
      _key (0),
      _luaMod (0),
      _log (Info) {

   _init (local);
}


dmz::LuaPluginReset::~LuaPluginReset () {

}


// Plugin Interface
void
dmz::LuaPluginReset::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      if (!_luaMod) { _luaMod = LuaModule::cast (PluginPtr); }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_luaMod && (_luaMod == LuaModule::cast (PluginPtr))) { _luaMod = 0; }
   }
}


// Input Observer Interface
void
dmz::LuaPluginReset::receive_button_event (
      const Handle Channel,
      const InputEventButton &Value) {

}


void
dmz::LuaPluginReset::receive_key_event (
      const Handle Channel,
      const InputEventKey &Value) {

   if ((Value.get_key () == _key) && Value.get_key_state ()) {

      _log.info << "Resetting Lua runtime." << endl;
      if (_luaMod) { _luaMod->reset_lua (); }
   }
}


void
dmz::LuaPluginReset::_init (Config &local) {

   _key = config_to_key_value ("key.value", local,  UInt32 ('R'), &_log);

   init_input_channels (
      local,
      InputEventButtonMask | InputEventKeyMask,
      &_log);
}
//! \endcond


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzLuaPluginReset (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::LuaPluginReset (Info, local);
}

};
