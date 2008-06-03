#include <dmzAudioSoundAttributes.h>
#include <dmzTypesVector.h>



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


dmz::SoundAttributes::SoundAttributes () : _state (*(new State)) {;}


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


dmz::SoundAttributes::SoundAttributes (const SoundAttributes &Value) :
      _state (*(new State)) { *this = Value; }


dmz::SoundAttributes::~SoundAttributes () { delete &_state; }


dmz::SoundAttributes &
dmz::SoundAttributes::operator= (const SoundAttributes &Value) {

   _state = Value._state;

   return *this;
}


void
dmz::SoundAttributes::set_position (const Vector &Pos) {

   _state.position = Pos;
}


void
dmz::SoundAttributes::get_position (Vector &pos) const { 
   
   pos = _state.position;
}


void
dmz::SoundAttributes::set_velocity (const Vector &Vel) {

   _state.velocity = Vel;
}


void
dmz::SoundAttributes::get_velocity (Vector &vel) const { 
   
   vel = _state.velocity;
}

void
dmz::SoundAttributes::set_pitch_scale (const Float64 Value) {

   _state.pitch = Value;
}


dmz::Float64
dmz::SoundAttributes::get_pitch_scale () const { return _state.pitch; }


void
dmz::SoundAttributes::set_loop (const Boolean Value) {

   _state.loop = Value;
}


dmz::Boolean
dmz::SoundAttributes::get_loop () const { return _state.loop; }

