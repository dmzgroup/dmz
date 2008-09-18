#include "dmzAudioPluginEventSimple.h"
#include <dmzAudioModule.h>
#include <dmzAudioSoundAttributes.h>
#include <dmzAudioSoundInit.h>
#include <dmzEventCallbackMasks.h>
#include <dmzEventConsts.h>
#include <dmzEventModule.h>
#include <dmzRuntimeConfigToBase.h>
#include <dmzRuntimeDefinitions.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzTypesVector.h>

/*!

\class dmz::AudioPluginEventSimple
\ingroup Audio
\brief Plays an audio file mapped to various EventTypes.

*/

//! \cond
dmz::AudioPluginEventSimple::AudioPluginEventSimple (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      EventObserverUtil (Info, local),
      _log (Info),
      _audioMod (0),
      _defaultEventHandle (0),
      _rc (Info) {

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

               es->sound = _audioMod->create_sound (es->File);

               if (!es->sound) {

                  deactivate_event_callback (es->event, EventCloseMask);
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

            SoundInit init;
            SoundAttributes attributes;
            attributes.set_position (pos);
            _audioMod->play_sound (es->sound, init, attributes);
         }
      }
   }
}


void
dmz::AudioPluginEventSimple::_init (Config &local) {

   Definitions defs (get_plugin_runtime_context (), &_log);

   _defaultEventHandle = defs.create_named_handle (EventAttributeDefaultName);

   Config eventList;

   if (local.lookup_all_config ("event-type", eventList)) {

      ConfigIterator it;
      Config event;

      while (eventList.get_next_config (it, event)) {

         const String EventTypeName = config_to_string ("name", event);
         const String FileName = _rc.find_file (config_to_string ("resource", event));

         if (FileName && EventTypeName) {

            EventStruct *es = new EventStruct (FileName);

            if (es) {

               es->event = activate_event_callback (EventTypeName, EventCloseMask);

               if (es->event) {

                  const Handle EventHandle (es->event.get_handle ());

                  if (!_eventTable.store (EventHandle, es)) {

                     delete es; es = 0;
                     es = _eventTable.lookup (EventHandle);

                     _log.error << "Unable to bind sound: " << FileName << " to event: "
                        << EventTypeName << " because file: "
                        << (es ? es->File : "<Unknown File>")
                        << " has already been bound to the event type" << endl;
                  }
               }
               else {

                  delete es; es = 0;

                  _log.error << "Unknown event type: " << EventTypeName << endl;
               }
            }
         }

         if (!FileName) {

            _log.error << "No audio file specified for event type: "
               << (EventTypeName ? EventTypeName : "<Unknown Type>") << endl;
         }

         if (!EventTypeName) {

            _log.error << "No event type specified for audio file: "
               << (FileName ? FileName : "<Unknown File>") << endl;
         }
      }
   }
}
//! \endcond


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzAudioPluginEventSimple (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::AudioPluginEventSimple (Info, local);
}

};
