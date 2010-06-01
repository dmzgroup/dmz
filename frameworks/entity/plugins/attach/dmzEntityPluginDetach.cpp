#include <dmzObjectAttributeMasks.h>
#include <dmzEntityConsts.h>
#include "dmzEntityPluginDetach.h"
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>

/*!

\class dmz::EntityPluginDetach
\ingroup Entity
\brief Detaches entity.

*/

//! \cond
dmz::EntityPluginDetach::EntityPluginDetach (const PluginInfo &Info, Config &local) :
      Plugin (Info),
      MessageObserver (Info),
      ObjectObserverUtil (Info, local),
      _log (Info),
      _convert (Info),
      _attached (0) {

   _init (local);
}


dmz::EntityPluginDetach::~EntityPluginDetach () {

}


// Plugin Interface
void
dmz::EntityPluginDetach::update_plugin_state (
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
dmz::EntityPluginDetach::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

   }
   else if (Mode == PluginDiscoverRemove) {

   }
}


// Message Observer Interface
void
dmz::EntityPluginDetach::receive_message (
      const Message &Type,
      const UInt32 MessageSendHandle,
      const Handle TargetObserverHandle,
      const Data *InData,
      Data *outData) {

   _attached = _convert.to_handle (InData);
}


// Object Observer Interface
void
dmz::EntityPluginDetach::destroy_object (
      const UUID &Identity,
      const Handle ObjectHandle) {

   if (ObjectHandle == _attached) {

      Data out = _convert.to_data (0);
      _attachMsg.send (&out);
   }
}


void
dmz::EntityPluginDetach::_init (Config &local) {

   _attachMsg = config_create_message (
      "attach-message.name",
      local,
      EntityMessageAttachName,
      get_plugin_runtime_context ());

   subscribe_to_message (_attachMsg);

   activate_default_object_attribute (ObjectDestroyMask);
}
//! \endcond


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzEntityPluginDetach (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::EntityPluginDetach (Info, local);
}

};
