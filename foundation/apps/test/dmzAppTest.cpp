#include <dmzCommandLine.h>
#include <dmzApplication.h>
#include <dmzRuntimeLogObserverBasic.h>
#include <dmzSystem.h>

using namespace dmz;

int
main (int argc, char *argv[]) {

   CommandLine cl (argc, argv);

   Application app ("dmzAppTest", "dmz");

   app.set_quiet (True);

   LogObserverBasic obs (app.get_context ());

   const String LogLevelEnvStr (get_env ("DMZ_TEST_APP_LOG_LEVEL"));

   const LogLevelEnum Level (
      LogLevelEnvStr ? string_to_log_level (LogLevelEnvStr) : LogLevelWarn);

   obs.set_level (Level);

   app.load_session ();
   app.process_command_line (cl);
   app.load_plugins ();
   app.start ();
   while (app.sync ()) {;}
   app.stop ();
   app.unload_plugins ();
   app.save_session ();

   return app.is_error () ? -1 : 0;
}
