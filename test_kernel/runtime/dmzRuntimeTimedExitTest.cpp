#include <dmzRuntimeInit.h>
#include <dmzRuntimeData.h>
#include <dmzRuntimeExit.h>
#include <dmzRuntimeLog.h>
#include <dmzSystem.h>
#include <dmzTest.h>

using namespace dmz;

int
main (int argc, char *argv[]) {

   Test test ("dmzRuntimeTimedExitTest", argc, argv);
   RuntimeContext *context (test.rt.get_context ());

   ExitObserverBasic exitObserver (context);

   test.validate (
      "exitObserver does not initially want to quit.",
      !exitObserver.is_exit_requested ());

   test.rt.sync ();

   TimedExit timedExitObject (0.1, ExitStatusNormal, context);
   timedExitObject.start_sync ();

   test.rt.sync ();

   test.validate (
      "exitObserver has not recieved the timed message instantly.",
      !exitObserver.is_exit_requested ());

   sleep (0.05);
   test.rt.sync ();
   test.validate (
      "exitObserver has not recieved the timed message in < time period.",
      !exitObserver.is_exit_requested ());

   sleep (0.1);
   test.rt.sync ();
   test.validate (
      "exitObserver recieves the message after the time has passed.",
      exitObserver.is_exit_requested ());

   // test get_status and set_status
   test.validate (
      "get_status reads correct initial value.",
      timedExitObject.get_status () == ExitStatusNormal);

   timedExitObject.set_status (ExitStatusError);

   test.validate (
      "get_status reads set value from set_status.",
      timedExitObject.get_status () == ExitStatusError);

   return test.result ();
}
