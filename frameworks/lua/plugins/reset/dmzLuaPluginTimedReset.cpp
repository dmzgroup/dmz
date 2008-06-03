#include "dmzLuaPluginTimedReset.h"
#include <dmzLuaModule.h>
#include <dmzRuntimeConfigRead.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>


dmz::LuaPluginTimedReset::LuaPluginTimedReset (const PluginInfo &Info, Config &local) :
      Plugin (Info),
      Sync (
         Info,
         SyncTypeSystemTime,
         SyncModeRepeating,
         config_to_float64 ("time.value", local, 6.0)),
      _luaMod (0),
      _log (Info) {

   _init (local);
}


dmz::LuaPluginTimedReset::~LuaPluginTimedReset () {

}


// Plugin Interface
void
dmz::LuaPluginTimedReset::discover_plugin (const Plugin *PluginPtr) {

   if (!_luaMod) { _luaMod = LuaModule::cast (PluginPtr); }
}


void
dmz::LuaPluginTimedReset::start_plugin () {

}


void
dmz::LuaPluginTimedReset::stop_plugin () {

}


void
dmz::LuaPluginTimedReset::shutdown_plugin () {

}


void
dmz::LuaPluginTimedReset::remove_plugin (const Plugin *PluginPtr) {

   if (_luaMod && (_luaMod == LuaModule::cast (PluginPtr))) { _luaMod = 0; }
}


// Sync Interface
void
dmz::LuaPluginTimedReset::update_sync (const Float64 TimeDelta) {

   _log.error << "Reseting Lua" << endl;
   if (_luaMod) { _luaMod->reset_lua (); }
}


void
dmz::LuaPluginTimedReset::_init (Config &local) {

}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzLuaPluginTimedReset (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::LuaPluginTimedReset (Info, local);
}

};
