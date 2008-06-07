#include "dmzPluginUndoDump.h"
#include <dmzRuntimeConfigRead.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>

/*!

\class dmz::PluginUndoDump
\ingroup Foundation
\brief Dumps the undo and redo stack to the info log.

*/

//! \cond
dmz::PluginUndoDump::PluginUndoDump (const PluginInfo &Info, Config &local) :
      Plugin (Info),
      MessageObserver (Info),
      _defs (Info),
      _undo (Info),
      _log (Info) {

   _init (local);
}


dmz::PluginUndoDump::~PluginUndoDump () {

}


// Message Observer Interface
void
dmz::PluginUndoDump::receive_message (
      const Message &Msg,
      const UInt32 MessageSendHandle,
      const Handle TargetObserverHandle,
      const Data *InData,
      Data *outData) {

   _log.info << "----- Undo Stack -----" << endl << endl;
   _undo.dump (UndoTypeUndo, *this);
   _log.info << "----- Redo Stack -----" << endl;
   _undo.dump (UndoTypeRedo, *this);
   _log.info << "----------------------" << endl;
}


// Undo Dump Interface
void
dmz::PluginUndoDump::start_record (const Handle RecordHandle, const String &Name) {

   _log.info << "***** Start: " << Name << "[" << RecordHandle << "] *****" << endl
      << endl;
}


void
dmz::PluginUndoDump::store_action (
      const Message &Type,
      const Handle Target,
      const Data *Value) {

   _log.info << "Message: " << Type.get_name () << endl;
   _log.info << " Target: "
      << (Target ? _defs.lookup_named_handle_name (Target) : "<GLOBAL>") << endl;
   if (Value) {

      Data outData (get_plugin_runtime_context ());
      outData = *Value;
      _log.info << "   Data: " << outData << endl;
   }
}


void
dmz::PluginUndoDump::_init (Config &local) {

   RuntimeContext *context (get_plugin_runtime_context ());

   Message message;
  _defs.create_message_type ("DumpUndoStackMessage", message);
   subscribe_to_message (message);
}
//! \endcond


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzPluginUndoDump (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::PluginUndoDump (Info, local);
}

};
