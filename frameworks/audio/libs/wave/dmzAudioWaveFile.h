#ifndef DMZ_AUDIO_WAVE_FILE_DOT_H
#define DMZ_AUDIO_WAVE_FILE_DOT_H

#include <dmzAudioWaveExport.h>
#include <dmzTypesBase.h>
#include <dmzTypesString.h>

namespace dmz {

   //! \brief Specifies the wave format is PCM.
   //! \ingroup Audio
   const UInt32 WaveFormatPCM = 1;

   class DMZ_AUDIO_WAVE_LINK_SYMBOL WaveFile {

      public:
         WaveFile ();
         WaveFile (const String &FileName);
         ~WaveFile ();

         String get_file_name () const;

         Boolean is_valid () const;

         void clear ();

         Boolean load_file (const String &FileName);

         UInt32 get_audio_format () const;
         UInt32 get_channel_count () const;
         UInt32 get_frequency () const;
         UInt32 get_bits_per_sample () const;

         char *get_audio_buffer (UInt32 &size);

         String get_error () const;

      protected:
         struct State;
         State &_state; //!< Internal state.
   };

};

#endif // DMZ_AUDIO_WAVE_FILE_DOT_H
