#include <dmzApplication.h>
#include <dmzFoundationCommandLine.h>
#include <dmzRuntimeLogObserverBasic.h>
#include <dmzSystem.h>
#include <dmzSystemDynamicLibrary.h>

using namespace dmz;

int
main (int argc, char *argv[]) {

   CommandLine cl (argc, argv);

   Application app ("dmzAppBasic", "dmz");

   LogObserverBasic obs (app.get_context ());

   obs.set_level (string_to_log_level (get_env ("DMZ_APP_LOG_LEVEL")));

   app.load_session ();
   app.process_command_line (cl);
   app.load_plugins ();
   app.start ();
   while (app.update_time_slice ()) {;}
   app.stop ();
   app.unload_plugins ();
   app.save_session ();

   DynamicLibrary::dump_loaded (app.log.info);

   return app.is_error () ? -1 : 0;
}
