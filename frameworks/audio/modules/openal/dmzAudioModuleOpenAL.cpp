#include "dmzAudioModuleOpenAL.h"
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzSystemFile.h>
#include <dmzTypesMatrix.h>
#include <dmzTypesVector.h>

dmz::AudioModuleOpenAL::AudioModuleOpenAL (const PluginInfo &Info, Config &local) :
      Plugin (Info),
      TimeSlice (Info),
      AudioModule (Info),
      _log (Info),
      _device (0),
      _context (0) {

   _init (local);
}


dmz::AudioModuleOpenAL::~AudioModuleOpenAL () {

   _soundTimedTable.clear ();
   _soundTable.empty ();
   _bufferHandleTable.clear ();
   _bufferNameTable.empty ();

   if (_context) {

      alcMakeContextCurrent (0);
      alcDestroyContext (_context);
      _context = 0;
   }

   if (_device) {

      alcCloseDevice (_device);
      _device = 0;
   }
}


// Plugin Interface
void
dmz::AudioModuleOpenAL::update_plugin_state (
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
dmz::AudioModuleOpenAL::discover_plugin (
      const PluginDiscoverEnum Mode,
      const Plugin *PluginPtr) {

   if (Mode == PluginDiscoverAdd) {

   }
   else if (Mode == PluginDiscoverRemove) {

   }
}


// Time Slice Interface
void
dmz::AudioModuleOpenAL::update_time_slice (const Float64 TimeDelta) {

   HashTableHandleIterator it;

   SoundStruct *ss (_soundTimedTable.get_first (it));

   while (ss) {

      ALint value (0);

      alGetSourcei (ss->source, AL_SOURCE_STATE, &value);

      if (value == AL_STOPPED) {

         _soundTimedTable.remove (ss->Handle.get_runtime_handle ());
         _soundTable.remove (ss->Handle.get_runtime_handle ());

         delete ss; ss = 0;
      }

      ss = _soundTimedTable.get_next (it);
   }
}


// Audio Module Interface
dmz::Handle
dmz::AudioModuleOpenAL::create_audio_handle (const String &FileName) {

   Handle result (0); 
   String absPath;

   if (_context && get_absolute_path (FileName, absPath)) {

      BufferStruct *bs (_bufferNameTable.lookup (absPath));

      if (!bs) {

         bs = new BufferStruct (absPath, get_plugin_runtime_context ());

         if (bs && !_bufferNameTable.store (FileName, bs)) { delete bs; bs = 0; }
         else { _bufferHandleTable.store (bs->Handle.get_runtime_handle (), bs); }

          if (bs &&
                bs->file.is_valid () &&
                (bs->file.get_audio_format () == WaveFormatPCM)) {

             alGenBuffers (1, &(bs->buffer));
             ALenum format (0);
             const UInt32 Channels (bs->file.get_channel_count ());
             const UInt32 BPS (bs->file.get_bits_per_sample ());

             if (Channels == 1) {

                if (BPS == 8) { format = AL_FORMAT_MONO8; }
                else if (BPS == 16) { format = AL_FORMAT_MONO16; }
             }
             else if (Channels == 2) {

                if (BPS == 8) { format = AL_FORMAT_STEREO8; }
                else if (BPS == 16) { format = AL_FORMAT_STEREO16; }
             }

             UInt32 size (0);
             ALvoid *data = (ALvoid *)(bs->file.get_audio_buffer (size));

             if (size && data && format) {

                alBufferData (
                   bs->buffer,
                   format,
                   data,
                   (ALsizei)size,
                   bs->file.get_frequency ());

                ALenum error = alGetError ();

                if (error != AL_NO_ERROR) {

                   _log.error << "Unable to bind file: " << FileName
                      << " to OpenAL buffer." << endl;
                   delete bs; bs = 0;
                }

                _log.info << "Loaded audio file: " << bs->FileName << endl;
                bs->file.clear ();
             }
          }
          else if (bs) {

             if (bs->file.is_valid ()) {

                _log.error << "Unable to load audio file: " << FileName << " because: "
                   << " Wave audio data is not PCM." << endl;
             }
             else {

                _log.error << "Unable to load audio file: " << FileName << " because: "
                   << bs->file.get_error () << endl;
             }

             delete bs; bs = 0;
          }
      }

      if (bs) {

         result = bs->Handle.get_runtime_handle ();
      }
   }

   return result;
}


dmz::Boolean
dmz::AudioModuleOpenAL::destroy_audio_handle (const Handle AudioHandle) {

   Boolean result (False);

   BufferStruct *bs (_bufferHandleTable.remove (AudioHandle));

   if (bs) {

      bs = _bufferNameTable.remove (bs->FileName);

      if (bs) { delete bs; bs = 0; result = True; }
   }

   return result;
}


dmz::Handle
dmz::AudioModuleOpenAL::play_sound (
      const Handle AudioHandle,
      const SoundAttributes &Attributes) {

   Handle result (0);

   BufferStruct *bs (_bufferHandleTable.lookup (AudioHandle));
   SoundStruct *ss (new SoundStruct (get_plugin_runtime_context ()));

   if (bs && ss) {

      result = ss->Handle.get_runtime_handle ();
      ss->looped = Attributes.get_loop ();

      if (_soundTable.store (result, ss)) {

         alSourcei (ss->source, AL_BUFFER, bs->buffer);
         alSourcei (ss->source, AL_LOOPING, ss->looped ? AL_TRUE : AL_FALSE);

         if (!(ss->looped)) {

            _soundTimedTable.store (result, ss);
         }

         ss->attr = Attributes;

         _update_sound (*ss);
         alSourcePlay (ss->source);
_log.warn << "Playing sound: " << bs->FileName << endl;
      }
      else { delete ss; ss = 0; result = 0; }

   }
   else { delete ss; ss = 0; }

   return result;
}


dmz::Boolean
dmz::AudioModuleOpenAL::update_sound (
      const Handle InstanceHandle,
      const SoundAttributes &Attributes) {

   Boolean result (False);

   SoundStruct *ss (_soundTable.lookup (InstanceHandle));

   if (ss) { ss->attr = Attributes; _update_sound (*ss); result = True; }

   return result;
}


dmz::Boolean
dmz::AudioModuleOpenAL::lookup_sound (
      const Handle InstanceHandle,
      SoundAttributes &attributes) {

   Boolean result (False);

   SoundStruct *ss (_soundTable.lookup (InstanceHandle));

   if (ss) { attributes = ss->attr; attributes.set_loop (ss->looped); result = True; }

   return result;
}


dmz::Boolean
dmz::AudioModuleOpenAL::stop_sound (const Handle InstanceHandle) {

   Boolean result (False);

   SoundStruct *ss (_soundTable.remove (InstanceHandle));

   if (ss) {

_log.warn << "Stopping sound: " << ss->Handle.get_runtime_handle () << endl;

      _soundTimedTable.remove (InstanceHandle);

      delete ss; ss = 0;
      result = True;
   }

   return result;
}


dmz::Boolean
dmz::AudioModuleOpenAL::set_mute_all_state (const Boolean Mute) {

   Boolean result (False);

   return result;
}


dmz::Boolean
dmz::AudioModuleOpenAL::get_mute_all_state (Boolean &mute) {

   Boolean result (False);

   return result;
}


dmz::Handle
dmz::AudioModuleOpenAL::create_listener (const String &Name) {

   Handle result (0);

   if (!_listenerName && Name) {

      result = get_plugin_handle ();
      _listenerName = Name;
   }

   return result;
}


dmz::Handle
dmz::AudioModuleOpenAL::lookup_listener (const String &Name) {

   Handle result (0);

   if (Name && (_listenerName == Name)) { result = get_plugin_handle (); }

   return result;
}


dmz::Boolean
dmz::AudioModuleOpenAL::set_listener (
      const Handle ListenerHandle,
      const Vector &Position,
      const Matrix &Orientation) {

   Boolean result (False);

   if (ListenerHandle == get_plugin_handle ()) {

      Vector f (0.0, 0.0, -1.0);
      Vector up (0.0, 1.0, 1.0);
      Orientation.transform_vector (f);
      Orientation.transform_vector (up);

      ALfloat ori[6];
      ori[0] = (ALfloat)f.get_x ();
      ori[1] = (ALfloat)f.get_y ();
      ori[2] = (ALfloat)f.get_z ();
      ori[3] = (ALfloat)up.get_x ();
      ori[4] = (ALfloat)up.get_y ();
      ori[5] = (ALfloat)up.get_z ();

      alListenerfv (AL_ORIENTATION, ori);

      alListener3f (
         AL_POSITION,
#if 1
      0.0f, 0.0f, 0.0f);
#else
         (ALfloat)Position.get_x (),
         (ALfloat)Position.get_y (),
         (ALfloat)Position.get_z ());
#endif

      alListener3f (
         AL_VELOCITY,
         (ALfloat)0.0f,
         (ALfloat)0.0f,
         (ALfloat)0.0f);

      result = True;
   }

   return result;
}


