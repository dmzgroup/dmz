#include <dmzAudioSoundAttributes.h>
#include <dmzTypesVector.h>

/*!

\class dmz::SoundAttributes
\ingroup Audio
\brief Audio instance attributes.
\details Provides a container for getting and setting a sound instance's attributes
such as position, velocity, and pitch.

*/


struct dmz::SoundAttributes::State {

   Vector position;
   Vector velocity;
   Float64 gain;
   Float64 pitch;

   State () :
      gain (1.0),
      pitch (1.0) {;}

   State &operator= (const State &Value) {

      position = Value.position;
      velocity = Value.velocity;
      pitch = Value.pitch;
      gain = Value.gain;

      return *this;
   }
};


//! Constructor
dmz::SoundAttributes::SoundAttributes () : _state (*(new State)) {;}


//! Copy constructor.
dmz::SoundAttributes::SoundAttributes (const SoundAttributes &Value) :
      _state (*(new State)) { *this = Value; }


//! Destructor.
dmz::SoundAttributes::~SoundAttributes () { delete &_state; }


//! Assignment operator.
dmz::SoundAttributes &
dmz::SoundAttributes::operator= (const SoundAttributes &Value) {

   _state = Value._state;

   return *this;
}


//! Set position.
void
dmz::SoundAttributes::set_position (const Vector &Pos) {

   _state.position = Pos;
}


//! Get position.
void
dmz::SoundAttributes::get_position (Vector &pos) const {

   pos = _state.position;
}


//! Set velocity.
void
dmz::SoundAttributes::set_velocity (const Vector &Vel) {

   _state.velocity = Vel;
}


//! Get velocity.
void
dmz::SoundAttributes::get_velocity (Vector &vel) const {

   vel = _state.velocity;
}


//! Set gain scale. A value of 1.0 specifies the default gain.
void
dmz::SoundAttributes::set_gain_scale (const Float64 Value) {

   _state.gain = Value;
}


//! Get gain scale.
dmz::Float64
dmz::SoundAttributes::get_gain_scale () const { return _state.gain; }


//! Set pitch scale. A value of 1.0 specifies the default pitch.
void
dmz::SoundAttributes::set_pitch_scale (const Float64 Value) {

   _state.pitch = Value;
}


//! Get pitch scale.
dmz::Float64
dmz::SoundAttributes::get_pitch_scale () const { return _state.pitch; }

