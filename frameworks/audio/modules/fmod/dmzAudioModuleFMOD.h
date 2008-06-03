#ifndef DMZ_AUDIO_MODULE_FMOD_DOT_H
#define DMZ_AUDIO_MODULE_FMOD_DOT_H

#include <dmzAudioModule.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigRead.h>
#include <dmzRuntimeHandle.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimeSync.h>
#include <dmzSystemRefCount.h>
#include <dmzTypesBase.h>
#include <dmzTypesHashTableStringTemplate.h>
#include <dmzTypesHashTableUInt32Template.h>
#include <dmzTypesMatrix.h>
#include <dmzTypesVector.h>

#include <fmod.hpp>
#include <fmod_errors.h>

namespace dmz {

   class AudioModuleFMOD : 
         public Plugin,
         public Sync,
         private AudioModule {

      public:
         AudioModuleFMOD (const PluginInfo &Info, const Config &Local);
         ~AudioModuleFMOD ();

         virtual void discover_plugin (const Plugin *PluginPtr);
         virtual void start_plugin ();
         virtual void stop_plugin ();
         virtual void shutdown_plugin ();
         virtual void remove_plugin (const Plugin *PluginPtr);

         // Sync Interface
         virtual void update_sync (const Float64 TimeDelta);

         virtual UInt32 create_audio_handle (const String &FileName);
         virtual Boolean destroy_audio_handle (const UInt32 AudioHandle);

         virtual UInt32 play_sound (
            const UInt32 AudioHandle, 
            const SoundAttributes &Attributes);

         virtual Boolean update_sound (
            const UInt32 InstanceHandle, 
            const SoundAttributes &Attributes);

         virtual Boolean lookup_sound (
            const UInt32 InstanceHandle, 
            SoundAttributes &attributes);

         virtual Boolean stop_sound (const UInt32 InstanceHandle);

         virtual Boolean set_mute_all_state (const Boolean Mute);
         virtual Boolean get_mute_all_state (Boolean &mute);

         virtual UInt32 create_listener (const String &Name);
         virtual UInt32 lookup_listener (const String &Name);

         virtual Boolean set_listener (
            const UInt32 Handle,
            const Vector &Position,
            const Matrix &Orientation);

         virtual Boolean get_listener (
            const UInt32 Handle,
            Vector &position,
            Matrix &orientation);

         virtual Boolean destroy_listener (const UInt32 Handle);

      protected:
         struct SoundStruct : public RefCountDeleteOnZero {

            const String Filename;
            const RuntimeHandle Handle;
            FMOD::Sound *sound;

            UInt32 get_handle () const { return Handle.get_runtime_handle (); }

            SoundStruct (
                  const String &AbsFilePath, 
                  FMOD::Sound *soundData, 
                  RuntimeContext *context) :
                  RefCountDeleteOnZero (),
                  Filename (AbsFilePath),
                  Handle (Filename + ".SoundDataFMOD", context),
                  sound (soundData) {;}

            protected:
               ~SoundStruct () { if (sound) { sound->release (); } }
         };

         struct InstanceStruct {

            const RuntimeHandle Handle;
            AudioModuleFMOD &module;

            Float64 defaultFrequency;
            SoundStruct *data;
            FMOD::Channel *channel;

            InstanceStruct *next;

            UInt32 get_handle () const { return Handle.get_runtime_handle (); }

            void reset () {

               defaultFrequency = 1.0;
               if (data) { data->unref (); }
               data = 0;
               channel = 0;
               next = 0;
            }

            InstanceStruct (
                  const String &FilePath, 
                  AudioModuleFMOD &theModule,
                  SoundStruct *soundData, 
                  RuntimeContext *context) :
                  Handle (FilePath + ".SoundInstanceFMOD", context), 
                  module (theModule),
                  defaultFrequency (1.0),
                  data (soundData),
                  channel (0),
                  next (0) {
                  
               if (data) { data->ref (); }
            }

            ~InstanceStruct () { reset (); }
         };

         struct ListenerStruct {
         
            const RuntimeHandle Handle;
            const String Name;
            Int32 index;

            Vector position;
            Matrix orientation;
               
            UInt32 get_handle () const { return Handle.get_runtime_handle (); }

            ListenerStruct (
                  const String &TheName, 
                  const Int32 TheIndex, 
                  RuntimeContext *context) :
                  Handle (TheName + ".AudioListenerFMOD", context),
                  Name (TheName), 
                  index (TheIndex) {;}

            ~ListenerStruct () {;}
         };

         static FMOD_RESULT F_CALLBACK _channel_callback (
            FMOD_CHANNEL *channel, 
            FMOD_CHANNEL_CALLBACKTYPE type, 
            int command, 
            unsigned int commanddata1, 
            unsigned int commanddata2);

         void _init (const Config &Local);

         Boolean _error_check (
            const String &HeaderMessage, 
            const FMOD_RESULT Error);
 
         InstanceStruct *_get_new_instance (SoundStruct *soundData);
         void _remove_instance (UInt32 InstanceHandle);

         Log _log;

         FMOD::System *_system;

         HashTableUInt32Template<InstanceStruct> _instanceTable;
         HashTableUInt32Template<SoundStruct> _soundHandleTable;
         HashTableStringTemplate<SoundStruct> _soundNameTable;
         HashTableStringTemplate<ListenerStruct> _listenerNameTable;
         HashTableUInt32Template<ListenerStruct> _listenerHandleTable;
         HashTableUInt32Template<ListenerStruct> _listenerIndexTable;

         InstanceStruct *_instanceRecycle;

         Float32 _dopplerScale;
         Float32 _distanceFactor;
         Float32 _rollOffScale;
         Boolean _rightHandedCoordinates;
         Boolean _forceFullDirectSoundHRTF;
         Int32 _maxChannels;

      private:
         AudioModuleFMOD (const AudioModuleFMOD &);
         AudioModuleFMOD &operator= (const AudioModuleFMOD &);
   };
};


inline dmz::Boolean
dmz::AudioModuleFMOD::_error_check (
      const String &HeaderMessage, 
      const FMOD_RESULT Error) {
   
   Boolean result (True);

   if (Error != FMOD_OK) {

      result = False;

      _log.error << "FMOD error - " << HeaderMessage << ": " 
         << FMOD_ErrorString(Error) << endl;
   }

   return result;
}

#endif //  DMZ_AUDIO_MODULE_FMOD_DOT_H
