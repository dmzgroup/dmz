#include <dmzAudioSoundInit.h>

struct dmz::SoundInit::State {

   Boolean values[SoundMaxEnumValue];

   State &operator= (const State &TheState) {

      for (Int32 ix = 0; ix < SoundMaxEnumValue; ix++) {

         values[ix] = TheState.values[ix];
      }

      return *this;
   }

   State () {

      for (Int32 ix = 0; ix < SoundMaxEnumValue; ix++) {

         values[ix] = False;
      }
   }

   State (const State &TheState) { *this = TheState; }
};


dmz::SoundInit::SoundInit () : _state (*(new State)) {;}


dmz::SoundInit::SoundInit (const SoundInit &Value) :
      _state (*(new State (Value._state))) {
}


dmz::SoundInit::~SoundInit () { delete &_state; }


dmz::SoundInit &
dmz::SoundInit::operator= (const SoundInit &Value) {

   _state = Value._state;

   return *this;
}


void
dmz::SoundInit::set (const SoundInitEnum Type, const Boolean Value) {

   if (Type < SoundMaxEnumValue) { _state.values[Type] = Value; }
}


dmz::Boolean
dmz::SoundInit::get (const SoundInitEnum Type) const {

   Boolean result (False);

   if (Type < SoundMaxEnumValue) { result = _state.values[Type]; }

   return result;
}
