#include <dmzTest.h>
#include <dmzCommandLine.h>
#include <dmzCommandLineConfig.h>
#include <dmzSystem.h>

/*!

\class dmz::Test
\ingroup Foundation
\brief Library unit test utility class.
\details The class provides basic unit test support for creating unit test of
libraries.

*/

/*!

\brief Constructor.
\param[in] AppName String containing unit test name.
\param[in] argc Number of arguments in argv
\param[in] argv Array of char arrays containing the command line arguments.

*/
dmz::Test::Test (const String &AppName, int argc, char *argv[]) : 
      obs (rt.get_context ()),
      config ("global"),
      log ("", rt.get_context ()),
      error (False) {

   const String LogLevelEnvStr (get_env ("DMZ_TEST_LOG_LEVEL"));
   LogLevelEnum level (LogLevelWarn);
   if (LogLevelEnvStr) { level = string_to_log_level (LogLevelEnvStr); }
   obs.set_level (level);

   log.out
      << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-" << endl
      << "Running " << AppName << " unit test." << endl
      << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-" << endl;

   CommandLine cl (argc, argv);
   CommandLineConfig clconfig;

   if (!clconfig.process_command_line (cl, config)) {

      log.error << "Unable to process command line: " << clconfig.get_error () << endl;
      error = True;
   }
}


//! Destructor.
dmz::Test::~Test () {;}


/*!

\brief Validates test.
\param[in] Message String containing description of the test.
\param[in] Result Boolean result of the test.
\return Returns the \a Result of the test.

*/
dmz::Boolean
dmz::Test::validate (const String &Message, const Boolean Result) {

   out.flush () << Message;
   if (Result) { passed (); }
   else { failed (); }

   return Result;
}


//! Indicates a test passed. Not needed if using the validate function.
void
dmz::Test::passed () {

   log.out << "PASSED: " << out << endl;
   out.flush ();
}


//! Indicates a test failed. Not needed if using the validate function.
void
dmz::Test::failed () {

   log.out << "FAILED: " << out << endl;
   out.flush ();
   error = True;
}


/*!

\brief Get test result.
\return Returns zero if all tests passed. Returns -1 if any test failed.

*/
dmz::Int32
dmz::Test::result () { return error ? -1 : 0; }

