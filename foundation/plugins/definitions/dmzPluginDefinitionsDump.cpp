#include "dmzPluginDefinitionsDump.h"
#include <dmzRuntimeEventType.h>
#include <dmzRuntimeObjectType.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>

dmz::PluginDefinitionsDump::PluginDefinitionsDump (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      DefinitionsObserver (Info),
      _log (Info) {

   _init (local);
}


// Definitions Observer Interface
void
dmz::PluginDefinitionsDump::define_named_handle (
      const Handle TheHandle,
      const String &Name) {

   _log.warn << "Defined Named Handle: " << Name << "[" << TheHandle << "]" << endl;
}


void
dmz::PluginDefinitionsDump::define_state (const Mask &TheState, const String &Name) {

   _log.warn << "Defined State: " << Name << " - " << TheState << endl;
}


void
dmz::PluginDefinitionsDump::define_object_type (const ObjectType &Type) {

   _log.warn << "Defined ObjectType: " << Type.get_name () << endl;
}


void
dmz::PluginDefinitionsDump::define_event_type (const EventType &Type) {

   _log.warn << "Defined EventType: " << Type.get_name () << endl;
}


void
dmz::PluginDefinitionsDump::_init (Config &local) {

   set_definitions_observer_callback_mask (RuntimeAllMask);
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzPluginDefinitionsDump (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::PluginDefinitionsDump (Info, local);
}

};
