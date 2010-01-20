#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimeVersion.h>
#include <stdlib.h>
/*!

\class dmz::Version
\ingroup Runtime
\brief Container object that hold application version information.
\details Parses a Config object containing application version information and stores
the values internally. The config XML format:
\code
<dmz>
<version>
   <name value="Application Name"/>
   <major value="Major Version Number"/>
   <minor value="Minor Version Number"/>
   <bug value="Bug Version Number"/>
   <build value="Build Number"/>
   <release value="Release Tag"/>
   <image value="Image to use in about box"/>
</version>
</dmz>
\endcode

*/

struct dmz::Version::State {

   String name;
   String major;
   String minor;
   String bug;
   String build;
   Int32 buildDate;
   Int32 buildTime;
   String release;
   String image;
   
   State &operator= (const State &Value) {

      name = Value.name;
      major = Value.major;
      minor = Value.minor;
      bug = Value.bug;
      build = Value.build;
      buildDate = Value.buildDate;
      buildTime = Value.buildTime;
      release = Value.release;
      image = Value.image;

      return *this;
   }
   
   Boolean operator< (const State &Value) const {

      Boolean result;
      if (buildDate == Value.buildDate) { result = (buildTime < Value.buildTime); }
      else { result = buildDate < Value.buildDate; }
      return result;
   }

   Boolean operator> (const State &Value) const {

      Boolean result;
      if (buildDate == Value.buildDate) { result = buildTime > Value.buildTime; }
      else { result = buildDate > Value.buildDate; }
      return result;
   }

   State (const Config &Source, const String &Prefix) {

      name = config_to_string (Prefix + ".name.value", Source);
      major = config_to_string (Prefix + ".major.value", Source);
      minor = config_to_string (Prefix + ".minor.value", Source);
      bug = config_to_string (Prefix + ".bug.value", Source);
      build = config_to_string (Prefix + ".build.value", Source);
      release = config_to_string (Prefix + ".release.value", Source);
      image = config_to_string (Prefix + ".image.value", Source);
      
      Int32 index (0);
      if (build.find_sub ("INTERNAL", index)) {
         
         buildDate = buildTime = 99999999;
      }
      else if (build.find_sub ("-", index)) {
         
         buildDate = atoi (build.get_sub (0, index - 1).get_buffer ());
         buildTime = atoi (build.get_sub (index + 1, -1).get_buffer ());
      }
      else {
         
         buildDate = buildTime = atoi (build.get_buffer ());
      }
   }

   State (const State &Value) { *this = Value; }

   State () {;}
};


//! Constructor.
dmz::Version::Version () : _state (*(new State)) {;}


//! \brief Constructor.
//! \param[in] Source Config object containing the version initialization data.
dmz::Version::Version (const Config &Source) :
      _state (*(new State (Source, "dmz.version"))) {;}


//! \brief Constructor.
//! \param[in] Source Config object containing the version initialization data.
//! \param[in] Prefix String containing prefix to use in place of dmz.version.
dmz::Version::Version (const Config &Source, const String &Prefix) :
      _state (*(new State (Source, Prefix))) {;}


//! Copy constructor.
dmz::Version::Version (const Version &Value) :
      _state (*(new State (Value._state))) {;}

//! Destructor
dmz::Version::~Version () { delete &_state; }


//! Assignment operator.
dmz::Version &
dmz::Version::operator= (const Version &Value) {

   _state = Value._state;

   return *this;
}


//! Less than operator
dmz::Boolean
dmz::Version::operator< (const Version &Value) const {
	
   return _state < Value._state;
}


//! Greater than operator
dmz::Boolean
dmz::Version::operator> (const Version &Value) const {
	
   return _state > Value._state;
}


//! \brief Gets a formatted version string.
//! \return Returns a String with the contents formatted: major.minor.bug release
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


//! \brief Gets the application's name.
//! \return Returns a String containing the application's name.
dmz::String
dmz::Version::get_name () const { return _state.name; }


//! \brief Gets the application's major version number.
//! \return Returns a String containing the application's major version number.
dmz::String
dmz::Version::get_major () const { return _state.major; }


//! \brief Gets the application's minor version number.
//! \return Returns a String containing the application's minor version number.
dmz::String
dmz::Version::get_minor () const { return _state.minor; }


//! \brief Gets the application's bug version number.
//! \return Returns a String containing the application's bug version number.
dmz::String
dmz::Version::get_bug () const { return _state.bug; }


//! \brief Gets the application's build version number.
//! \return Returns a String containing the application's build version number.
dmz::String
dmz::Version::get_build () const { return _state.build; }


//! \brief Gets the application's release type.
//! \return Returns a String containing the application's release type.
dmz::String
dmz::Version::get_release () const { return _state.release; }


//! \brief Gets the name of the image to use in the about box.
//! \return Returns a String containing name of the image to use in the about box.
dmz::String
dmz::Version::get_image_name () const { return _state.image; }
