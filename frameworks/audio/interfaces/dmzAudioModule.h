#ifndef DMZ_AUDIO_MODULE_DOT_H
#define DMZ_AUDIO_MODULE_DOT_H

#include <dmzRuntimePlugin.h>
#include <dmzRuntimeRTTI.h>
#include <dmzTypesBase.h>

#define DMZ_AUDIO_MODULE_INTERFACE_NAME "AudioModuleInterface"

namespace dmz {

   class Vector;
   class Matrix;
   class SoundAttributes;

   class AudioModule {

      public:
         static AudioModule *cast (
            const Plugin *PluginPtr,
            const String &PluginName = "");

         virtual UInt32 create_audio_handle (const String &FileName) = 0;
         virtual Boolean destroy_audio_handle (const UInt32 AudioHandle) = 0;

         virtual UInt32 play_sound (
            const UInt32 AudioHandle, 
            const SoundAttributes &Attributes) = 0;

         virtual Boolean update_sound (
            const UInt32 InstanceHandle, 
            const SoundAttributes &Attributes) = 0;

         virtual Boolean lookup_sound (
            const UInt32 InstanceHandle, 
            SoundAttributes &attributes) = 0;

         virtual Boolean stop_sound (const UInt32 InstanceHandle) = 0;

         virtual Boolean set_mute_all_state (const Boolean Mute) = 0;
         virtual Boolean get_mute_all_state (Boolean &mute) = 0;

         virtual UInt32 create_listener (const String &Name) = 0;
         virtual UInt32 lookup_listener (const String &Name) = 0;

         virtual Boolean set_listener (
            const UInt32 Handle,
            const Vector &Position,
            const Matrix &Orientation) = 0;

         virtual Boolean get_listener (
            const UInt32 Handle,
            Vector &position,
            Matrix &orientation) = 0;
         
         virtual Boolean destroy_listener (const UInt32 Handle) = 0;

      protected:
         AudioModule (const PluginInfo &Info);
         ~AudioModule ();

      private:
         AudioModule (const AudioModule &);
         AudioModule &operator= (const AudioModule &);
         const PluginInfo &__Info;
   };
};


inline dmz::AudioModule *
dmz::AudioModule::cast (const Plugin *PluginPtr, const String &PluginName) { 

   return (AudioModule *)lookup_rtti_interface (
      DMZ_AUDIO_MODULE_INTERFACE_NAME, 
      PluginName,
      PluginPtr);
}


inline
dmz::AudioModule::AudioModule (const PluginInfo &Info) :
      __Info (Info) {

   store_rtti_interface (DMZ_AUDIO_MODULE_INTERFACE_NAME, __Info, (void *)this);
}


inline
dmz::AudioModule::~AudioModule () {

   remove_rtti_interface (DMZ_AUDIO_MODULE_INTERFACE_NAME, __Info);
}

#endif //  DMZ_AUDIO_MODULE_DOT_H
