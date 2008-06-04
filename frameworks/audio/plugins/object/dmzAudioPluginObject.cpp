#include "dmzAudioPluginObject.h"
#include <dmzAudioModule.h>
#include <dmzAudioSoundAttributes.h>
#include <dmzObjectAttributeMasks.h>
#include <dmzObjectModule.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigRead.h>
#include <dmzRuntimeObjectType.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>


dmz::AudioPluginObject::AudioPluginObject (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      Sync (Info),
      ObjectObserverUtil (Info, local),
      _log (Info),
      _defs (Info, &_log),
      _audioMod (0),
      _defaultHandle (0) {

   _init (local);

   _defaultHandle = activate_default_object_attribute (
      ObjectCreateMask |
         ObjectDestroyMask |
         ObjectTypeMask |
         ObjectStateMask |
         ObjectPositionMask |
         ObjectVelocityMask);
}


dmz::AudioPluginObject::~AudioPluginObject () {

   _updateTable.clear ();
   _objectTable.empty ();
   _soundTable.empty ();
}


// Plugin Interface
void
dmz::AudioPluginObject::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

      if (!_audioMod) {

         _audioMod = AudioModule::cast (PluginPtr);

         if (_audioMod) {

            HashTableHandleIterator it;

            SoundDefStruct *ss (_soundTable.get_first (it));

            while (ss) {

               SoundDefStruct *current (ss);

               while (current) {

                  _lookup_sound_handles (*current);

                  current = current->next;
               }

               ss = _soundTable.get_next (it);
            }

            ObjectStruct *os (_objectTable.get_first (it));

            while (os) {

               HashTableHandleIterator attrIt;

               SoundStruct *current (os->list);

               SoundAttributes attr (os->pos, os->vel, 1.0, True);

               while (current) {

                  attr.set_pitch_scale (current->scale);

                  current->handle =
                     _audioMod->play_sound (current->Data.loopHandle, attr);

                  current = current->next;
               }

               os = _objectTable.get_next (it);
            }
         }
      }
   }
   else if (Mode == PluginDiscoverRemove) {

      if (_audioMod && (_audioMod == AudioModule::cast (PluginPtr))) {

         HashTableHandleIterator it;

         _stop_all_looped_sounds ();

         SoundDefStruct *ss (_soundTable.get_first (it));

         while (ss) {

            SoundDefStruct *current (ss);

            while (current) {

               if (current->activateHandle) {

                  _audioMod->destroy_audio_handle (current->activateHandle);
                  current->activateHandle = 0;
               }

               if (current->loopHandle) {

                  _audioMod->destroy_audio_handle (current->loopHandle);
                  current->loopHandle = 0;
               }

               if (current->deactivateHandle) {

                  _audioMod->destroy_audio_handle (current->deactivateHandle);
                  current->deactivateHandle = 0;
               }

               current = current->next;
            }

            ss = _soundTable.get_next (it);
         }

         _audioMod = 0;
      }
   }
}


// Sync Interface
void
dmz::AudioPluginObject::update_sync (const Float64 TimeDelta) {

   if (_audioMod) {

      HashTableHandleIterator it;

      ObjectStruct *os (_updateTable.get_first (it));

      while (os) {

         SoundAttributes attr (os->pos, os->vel, 1.0, True);

         HashTableHandleIterator instanceIt;

         SoundStruct *current (os->list);

         while (current) {

            attr.set_pitch_scale (current->scale);

            _audioMod->update_sound (current->handle, attr);

            current = current->next;
         }

         os = _updateTable.get_next (it);
      }
   }

   _updateTable.clear ();
}


// Object Observer Interface
void
dmz::AudioPluginObject::create_object (
      const UUID &Identity,
      const Handle ObjectHandle,
      const ObjectType &Type,
      const ObjectLocalityEnum Locality) {

   if (_audioMod) {

      SoundDefStruct *ss (_get_sound_list (Type));

      if (ss) {

         ObjectStruct *os (new ObjectStruct ());

         _objectTable.store (ObjectHandle, os);

         os->list = new SoundStruct (*ss);
      }
   }
}


void
dmz::AudioPluginObject::destroy_object (const UUID &Identity, const Handle ObjectHandle) {

   ObjectStruct *os (_objectTable.remove (ObjectHandle));

   if (os) {

      _stop_looped_sounds (*os);

      _updateTable.remove (ObjectHandle);

      delete os; os = 0;
   }
}


void
dmz::AudioPluginObject::update_object_type (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const ObjectType &Value,
      const ObjectType *PreviousValue) {

}


void
dmz::AudioPluginObject::update_object_state (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Mask &Value,
      const Mask *PreviousValue) {

   ObjectStruct *os (_objectTable.lookup (ObjectHandle));

   if (os) {

      SoundStruct *current = os->list;

      SoundAttributes attr (os->pos, os->vel, 1.0, False);

      while (current) {

         const Boolean IsSet (Value.contains (current->Data.State));

         const Boolean WasSet (
            PreviousValue ? PreviousValue->contains (current->Data.State) : False);

         ObjectModule *objMod (get_object_module ());

         if (objMod && current->Data.scalarAttributeHandle) {

            objMod->lookup_scalar (
               ObjectHandle,
               current->Data.scalarAttributeHandle,
               current->scale);

            attr.set_pitch_scale (current->scale);
         }
         else { attr.set_pitch_scale (1.0); }

         if (IsSet && !WasSet) {

            if (_audioMod && current->Data.activateHandle) {

               attr.set_loop (False);
               _audioMod->play_sound (current->Data.activateHandle, attr);
            }

            if (_audioMod && current->Data.loopHandle) {

               attr.set_loop (True);
               current->handle = _audioMod->play_sound (current->Data.loopHandle, attr);
            }
         }

         if (WasSet && !IsSet) {

            if (_audioMod && current->Data.deactivateHandle) {

               attr.set_loop (False);
               _audioMod->play_sound (current->Data.deactivateHandle, attr);
            }

            if (_audioMod && current->Data.loopHandle) {

               _audioMod->stop_sound (current->handle);
               current->handle = 0;
            }
         }

         current = current->next;
      }
   }
}


