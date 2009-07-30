#include "dmzAudioPluginObject.h"
#include <dmzAudioModule.h>
#include <dmzAudioSoundAttributes.h>
#include <dmzAudioSoundInit.h>
#include <dmzObjectAttributeMasks.h>
#include <dmzObjectModule.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeObjectType.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzTypesUUID.h>

/*!

\class dmz::AudioPluginObject
\ingroup Audio
\brief Maps object attributes to audio instances.
\details The object audio XML format:
\code
<dmz>
<runtime>
   <types>
      <object name="object type name">
         <audio>
            <state
               name="state name"
               activate-resource="audio resource name"
               looped-resource="audio resource name"
               deactivate-resource="audio resource name"
               scalar="scalar attribute name"
               offset="minimum scalar value"
               scale="scalars scale"
            />
         </audio>
      </object>
   </types>
</runtime>
</dmz>
\endcode

- \b state: Name of the state that activates the sound.
- \b activate-resource: Audio file to play when the state is set. (Optional)
- \b looped-resource: Audio file to play continuously while the state is set. (Optional)
- \b deactivate-resource: Audio file to play when the state is unset. (Optional)
- \b scalar: Name of the objects scalar attribute to used to adjust the looped
file's pitch. (Optional)
- \b offset: Minimum value the scalar will have. Defaults to 0.0. (Optional)
- \b scale: Defines the scalar's scale. Defaults to 1.0. (Optional)

At least one sound type needs to be defined (i.e.
\b activatefile, \b loopedfile, \b deactivatefile).

Pitch is calculated as follows:
\code
   value = scalar - offset
   if value < 0.0 then value = 0.0 end
   pitch = value / scale
\endcode

*/

//! \cond

dmz::AudioPluginObject::AudioPluginObject (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      TimeSlice (Info),
      ObjectObserverUtil (Info, local),
      _log (Info),
      _defs (Info, &_log),
      _rc (Info, &_log),
      _audioMod (0),
      _defaultHandle (0) {

   _init (local);

   _defaultHandle = activate_default_object_attribute (
      ObjectCreateMask |
         ObjectDestroyMask |
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

            ObjectModule *objMod (get_object_module ());

            if (objMod) {

               ObjectStruct *os (_objectTable.get_first (it));

               while (os) {

                  Mask state;
                  UUID uuid;
                  const Handle ObjectHandle = it.get_hash_key ();
                  objMod->lookup_state (ObjectHandle, _defaultHandle, state);
                  update_object_state (uuid, ObjectHandle, _defaultHandle, state, 0);

                  os = _objectTable.get_next (it);
               }
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

                  _audioMod->destroy_sound (current->activateHandle);
                  current->activateHandle = 0;
               }

               if (current->loopHandle) {

                  _audioMod->destroy_sound (current->loopHandle);
                  current->loopHandle = 0;
               }

               if (current->deactivateHandle) {

                  _audioMod->destroy_sound (current->deactivateHandle);
                  current->deactivateHandle = 0;
               }

               current = current->next;
            }

            ss = _soundTable.get_next (it);
         }

         ObjectStruct *os (_objectTable.get_first (it));

         while (os) {

            SoundStruct *ss = os->list;

            while (ss) {

               ss->handle = 0;
               ss->scale = 1.0;
               ss = ss->next;
            }

            os = _objectTable.get_next (it);
         }

         _audioMod = 0;
      }
   }
}


// TimeSlice Interface
void
dmz::AudioPluginObject::update_time_slice (const Float64 TimeDelta) {

   if (_audioMod) {

      HashTableHandleIterator it;

      ObjectStruct *os (_updateTable.get_first (it));

      while (os) {

         HashTableHandleIterator instanceIt;

         SoundStruct *current (os->list);

         while (current) {

            SoundAttributes attr;
            attr.set_pitch_scale (current->scale);

            if (!current->Data.relative) {

               attr.set_position (os->pos);
               attr.set_velocity (os->vel);
            }

            if (current->handle) { _audioMod->update_sound (current->handle, attr); }

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
dmz::AudioPluginObject::update_object_state (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Mask &Value,
      const Mask *PreviousValue) {

   ObjectStruct *os (_objectTable.lookup (ObjectHandle));

   if (os) {

      SoundStruct *current = os->list;

      while (current) {

         SoundInit init;
         SoundAttributes attr;

         if (!current->Data.relative) {

            attr.set_position (os->pos);
            attr.set_velocity (os->vel);
         }
         else { init.set (SoundRelative, True); }

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

            if (PreviousValue && _audioMod && current->Data.activateHandle) {

               init.set (SoundLooped, False);
               _audioMod->play_sound (current->Data.activateHandle, init, attr);
            }

            if (_audioMod && current->Data.loopHandle) {

               init.set (SoundLooped, True);
               current->handle =
                  _audioMod->play_sound (current->Data.loopHandle, init, attr);
            }
         }

         if (WasSet && !IsSet) {

            if (_audioMod && current->Data.deactivateHandle) {

               init.set (SoundLooped, False);
               _audioMod->play_sound (current->Data.deactivateHandle, init, attr);
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

            if (current->Data.scale > 0.0) {

               Float64 offset = Value - current->Data.offset;
               if (offset < 0.0) { offset = 0.0; }
            
               current->scale = offset / current->Data.scale;

               _updateTable.store (ObjectHandle, os);
            }
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

   if (Type.get_config ().lookup_all_config ("audio.state", list)) {

      SoundDefStruct *current (0);

      ConfigIterator it;
      Config data;

      Boolean found (list.get_first_config (it, data));

      while (found) {

         Mask state;
         const String StateName (config_to_string ("name", data));

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

   const String ActivateName = config_to_string ("activate-resource", data);
   if (ActivateName) { ss.activateName = _rc.find_file (ActivateName); }
   const String DeactivateName = config_to_string ("deactivate-resource", data);
   if (DeactivateName) { ss.deactivateName = _rc.find_file (DeactivateName); }
   const String LoopName = config_to_string ("looped-resource", data);
   if (LoopName) { ss.loopName = _rc.find_file (LoopName); }

   _lookup_sound_handles (ss);

   const String ScalarName (config_to_string ("scalar", data));

   if (ScalarName) {

      ss.scalarAttributeHandle = activate_object_attribute (ScalarName, ObjectScalarMask);
   }

   ss.offset = config_to_float64 ("offset", data, ss.offset);
   ss.scale = config_to_float64 ("scale", data, ss.scale);
   ss.relative = config_to_boolean ("relative", data, ss.relative);
}


void
dmz::AudioPluginObject::_lookup_sound_handles (SoundDefStruct &ss) {

   if (_audioMod) {

      if (ss.activateName) {

         ss.activateHandle = _audioMod->create_sound (ss.activateName);
      }

      if (ss.deactivateName) {

         ss.deactivateHandle = _audioMod->create_sound (ss.deactivateName);
      }

      if ( ss.loopName) {

         ss.loopHandle = _audioMod->create_sound (ss.loopName);
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

   if (_audioMod) {

      HashTableHandleIterator loopIt;

      SoundStruct *current (obj.list);

      while (current) {

         _audioMod->stop_sound (current->handle);

         current->handle = 0;

         current = current->next;
      }
   }
}


void
dmz::AudioPluginObject::_init (Config &local) {

}
//! \endcond


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzAudioPluginObject (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::AudioPluginObject (Info, local);
}

};
