#include <dmzAudioSoundInit.h>

/*!

\class dmz::SoundInit
\ingroup Audio
\brief Audio instance initialization parameters.
\details Provides a container for getting and setting a sound instance's initialization
parameters such as looped and relative. All parameters default to dmz::False.

*/

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


//! Constructor.
dmz::SoundInit::SoundInit () : _state (*(new State)) {;}


//! Copy constructor.
dmz::SoundInit::SoundInit (const SoundInit &Value) :
      _state (*(new State (Value._state))) {
}


//! Destructor.
dmz::SoundInit::~SoundInit () { delete &_state; }


//! Assignment operator.
dmz::SoundInit &
dmz::SoundInit::operator= (const SoundInit &Value) {

   _state = Value._state;

   return *this;
}


/*!

\brief Sets the specified initialization parameter.
\param[in] Type The SoundInitEnum specifying the initialization parameter to set.
\param[in] Value The value of the parameter.

*/
void
dmz::SoundInit::set (const SoundInitEnum Type, const Boolean Value) {

   if (Type < SoundMaxEnumValue) { _state.values[Type] = Value; }
}


/*!

\brief Gets the specified initialization parameter.
\param[in] Type The SoundInitEnum specifying the initialization parameter to get.
\return Returns the value of the parameter.

*/
dmz::Boolean
dmz::SoundInit::get (const SoundInitEnum Type) const {

   Boolean result (False);

   if (Type < SoundMaxEnumValue) { result = _state.values[Type]; }

   return result;
}
