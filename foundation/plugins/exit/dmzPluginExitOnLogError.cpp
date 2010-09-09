#include "dmzPluginExitOnLogError.h"
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>

dmz::PluginExitOnLogError::PluginExitOnLogError (const PluginInfo &Info, Config &local) :
      Plugin (Info),
      LogObserver (Info),
      _log (Info),
      _exit (Info),
      _called (False) {

   _init (local);
}


dmz::PluginExitOnLogError::~PluginExitOnLogError () {

}


// Plugin Interface
void
dmz::PluginExitOnLogError::update_plugin_state (
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
dmz::PluginExitOnLogError::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

   }
   else if (Mode == PluginDiscoverRemove) {

   }
}


// LogObserver Interface
void
dmz::PluginExitOnLogError::store_log_message (
      const String &LogName,
      const LogLevelEnum Level,
      const String &Message) {

   if (!_called && (Level == LogLevelError)) {

      _called = True;
      _exit.request_exit (ExitStatusError, Message);
   }
}


// PluginExitOnLogError Interface
void
dmz::PluginExitOnLogError::_init (Config &local) {

}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzPluginExitOnLogError (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::PluginExitOnLogError (Info, local);
}

};
