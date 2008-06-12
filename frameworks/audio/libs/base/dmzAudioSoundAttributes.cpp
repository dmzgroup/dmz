#include <dmzAudioSoundAttributes.h>
#include <dmzTypesVector.h>

/*!

\class dmz::SoundAttributes
\ingroup Audio
\brief Audio instance attributes.
\details Provides a container for getting and setting a sound instance's attributes
such as position, velocity, pitch, and looped state.

*/


struct dmz::SoundAttributes::State {

   Vector position;
   Vector velocity;
   Float64 pitch;
   Boolean loop;

   State () :
      pitch (1.0),
      loop (False) {;}

   State &operator= (const State &Value) {

      position = Value.position;
      velocity = Value.velocity;
      pitch = Value.pitch;
      loop = Value.loop;

      return *this;
   }
};


//! Constructor
dmz::SoundAttributes::SoundAttributes () : _state (*(new State)) {;}

/*!

\brief Constructor
\param[in] Position Vector containing the sound instance's position.
\param[in] Velocity Vector containing the sound instance's velocity.
\param[in] Pitch Scalar value specifying the sound instance's pitch where 1.0 gives the
default pitch.
\param[in] Loop Boolean that specifies if the sound instance should be looped.

*/
dmz::SoundAttributes::SoundAttributes (
      const Vector &Position,
      const Vector &Velocity,
      const Float64 Pitch,
      const Boolean Loop) : _state (*(new State)) {

   _state.position = Position;
   _state.velocity = Velocity;
   _state.pitch = Pitch;
   _state.loop = Loop;
}


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


//! Set pitch scale. A value of 1.0 specifies the default pitch.
void
dmz::SoundAttributes::set_pitch_scale (const Float64 Value) {

   _state.pitch = Value;
}


//! Get pitch scale.
dmz::Float64
dmz::SoundAttributes::get_pitch_scale () const { return _state.pitch; }


//! Set loop state.
void
dmz::SoundAttributes::set_loop (const Boolean Value) {

   _state.loop = Value;
}


//! Get loop state.
dmz::Boolean
dmz::SoundAttributes::get_loop () const { return _state.loop; }

