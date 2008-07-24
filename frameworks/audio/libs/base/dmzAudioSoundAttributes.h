#ifndef DMZ_AUDIO_SOUND_ATTRIBUTES_DOT_H
#define DMZ_AUDIO_SOUND_ATTRIBUTES_DOT_H

#include <dmzAudioBaseExport.h>
#include <dmzTypesBase.h>

namespace dmz {

   class Vector;

   class DMZ_AUDIO_BASE_LINK_SYMBOL SoundAttributes {

      public:
         SoundAttributes ();

         SoundAttributes (
            const Vector &Position,
            const Vector &Velocity,
            const Float64 Pitch,
            const Boolean Loop);

         SoundAttributes (const SoundAttributes &Value);

         ~SoundAttributes ();

         SoundAttributes &operator= (const SoundAttributes &Value);

         void set_position (const Vector &Pos);
         void get_position (Vector &pos) const;

         void set_velocity (const Vector &Vel);
         void get_velocity (Vector &vel) const;

         void set_gain_scale (const Float64 Value);
         Float64 get_gain_scale () const;

         void set_pitch_scale (const Float64 Value);
         Float64 get_pitch_scale () const;

         void set_loop (const Boolean Value);
         Boolean get_loop () const;

      protected:
         struct State;
         State &_state; //!< Internal state.
   };

};

#endif // DMZ_AUDIO_SOUND_ATTRIBUTES_DOT_H
