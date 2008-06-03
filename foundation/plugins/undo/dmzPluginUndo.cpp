#include "dmzPluginUndo.h"
#include <dmzRuntimeConfigRead.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>

dmz::PluginUndo::PluginUndo (const PluginInfo &Info, Config &local) :
      Plugin (Info),
      MessageObserver (Info),
      _undo (Info),
      _log (Info) {

   _init (local);
}


dmz::PluginUndo::~PluginUndo () {

}


// Plugin Interface
void
dmz::PluginUndo::discover_plugin (const Plugin *PluginPtr) {

}


void
dmz::PluginUndo::start_plugin () {

}


void
dmz::PluginUndo::stop_plugin () {

}


void
dmz::PluginUndo::shutdown_plugin () {

}


void
dmz::PluginUndo::remove_plugin (const Plugin *PluginPtr) {

}


// Message Observer Interface
void
dmz::PluginUndo::receive_message (
      const Message &Msg,
      const UInt32 MessageSendHandle,
      const Handle TargetObserverHandle,
      const Data *InData,
      Data *outData) {

_log.error << "Got message: " << Msg.get_name () << endl;

   if (Msg == _undoMessage) { _undo.do_next (UndoTypeUndo); }
   else if (Msg == _redoMessage) { _undo.do_next (UndoTypeRedo); }
}


void
dmz::PluginUndo::_init (Config &local) {

   RuntimeContext *context (get_plugin_runtime_context ());

   _undoMessage = config_create_message_type (
      "undo.name",
      local,
      "Plugin_Undo_Message",
      context);

   _redoMessage = config_create_message_type (
      "redo.name",
      local,
      "Plugin_Redo_Message",
      context);

   subscribe_to_message (_undoMessage);
   subscribe_to_message (_redoMessage);
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzPluginUndo (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::PluginUndo (Info, local);
}

};
