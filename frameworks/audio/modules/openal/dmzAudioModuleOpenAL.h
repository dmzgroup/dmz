#ifndef DMZ_AUDIO_MODULE_OPEN_AL_DOT_H
#define DMZ_AUDIO_MODULE_OPEN_AL_DOT_H

#include <dmzAudioModule.h>
#include <dmzAudioSoundAttributes.h>
#include <dmzAudioSoundInit.h>
#include <dmzAudioWaveFile.h>
#include <dmzRuntimeHandle.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimeTimeSlice.h>
#include <dmzSystemRefCount.h>
#include <dmzTypesHashTableStringTemplate.h>
#include <dmzTypesHashTableHandleTemplate.h>
#include <dmzTypesMatrix.h>
#include <dmzTypesVector.h>
#ifdef __linux
#   include <AL/al.h>
#   include <AL/alc.h>
#else
#   include <OpenAL/al.h>
#   include <OpenAL/alc.h>
#endif

namespace dmz {

   class AudioModuleOpenAL :
         public Plugin,
         public TimeSlice,
         public AudioModule {

      public:
         //! \cond
         AudioModuleOpenAL (const PluginInfo &Info, Config &local);
         ~AudioModuleOpenAL ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level);

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // Time Slice Interface
         virtual void update_time_slice (const Float64 TimeDelta);

         // Audio Module Interface
         virtual Handle create_sound (const String &FileName);
         virtual Boolean destroy_sound (const Handle AudioHandle);

         virtual Handle play_sound (
            const Handle AudioHandle,
            const SoundInit &Init,
            const SoundAttributes &Attributes);

         virtual Boolean update_sound (
            const Handle InstanceHandle,
            const SoundAttributes &Attributes);

         virtual Boolean lookup_sound (
            const Handle InstanceHandle,
            SoundInit &init,
            SoundAttributes &attributes);

         virtual Boolean stop_sound (const Handle InstanceHandle);

         virtual Boolean set_mute_all_state (const Boolean Mute);
         virtual Boolean get_mute_all_state (Boolean &mute);

         virtual Handle create_listener (const String &Name);
         virtual Handle lookup_listener (const String &Name);

         virtual Boolean set_listener (
            const Handle ListenerHandle,
            const Vector &Position,
            const Matrix &Orientation,
            const Vector &Velocity);

         virtual Boolean get_listener (
            const Handle ListenerHandle,
            Vector &position,
            Matrix &orientation,
            Vector &velocity);

         virtual Boolean destroy_listener (const Handle ListenerHandle);

      protected:
         struct BufferStruct : RefCount {

            const RuntimeHandle Handle;
            const String FileName;
            WaveFile file;
            ALuint buffer;
            HashTableStringTemplate<BufferStruct> &nameTable;
            HashTableHandleTemplate<BufferStruct> &handleTable;

            BufferStruct (
                  const String &TheFileName,
                  RuntimeContext *context,
                  HashTableStringTemplate<BufferStruct> &theNameTable,
                  HashTableHandleTemplate<BufferStruct> &theHandleTable) :
                  Handle (TheFileName + ".AudioBuffer", context),
                  FileName (TheFileName),
                  file (TheFileName),
                  buffer (0),
                  nameTable (theNameTable),
                  handleTable (theHandleTable) {;}

            protected:
               ~BufferStruct () {

                  if (alIsBuffer (buffer)) { alDeleteBuffers (1, &buffer); }
               }

               virtual void _ref_count_is_zero () {

                  nameTable.remove (FileName);
                  handleTable.remove (Handle.get_runtime_handle ());

                  delete this;
               }
         };

         struct SoundStruct {

            const RuntimeHandle Handle;
            BufferStruct &buffer;
            ALuint source;
            SoundAttributes attr;
            SoundInit init;

            SoundStruct (BufferStruct &theBuffer, RuntimeContext *context) :
                  Handle ("Sound Instance", context),
                  buffer (theBuffer),
                  source (0) { buffer.ref (); }

            ~SoundStruct () {

               if (source) {

                  ALint value (0);
                  alGetSourcei (source, AL_SOURCE_STATE, &value);

                  if (value == AL_PLAYING) { alSourceStop (source); }

                  alDeleteSources (1, &source);
               }

               buffer.unref ();
            }
         };

         void _update_sound (SoundStruct &ss);

         void _init (Config &local);

         Log _log;

         ALCdevice *_device;
         ALCcontext *_context;

         String _listenerName;
         Vector _listenerPos;
         Matrix _listenerOri;
         Vector _listenerVel;

         HashTableStringTemplate<BufferStruct> _bufferNameTable;
         HashTableHandleTemplate<BufferStruct> _bufferHandleTable;
         HashTableHandleTemplate<SoundStruct> _soundTable;
         HashTableHandleTemplate<SoundStruct> _soundTimedTable;
         //! \endcond

      private:
         AudioModuleOpenAL ();
         AudioModuleOpenAL (const AudioModuleOpenAL &);
         AudioModuleOpenAL &operator= (const AudioModuleOpenAL &);

   };
};

#endif // DMZ_AUDIO_MODULE_OPEN_AL_DOT_H
