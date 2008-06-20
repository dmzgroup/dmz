#include <dmzRuntimeExit.h>
#include <dmzRuntimeLog.h>
#include <dmzTestPluginUtil.h>

/*!

\class dmz::TestPluginUtil
\ingroup Foundation
\brief Plugin unit test utility class.
\details This class provides basic unit test support for creating a unit test with
Plugins.

*/

struct dmz::TestPluginUtil::State {

   Log log;
   Exit exit;
   Boolean error;

   State (RuntimeContext *context) :
         log ("", context),
         exit (context),
         error (False) {;}
};

/*!

\brief Constructor.
\param[in] AppName String containing the unit test name.
\param[in] context Pointer to the runtime context.

*/
dmz::TestPluginUtil::TestPluginUtil (const String &AppName, RuntimeContext *context) :
      __state (*(new State (context))) {

   __state.log.out
      << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-" << endl
      << "Running " << AppName << " unit test." << endl
      << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-" << endl;
}


//! Destructor.
dmz::TestPluginUtil::~TestPluginUtil () { delete &__state; }


/*!

\brief Validates test.
\param[in] Value Boolean result of the test.
\param[in] Message String containing description of the test.

*/
void
dmz::TestPluginUtil::validate (const Boolean Value, const String &Message) {

   out.flush () << Message;

   if (Value) { passed (); }
   else { failed (); }
}


//! Indicates a test passed. Not needed if using the validate function.
void
dmz::TestPluginUtil::passed () {

   __state.log.out << "PASSED: " << out << endl;
   out.flush ();
}


//! Indicates a test failed. Not needed if using the validate function.
void
dmz::TestPluginUtil::failed () {

   __state.log.out << "FAILED: " << out << endl;
   __state.error = True;
   out.flush ();
}


/*!

\brief Requests and exit.
\param[in] Message String containing exit message.

*/
void
dmz::TestPluginUtil::exit (const String &Message) {

   __state.exit.request_exit (
      __state.error ? ExitStatusError : ExitStatusNormal,
      Message);
}

