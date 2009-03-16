#include <dmzAudioModule.h>
#include "dmzAudioPluginEvent.h"
#include <dmzAudioSoundAttributes.h>
#include <dmzAudioSoundInit.h>
#include <dmzEventCallbackMasks.h>
#include <dmzEventConsts.h>
#include <dmzEventModule.h>
#include <dmzObjectModule.h>
#include <dmzRuntimeConfig.h>
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

   if (!_ignoredEventTypes.contains_exact_type (Type) && _audioMod && _eventMod) {

      EventStruct *es = _create_event_struct (Type);

      if (!es) {

         EventType current (Type);

         while (!es && current.become_parent ()) {

            es = _create_event_struct (current);
         }
      }

      if (es) {

         Handle sound (es->Sound);

         AttrStruct *attr = es->attr;
         ObjectTypeStruct *ots = es->types;

         while (ots && attr) {

            ObjectType type;

            if (!_eventMod->lookup_object_type (EventHandle, attr->Attr, type) &&
                  (_objMod != 0)) {

               Handle obj (0);

               if (_eventMod->lookup_object_handle (EventHandle, attr->Attr, obj)) {

                  type = _objMod->lookup_object_type (obj);
               }
            }

            if (type) {

               ObjectTypeStruct *next = ots->table.lookup (type.get_handle ());

               while (!next && type.become_parent ()) {

                  next = ots->table.lookup (type.get_handle ());
               }

               if (next) { ots = next; attr = attr->next; }

               if (ots->Sound) { sound = ots->Sound; }
            }
            else { ots = 0; attr = 0; }
         }

         if (sound) {

            Vector pos;

            _eventMod->lookup_position (EventHandle, _defaultEventHandle, pos);

            SoundInit si;
            SoundAttributes sa;
            sa.set_position (pos);

            _audioMod->play_sound (sound, si, sa);
         }
      }
      else { _ignoredEventTypes.add_event_type (Type); }
   }
}


dmz::AudioPluginEvent::EventStruct *
dmz::AudioPluginEvent::_create_event_struct (const EventType &Type) {

   EventStruct *result (_eventTable.lookup (Type.get_handle ()));

   if (!result && _audioMod) {

      Config audio;

      if (Type.get_config ().lookup_all_config_merged ("audio", audio)) {

         const String SoundName = config_to_string ("resource", audio);

         const Handle Sound = _create_sound (SoundName);

         result = new EventStruct (Sound);

         if (result && !_eventTable.store (Type.get_handle (), result)) {

            delete result; result = 0;
         }

         if (result) {

            _create_attr_list (Type, audio, *result);

            ObjectTypeStruct *types = new ObjectTypeStruct (0);

            if (types) {

               _create_obj_types (Type, audio, *types);

               if (types->table.get_count ()) { result->types = types; }
               else { delete types; types = 0; }
            }
         }
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
dmz::AudioPluginEvent::_create_attr_list (
      const EventType &Type,
      const Config &Source,
      EventStruct &event) {

   Config AttrList;

   if (Source.lookup_all_config ("attribute", AttrList)) {

      AttrStruct *first (0);
      AttrStruct *current (0);

      ConfigIterator it;
      Config attr;

      while (AttrList.get_next_config (it, attr)) {

         String attrName;

         if (config_to_boolean ("source", attr)) { attrName = EventAttributeSourceName; }
         else if (config_to_boolean ("target", attr)) {

            attrName = EventAttributeTargetName;
         }
         else if (config_to_boolean ("munitions", attr)) {

            attrName = EventAttributeMunitionsName;
         }
         else { attrName = config_to_string ("name", attr); }

         Handle attrHandle (0);

         if (attrName) { attrHandle = _defs.create_named_handle (attrName); }
         else {

            _log.error << "Unable to find attribute name for audio event: "
               << Type.get_name () << endl;
         }

         AttrStruct *as = new AttrStruct (attrHandle);

         if (!current) { current = first = as; }
         else { current->next = as; current = as; }
      }

      if (event.attr) { delete (event.attr); event.attr = 0; }
      event.attr = first;
   }
}


void
dmz::AudioPluginEvent::_create_obj_types (
      const EventType &Event,
      const Config &Source,
      ObjectTypeStruct &types) {

   Config list;

   if (Source.lookup_all_config ("object-type", list)) {

      ConfigIterator it;
      Config data;

      while (list.get_next_config (it, data)) {

         const String TypeName = config_to_string ("name", data);

         ObjectType type;

         if (_defs.lookup_object_type (TypeName, type)) {

            const String SoundName = config_to_string ("resource", data);

            const Handle Sound = _create_sound (SoundName);

            ObjectTypeStruct *ots = new ObjectTypeStruct (Sound);

            if (ots && !types.table.store (type.get_handle (), ots)) {

               delete ots; ots = types.table.lookup (type.get_handle ());
            }

            if (ots) { _create_obj_types (Event, data, *ots); }
         }
         else {

            _log.error << "Unknown Object Type: " << TypeName << " in audio event: "
               << Event.get_name () << endl;
         }
      }
   }
}


void
dmz::AudioPluginEvent::_init (Config &local) {

   RuntimeContext *context = get_plugin_runtime_context ();

   _defaultEventHandle = _defs.create_named_handle (EventAttributeDefaultName);

   _eventTypes = config_to_event_type_set ("event-list", local, context);

   if (_eventTypes.get_count ()) {

      EventTypeIterator it;
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

   _eventTable.empty ();
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
