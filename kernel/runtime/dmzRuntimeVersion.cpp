#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeVersion.h>

struct dmz::Version::State {

   String name;
   String major;
   String minor;
   String bug;
   String build;
   String release;
   String image;

   State &operator= (const State &Value) {

      name = Value.name;
      major = Value.major;
      minor = Value.minor;
      bug = Value.bug;
      build = Value.build;
      release = Value.release;
      image = Value.image;

      return *this;
   }

   State (const Config &Source, const String &Prefix) {

      name = config_to_string (Prefix + ".name.value", Source);
      major = config_to_string (Prefix + ".major.value", Source);
      minor = config_to_string (Prefix + ".minor.value", Source);
      bug = config_to_string (Prefix + ".bug.value", Source);
      build = config_to_string (Prefix + ".build.value", Source);
      release = config_to_string (Prefix + ".release.value", Source);
      image = config_to_string (Prefix + ".image.value", Source);
   }

   State (const State &Value) { *this = Value; }

   State () {;}
};


dmz::Version::Version () : _state (*(new State)) {;}


dmz::Version::Version (const Config &Source) :
      _state (*(new State (Source, "dmz.version"))) {;}


dmz::Version::Version (const Config &Source, const String &Prefix) :
      _state (*(new State (Source, Prefix))) {;}


dmz::Version::Version (const Version &Value) :
      _state (*(new State (Value._state))) {;}


dmz::Version &
dmz::Version::operator= (const Version &Value) {

   _state = Value._state;

   return *this;
}


dmz::String
dmz::Version::get_version () const {

   String result;

   if (_state.major) { result << _state.major; }
   else { result << "X"; }

   result << ".";

   if (_state.minor) { result << _state.minor; }
   else { result << "X"; }

   result << ".";

   if (_state.bug) { result << _state.bug; }
   else { result << "X"; }

   if (_state.release) { result << " " << _state.release; }

   return result;
}


dmz::String
dmz::Version::get_name () const { return _state.name; }


dmz::String
dmz::Version::get_major () const { return _state.major; }


dmz::String
dmz::Version::get_minor () const { return _state.minor; }


dmz::String
dmz::Version::get_bug () const { return _state.bug; }


dmz::String
dmz::Version::get_build () const { return _state.build; }


dmz::String
dmz::Version::get_release () const { return _state.release; }


dmz::String
dmz::Version::get_image_name () const { return _state.image; }