void
dmz::AudioPluginObject::update_object_position (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Value,
      const Vector *PreviousValue) {

   ObjectStruct *os (_objectTable.lookup (ObjectHandle));

   if (os) {

      os->pos = Value;

      _updateTable.store (ObjectHandle, os);
   }
}


void
dmz::AudioPluginObject::update_object_velocity (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Vector &Value,
      const Vector *PreviousValue) {

   ObjectStruct *os (_objectTable.lookup (ObjectHandle));

   if (os) {

      os->vel = Value;

      _updateTable.store (ObjectHandle, os);
   }
}


void
dmz::AudioPluginObject::update_object_scalar (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Float64 Value,
      const Float64 *PreviousValue) {

   ObjectStruct *os (_objectTable.lookup (ObjectHandle));

   if (os) {

      SoundStruct *current (os->list);

      while (current) {

         if (AttributeHandle == current->Data.scalarAttributeHandle) {

            current->scale = Value;

            _updateTable.store (ObjectHandle, os);
         }

         current = current->next;
      }
   }
}


void
dmz::AudioPluginObject::_remove_object_module (ObjectModule &objMod) {

   _stop_all_looped_sounds ();
   _updateTable.clear ();
   _objectTable.empty ();
}


dmz::AudioPluginObject::SoundDefStruct *
dmz::AudioPluginObject::_get_sound_list (const ObjectType &Type) {

   ObjectType current (Type);

   SoundDefStruct *result (0);

   while (!result && current) {

      result = _soundTable.lookup (current.get_handle ());

      if (!result) {

         result = _object_type_to_sound_list (current);
      }

      current.become_parent ();
   }

   return result;
}


dmz::AudioPluginObject::SoundDefStruct *
dmz::AudioPluginObject::_object_type_to_sound_list (const ObjectType &Type) {

   SoundDefStruct *result (0);

   Config list;

   if (Type.get_config ().lookup_all_config ("audio.sound", list)) {

      SoundDefStruct *current (0);

      ConfigIterator it;
      Config data;

      Boolean found (list.get_first_config (it, data));

      while (found) {

         Mask state;
         const String StateName (config_to_string ("state", data));

         if (StateName) {

            _defs.lookup_state (StateName, state);
         }

         SoundDefStruct *ss = new SoundDefStruct (state);

         if (ss) {

            _init_sound_struct (data, *ss);

            if (!current) {

               result = ss;
               _soundTable.store (Type.get_handle (), ss);
            }
            else { current->next = ss; }

            current = ss;
         }

         found = list.get_next_config (it, data);
      }
   }

   return result;
}


void
dmz::AudioPluginObject::_init_sound_struct (Config &data, SoundDefStruct &ss) {

   ss.activateName = config_to_string ("activatefile", data);
   ss.deactivateName = config_to_string ("deactivatefile", data);
   ss.loopName = config_to_string ("loopedfile", data);

   _lookup_sound_handles (ss);

   const String ScalarName (config_to_string ("scalar", data));

   if (ScalarName) {

      ss.scalarAttributeHandle = activate_object_attribute (ScalarName, ObjectScalarMask);
   }

   ss.minScale = config_to_float64 ("minscale", data, ss.minScale);
   ss.maxScale = config_to_float64 ("maxscale", data, ss.maxScale);
}


void
dmz::AudioPluginObject::_lookup_sound_handles (SoundDefStruct &ss) {

   if (_audioMod) {

      if (ss.activateName) {

         ss.activateHandle = _audioMod->create_audio_handle (ss.activateName);
      }

      if (ss.deactivateName) {

         ss.deactivateHandle = _audioMod->create_audio_handle (ss.deactivateName);
      }

      if ( ss.loopName) {

         ss.loopHandle = _audioMod->create_audio_handle (ss.loopName);
      }
   }
}


void
dmz::AudioPluginObject::_stop_all_looped_sounds () {

   if (_audioMod) {

      HashTableHandleIterator it;

      ObjectStruct *os (_objectTable.get_first (it));

      while (os) {

         _stop_looped_sounds (*os);

         os = _objectTable.get_next (it);
      }
   }
}


void
dmz::AudioPluginObject::_stop_looped_sounds (ObjectStruct &obj) {

   HashTableHandleIterator loopIt;

   SoundStruct *current (obj.list);

   while (current) {

      _audioMod->stop_sound (current->handle);

      current->handle = 0;

      current = current->next;
   }
}


void
dmz::AudioPluginObject::_init (Config &local) {

}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzAudioPluginObject (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::AudioPluginObject (Info, local);
}

};
