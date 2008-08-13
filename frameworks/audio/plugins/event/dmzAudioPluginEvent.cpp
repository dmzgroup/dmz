#include "dmzAudioPluginEvent.h"
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>

dmz::AudioPluginEvent::AudioPluginEvent (const PluginInfo &Info, Config &local) :
      Plugin (Info),
      EventObserverUtil (Info, local),
      _log (Info) {

   _init (local);
}


dmz::AudioPluginEvent::~AudioPluginEvent () {

}


// Event Observer Interface
void
dmz::AudioPluginEvent::close_event (
      const Handle EventHandle,
      const EventType &Type,
      const EventLocalityEnum Locality) {

}


void
dmz::AudioPluginEvent::_init (Config &local) {

}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzAudioPluginEvent (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::AudioPluginEvent (Info, local);
}

};
