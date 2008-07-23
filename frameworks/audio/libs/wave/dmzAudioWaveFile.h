#ifndef DMZ_AUDIO_WAVE_FILE_DOT_H
#define DMZ_AUDIO_WAVE_FILE_DOT_H

#include <dmzAudioWaveExport.h>
#include <dmzTypesBase.h>
#include <dmzTypesString.h>

namespace dmz {

   class DMZ_AUDIO_WAVE_LINK_SYMBOL WaveFile {

      public:
         WaveFile ();
         WaveFile (const String &FileName);
         Boolean load_file (const String &FileName);
         ~WaveFile ();
      protected:
         struct State;
         State &_state; //!< Internal state.
   };

};

#endif // DMZ_AUDIO_WAVE_FILE_DOT_H
