#include <dmzRuntimeConfigToNamedHandle.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include "dmzPluginMessageToUniqueFileName.h"
#include <dmzSystemFile.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>

dmz::PluginMessageToUniqueFileName::PluginMessageToUniqueFileName (
      const PluginInfo &Info, Config &local) :
      Plugin (Info),
      MessageObserver (Info),
      _log (Info),
      _convert (Info),
      _target (0) {

   _init (local);
}


dmz::PluginMessageToUniqueFileName::~PluginMessageToUniqueFileName () {

}


// Plugin Interface
void
dmz::PluginMessageToUniqueFileName::update_plugin_state (
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
dmz::PluginMessageToUniqueFileName::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

   }
   else if (Mode == PluginDiscoverRemove) {

   }
}


// Message Observer Interface
void
dmz::PluginMessageToUniqueFileName::receive_message (
      const Message &Type,
      const UInt32 MessageSendHandle,
      const Handle TargetObserverHandle,
      const Data *InData,
      Data *outData) {

   if (Type == _inMsg) {

      Int32 count = 0;
      Boolean done = False;
      String file;

      while (!done) {

         String testFile (_filePath);
         testFile << _fileRoot << "-" << String::number (count) << _fileExt;

         if (!is_valid_path (testFile)) { done = True; file = testFile; }
         else { count++; }
      }

      Data out = _convert.to_data (file);
      _outMsg.send (_target, &out);
   }
}


void
dmz::PluginMessageToUniqueFileName::_init (Config &local) {

   RuntimeContext *context = get_plugin_runtime_context ();

   _inMsg = config_create_message ("in-message.name", local, String (), context);
   subscribe_to_message (_inMsg);

   _outMsg = config_create_message ("out-message.name", local, String (), context);

   _target = config_to_named_handle ("target.name", local, String (), context);

   _fileRoot = config_to_string ("file.root", local, "DMZ File");
   _fileExt = config_to_string ("file.ext", local);

   _filePath = get_home_directory ();

   String desktop = _filePath + "Desktop/";
   if (is_valid_path (desktop)) { _filePath = desktop; }
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzPluginMessageToUniqueFileName (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::PluginMessageToUniqueFileName (Info, local);
}

};
