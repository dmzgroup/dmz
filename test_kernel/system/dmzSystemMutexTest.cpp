#include <dmzSystemMutex.h>
#include <dmzSystemThread.h>
#include <dmzSystem.h>
#include <stdio.h>

class thread : public dmz::ThreadFunction {

   protected:
      dmz::Mutex _mutex;
      dmz::Int32 _count;

   public:
      dmz::Mutex wait;
      thread () : _count (0) { wait.lock (); }
      virtual ~thread () {;}

      void print (const char *ptr) {

         _mutex.lock ();
            _count++;
            fprintf (stderr, "%s: %d\n", ptr, _count);
         _mutex.unlock ();
      }

      virtual void run_thread_function () {

         for (dmz::Int32 ix = 0; ix < 100000; ix++) { print ("thread"); }

         wait.unlock ();
      }
};


int
main (int argc, char *argv[]) {

   const dmz::Float64 StartTime (dmz::get_time ());

   thread t;

   dmz::create_thread (t);
   for (dmz::Int32 ix = 0; ix < 10000; ix++) { t.print ("main"); }

   while (!t.wait.try_lock ()) { dmz::sleep (0.001); }

   const dmz::Float64 EndTime (dmz::get_time ());
   fprintf (stderr, "Runtime: %f\n", EndTime - StartTime);

   return 0;
}

