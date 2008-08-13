#include "dmzAudioPluginEventSimple.h"
#include <dmzAudioModule.h>
#include <dmzAudioSoundAttributes.h>
#include <dmzEventCallbackMasks.h>
#include <dmzEventConsts.h>
#include <dmzEventModule.h>
#include <dmzRuntimeConfigToBase.h>
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
      _audioMod (0),
      _defaultEventHandle (0) {

   _init (local);
}


dmz::AudioPluginEventSimple::~AudioPluginEventSimple () {

   _eventTable.empty ();
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

            HashTableHandleIterator it;

            EventStruct *es (_eventTable.get_first (it));

            while (es) {

               es->sound = _audioMod->create_audio_handle (es->File);

               if (!es->sound) {

                  deactivate_event_callback (es->event, EventEndMask);
               }

               es = _eventTable.get_next (it);
            }
         }
      }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_audioMod && (_audioMod == AudioModule::cast (PluginPtr))){

         _eventTable.empty ();
         _audioMod = 0;
      }
   }
}


// Event Observer Interface
void
dmz::AudioPluginEventSimple::close_event (
      const Handle EventHandle,
      const EventType &Type,
      const EventLocalityEnum Locality) {

   EventType current (Type);
   EventStruct *es (0);

   while (!es && current) {

      es = _eventTable.lookup (current.get_handle ());

      if (!es) { current.become_parent (); }
   }
  
   if (es && es->sound) {

      EventModule *eventMod (get_event_module ());

      if (_audioMod && eventMod) {

         Vector pos;

         if (eventMod->lookup_position (EventHandle, _defaultEventHandle, pos)) {

            SoundAttributes sa;
            sa.set_position (pos);
            sa.set_loop (False);
            _audioMod->play_sound (es->sound, sa);
         }
      }
   }
}


void
dmz::AudioPluginEventSimple::_init (Config &local) {

   Definitions defs (get_plugin_runtime_context (), &_log);

   _defaultEventHandle = defs.create_named_handle (EventAttributeDefaultName);

   Config eventList;

   if (local.lookup_all_config ("event", eventList)) {

      ConfigIterator it;
      Config event;

      while (eventList.get_next_config (it, event)) {

         const String FileName = config_to_string ("file", event);
         const String EventName = config_to_string ("type", event);

         if (FileName && EventName) {

            EventStruct *es = new EventStruct (FileName);

            if (es) {

               es->event = activate_event_callback (EventName, EventEndMask);

               if (es->event) {

                  const Handle EventHandle (es->event.get_handle ());

                  if (!_eventTable.store (EventHandle, es)) {

                     delete es; es = 0;
                     es = _eventTable.lookup (EventHandle);

                     _log.error << "Unable to bind sound: " << FileName << " to event: "
                        << EventName << " because file: "
                        << (es ? es->File : "<Unknown File>")
                        << " has already been bound to the event type" << endl;
                  }
               }
               else {

                  delete es; es = 0;

                  _log.error << "Unknown event type: " << EventName << endl;
               }
            }
         }

         if (!FileName) {

            _log.error << "No audio file specified for event type: "
               << (EventName ? EventName : "<Unknown Type>") << endl;
         }

         if (!EventName) {

            _log.error << "No event type specified for audio file: "
               << (FileName ? FileName : "<Unknown File>") << endl;
         }
      }
   }
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