dmz::Boolean
dmz::AudioModuleOpenAL::get_listener (
      const Handle ListenerHandle,
      Vector &position,
      Matrix &orientation) {

   Boolean result (False);

   return result;
}


dmz::Boolean
dmz::AudioModuleOpenAL::destroy_listener (const Handle ListenerHandle) {

}


void
dmz::AudioModuleOpenAL::_update_sound (SoundStruct &ss) {

   Vector vec;

   ss.attr.get_position (vec);
   alSource3f (
      ss.source,
      AL_POSITION,
#if 1
      0.0f, 0.0f, 0.0f);
#else
      (ALfloat)vec.get_x (),
      (ALfloat)vec.get_y (),
      (ALfloat)vec.get_z ());
#endif

   ss.attr.get_velocity (vec);
   alSource3f (
      ss.source,
      AL_VELOCITY,
      (ALfloat)vec.get_x (),
      (ALfloat)vec.get_y (),
      (ALfloat)vec.get_z ());

   alSourcef (ss.source, AL_PITCH, (ALfloat)ss.attr.get_pitch_scale ());
}


void
dmz::AudioModuleOpenAL::_init (Config &local) {

   _device = alcOpenDevice (0);

   if (_device) {

      _context = alcCreateContext (_device, 0);

      if (_context) { alcMakeContextCurrent (_context); }
      else { _log.error << "Unable to create OpenAL Context." << endl; }
   }
   else { _log.error << "Unable to create OpenAL Device." << endl; }
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzAudioModuleOpenAL (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::AudioModuleOpenAL (Info, local);
}

};
