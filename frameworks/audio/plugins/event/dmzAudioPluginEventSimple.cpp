#include "dmzAudioPluginEventSimple.h"
#include <dmzAudioModule.h>
#include <dmzAudioSoundAttributes.h>
#include <dmzEventCallbackMasks.h>
#include <dmzEventConsts.h>
#include <dmzEventModule.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzTypesVector.h>

dmz::AudioPluginEventSimple::AudioPluginEventSimple (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      EventObserverUtil (Info, local),
      _log (Info),
      _launchFile ("../../assets/sounds/launch.wav"),
      _audioMod (0),
      _defaultEventHandle (0),
      _launchHandle (0) {

   _init (local);
}


dmz::AudioPluginEventSimple::~AudioPluginEventSimple () {

}


// Plugin Interface
void
dmz::AudioPluginEventSimple::update_plugin_state (
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
dmz::AudioPluginEventSimple::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      if (!_audioMod) {

         _audioMod = AudioModule::cast (PluginPtr);

         if (_audioMod) {

            _launchHandle = _audioMod->create_audio_handle (_launchFile);
         }
      }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_audioMod && (_audioMod == AudioModule::cast (PluginPtr))){

         _launchHandle = 0;
         _audioMod = 0;
      }
   }
}


// Event Observer Interface
void
dmz::AudioPluginEventSimple::end_event (
      const Handle EventHandle,
      const EventType &Type,
      const EventLocalityEnum Locality) {

   if (Type.is_of_type (_launchType)) {

      EventModule *eventMod (get_event_module ());

      if (_launchHandle && _audioMod && eventMod) {

         Vector pos;

         if (eventMod->lookup_position (EventHandle, _defaultEventHandle, pos)) {

            SoundAttributes sa;
            sa.set_position (pos);
            sa.set_loop (False);
            _audioMod->play_sound (_launchHandle, sa);
         }
      }
   }
}


void
dmz::AudioPluginEventSimple::_init (Config &local) {

   Definitions defs (get_plugin_runtime_context (), &_log);

   _defaultEventHandle = defs.create_named_handle (EventAttributeDefaultName);

   _launchType = activate_event_callback (EventLaunchName, EventEndMask);
   //activate_event_callback (EventDetonationName, EventEndMask);
   //activate_event_callback (EventCollisionName, EventEndMask);
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzAudioPluginEventSimple (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::AudioPluginEventSimple (Info, local);
}

};
