#include "dmzPluginMsgDump.h"
#include <dmzRuntimeData.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>

/*!

\class dmz::PluginMsgDump
\ingroup Foundation
\brief Message debugging plugin.
\details Plugin that dumps all messages and their associated data to the Log.

*/

//! \cond
dmz::PluginMsgDump::PluginMsgDump (const PluginInfo &Info, Config &local) :
      Plugin (Info),
      MessageObserver (Info),
      _log (Info.get_name (), Info.get_context ()) {

   subscribe_to_message (Definitions (Info).get_global_message_type ());
}


// Message Observer Interface
void
dmz::PluginMsgDump::receive_message (
      const Message &Msg,
      const UInt32 MessageSendHandle,
      const UInt32 TargetObserverHandle,
      const Data *InData,
      Data *outData) {

   Definitions defs (get_plugin_runtime_context ());

   _log.debug << "-=-=-=-=-=-=-=-=-=- START MESSAGE -=-=-=-=-=-=-=-=-=-" << endl;
   _log.debug << "Target: "
      << (TargetObserverHandle ?
         defs.lookup_runtime_name (TargetObserverHandle) : "GLOBAL")
      << endl;
   _log.debug << "Message[" << MessageSendHandle << "]: " << Msg.get_name () << endl;
   if (InData) { _log.debug << "Sent data:" << endl << *InData << endl; }
   if (outData) { _log.debug << "Returned data:" << endl << *outData << endl; }
   _log.debug << "-=-=-=-=-=-=-=-=-=-  END MESSAGE  -=-=-=-=-=-=-=-=-=-" << endl;
}
//! \endcond


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzPluginMsgDump (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::PluginMsgDump (Info, local);
}

};
