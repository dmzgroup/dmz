#include <dmzAudioModule.h>
#include "dmzAudioPluginEvent.h"
#include <dmzAudioSoundAttributes.h>
#include <dmzAudioSoundInit.h>
#include <dmzEventCallbackMasks.h>
#include <dmzEventConsts.h>
#include <dmzEventModule.h>
#include <dmzObjectModule.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigToNamedHandle.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeObjectType.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzTypesVector.h>

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
      _rc (Info, &_log),
      _defs (Info),
      _audioMod (0),
      _eventMod (0),
      _objMod (0),
      _defaultEventHandle (0) {

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
      if (!_eventMod) { _eventMod = EventModule::cast (PluginPtr); }
      if (!_objMod) { _objMod = ObjectModule::cast (PluginPtr); }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_audioMod && (_audioMod == AudioModule::cast (PluginPtr))) {

         _clear ();
         _audioMod = 0;
      }

      if (_eventMod && (_eventMod == EventModule::cast (PluginPtr))) { _eventMod = 0; }
      if (_objMod && (_objMod == ObjectModule::cast (PluginPtr))) { _objMod = 0; }
   }
}


// Event Observer Interface
void
dmz::AudioPluginEvent::close_event (
      const Handle EventHandle,
      const EventType &Type,
      const EventLocalityEnum Locality) {

   if (_audioMod && _eventMod) {

      const Handle Sound = _get_sound (EventHandle, Type);

      if (Sound) {

         Vector pos;

         _eventMod->lookup_position (EventHandle, _defaultEventHandle, pos);

         SoundInit si;
         SoundAttributes sa;
         sa.set_position (pos);

         _audioMod->play_sound (Sound, si, sa);
      }
   }
}


dmz::Handle
dmz::AudioPluginEvent::_get_sound (
      const Handle EventHandle,
      const EventType &Type) {

   Handle result (0);

   TypeStruct *ts (0);

   TypeTable *table (0);

   EventType event (Type);

   while (event && !result) {

      TypeTable *table = _get_type_table (Type);

      if (table) {

         ObjectType current;

         if (_eventMod) {

            if (!_eventMod->lookup_object_type (EventHandle, table->TypeAttr, current)) {

               Handle obj (0);

               _eventMod->lookup_object_handle (
                  EventHandle,
                  table->TypeAttr,
                  obj);

               if (_objMod && obj) { current = _objMod->lookup_object_type (obj); }
            }
         }

         const ObjectType Start (current);

         while (current && !ts) {

            ts = table->map.lookup (current.get_handle ());

            if (!ts) { ts = _create_type (EventHandle, event, current, *table); }

            if (!ts) { current.become_parent (); }
         }

         if (ts) {

            result = ts->Sound;

            if (current != Start) {

               table->map.store (Start.get_handle (), ts);
            }
         }

         if (!result) { result = table->Sound; }
      }

      if (!result) { event.become_parent (); }
   }

   return result;
}


dmz::AudioPluginEvent::TypeTable *
dmz::AudioPluginEvent::_get_type_table (const EventType &Type) {

   TypeTable *result (_typeTable.lookup (Type.get_handle ()));

   if (!result) {

      result = new TypeTable (
         _create_sound (config_to_string ("audio.resource", Type.get_config ())),
         config_to_named_handle (
            "audio.type-attribute",
            Type.get_config (),
            EventAttributeMunitionsName,
            get_plugin_runtime_context ()));

      if (!_typeTable.store (Type.get_handle (), result)) { delete result; result = 0; }
   }

   return result;
}


dmz::AudioPluginEvent::TypeStruct *
dmz::AudioPluginEvent::_create_type (
      const Handle EventHandle,
      const EventType &Event,
      const ObjectType &Object,
      TypeTable &table) {

   TypeStruct *result (0);

   Config info;

   Config list;

   if (Object.get_config ().lookup_all_config ("audio.event", list)) {

      ConfigIterator it;
      Config next;

      const String EventName = Event.get_name ();

      while (!info && list.get_next_config (it, next)) {

         if (EventName == config_to_string ("name", next)) { info = next; }
      }
   }

   if (info) {

      const Handle Sound = _create_sound (config_to_string ("resource", info));

      result = new TypeStruct (Sound);

      if (result) {

         if (table.table.store (Object.get_handle (), result)) {

            table.map.store (Object.get_handle (), result);
         }
         else { delete result; result = 0; }
      }
   }

   return result;
}


dmz::Handle
dmz::AudioPluginEvent::_create_sound (const String &Name) {

   Handle *ptr (_soundTable.lookup (Name));

   Handle result (ptr ? *ptr : 0);

   if (Name && !result) {

      const String FileName = _rc.find_file (Name);

      if (_audioMod && FileName) {

         result = _audioMod->create_sound (FileName);

         if (result) {

            ptr = new Handle (result);

            if (ptr && !_soundTable.store (Name, ptr)) { delete ptr; ptr = 0; }
         }
         else {

            _log.error << "Failed to load audio resource: " << Name << " from file: "
               << FileName << endl;
         }
      }
   }

   return result;
}


void
dmz::AudioPluginEvent::_init (Config &local) {

   RuntimeContext *context = get_plugin_runtime_context ();

   _defaultEventHandle = _defs.create_named_handle (EventAttributeDefaultName);

   _eventTypes = config_to_event_type_set ("event-list", local, context);

   if (_eventTypes.get_count ()) {

      RuntimeIterator it;
      EventType type;

      while (_eventTypes.get_next (it, type)) {

         activate_event_callback (type, EventCloseMask);
      }
   }
   else {

      activate_event_callback (_defs.get_root_event_type (), EventCloseMask);
   }
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

   _typeTable.empty ();
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
