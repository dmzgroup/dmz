#ifndef DMZ_AUDIO_SOUND_INIT_DOT_H
#define DMZ_AUDIO_SOUND_INIT_DOT_H

#include <dmzAudioBaseExport.h>
#include <dmzTypesBase.h>

namespace dmz {

   //! Initialization parameter types.
   //! \ingroup Audio
   enum SoundInitEnum {
      SoundLooped, //!< Looped sound source.
      SoundRelative, //!< Relative to the listener sound source.
      SoundMaxEnumValue, //!< Maximum parameter value.
   };

   class DMZ_AUDIO_BASE_LINK_SYMBOL SoundInit {

      public:
         SoundInit ();

         SoundInit (const SoundInit &Value);

         ~SoundInit ();

         SoundInit &operator= (const SoundInit &Value);

         void set (const SoundInitEnum Type, const Boolean Value);
         Boolean get (const SoundInitEnum Type) const;

      protected:
         struct State;
         State &_state; //!< Internal state.
   };

};

#endif // DMZ_AUDIO_SOUND_INIT_DOT_H
