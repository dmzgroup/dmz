#include <dmzRuntime.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimeLogObserverBasic.h>

int
main (int argc, char *argv[]) {

   dmz::Runtime rt;
   dmz::Log log ("test", rt.get_context ());
   dmz::LogObserverBasic obs (rt.get_context ());

   log.out << "Message 1" << dmz::endl;
   log.error << "Message 2" << dmz::endl;
   log.warn << "Message 3" << dmz::endl;
   log.info << "Message 4" << dmz::endl;
   log.debug << "Message 5" << dmz::endl;

   rt.update_time_slice ();

   return 0;
}
