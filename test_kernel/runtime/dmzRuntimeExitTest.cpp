#include <dmzRuntimeInit.h>
#include <dmzRuntimeData.h>
#include <dmzRuntimeLog.h>
#include <dmzTest.h>

#include <dmzRuntimeExit.h>

using namespace dmz;

int
main (int argc, char *argv[]) {

   Test test ("dmzRuntimeExitTest", argc, argv);
   RuntimeContext *context (test.rt.get_context ());

   Exit exitObject (context);
   ExitObserverBasic exitObserver (context);

   test.validate (
      "exitObserver does not initially want to quit.",
      !exitObserver.is_exit_requested ());

   exitObject.request_exit (ExitStatusNormal, "User Request");

   test.validate (
      "exitObserver has recieved the request to exit the program.",
      exitObserver.is_exit_requested ());

   return test.result ();
}
