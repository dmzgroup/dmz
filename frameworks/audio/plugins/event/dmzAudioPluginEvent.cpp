#include <dmzAudioModule.h>
#include "dmzAudioPluginEvent.h"
#include <dmzRuntimeEventType.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>

/*!

\class dmz::AudioPluginEvent
\ingroup Audio
\brief Work in Progress.

*/

//! \cond
dmz::AudioPluginEvent::AudioPluginEvent (const PluginInfo &Info, Config &local) :
      Plugin (Info),
      EventObserverUtil (Info, local),
      _log (Info),
      _rc (Info),
      _audioMod (0) {

   _init (local);
}


dmz::AudioPluginEvent::~AudioPluginEvent () {

   _clear ();
}


void
dmz::AudioPluginEvent::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      if (!_audioMod) { _audioMod = AudioModule::cast (PluginPtr); }
   }
   else if (Mode == PluginDiscoverAdd) {

      if (_audioMod && (_audioMod == AudioModule::cast (PluginPtr))) {

         _clear ();
         _audioMod = 0;
      }
   }
}



// Event Observer Interface
void
dmz::AudioPluginEvent::close_event (
      const Handle EventHandle,
      const EventType &Type,
      const EventLocalityEnum Locality) {

}


dmz::AudioPluginEvent::EventStruct *
dmz::AudioPluginEvent::_create_event_struct (const EventType &Type) {

   EventStruct *result (_eventTable.lookup (Type.get_handle ()));

   if (!result && _audioMod) {

   }

   return result;
}


void
dmz::AudioPluginEvent::_clear () {

   if (_audioMod) {

      HashTableStringIterator it;
      Handle *sound = _soundTable.get_first (it);
   
      while (sound) {

         _audioMod->destroy_sound (*sound);
         sound = _soundTable.get_next (it);
      }
   }

   _soundTable.empty ();

   _eventTable.empty ();
}


void
dmz::AudioPluginEvent::_init (Config &local) {

}
//! \endcond


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzAudioPluginEvent (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::AudioPluginEvent (Info, local);
}

};
