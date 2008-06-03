#include <dmzSystem.h>
#include <dmzTypesUUID.h>
#include <windows.h>
#include <Rpc.h>

static dmz::Boolean localFirstCall (dmz::True);
static LARGE_INTEGER localFrequency;
static dmz::Float64 localFrequencyFloat;
static dmz::Float64 localFrequencyFloatInverse;
static LARGE_INTEGER localFirstCallTime;
static LARGE_INTEGER localTwoMsecInTicks;

static void
local_init () {

   // We never call timeEndPeriod because there is no clean up function for this lib.
   // It shouldn't matter since we want a high resolution clock the whole time we are
   // running.
   timeBeginPeriod (1);
   QueryPerformanceFrequency (&localFrequency);
   localTwoMsecInTicks.QuadPart = localFrequency.QuadPart / 500;
   localFrequencyFloat = dmz::Float64 (localFrequency.QuadPart);
   localFrequencyFloatInverse = 1 / localFrequencyFloat;
   QueryPerformanceCounter (&localFirstCallTime);
   localFirstCall = dmz::False;
}


dmz::String
dmz::get_system_name () {

   String result ("win32");
   return result;
}


dmz::Float64
dmz::get_time () {

   if (localFirstCall) { local_init (); }
   LARGE_INTEGER ctime;
   QueryPerformanceCounter (&ctime);
   return Float64 (ctime.QuadPart - localFirstCallTime.QuadPart) *
      localFrequencyFloatInverse;
}


void
dmz::sleep (const Float64 Time) {

   if (localFirstCall) { local_init (); }
   if (Time > 0.00001) {

      LARGE_INTEGER stime, ctime;
      QueryPerformanceCounter (&stime);
      Int64 wait (Int64 (localFrequencyFloat * Time));
      Int64 ticksLeft (wait);
      Boolean done (False);

      while (!done) {

         // When there are less than two milliseconds left, go into a busy wait.
         if (ticksLeft > localTwoMsecInTicks.QuadPart) { Sleep (1); }
         else { for (Int32 ix = 0; ix < 10; ix++) { Sleep (0); } }
         QueryPerformanceCounter (&ctime);
         ticksLeft = wait - Int64 (ctime.QuadPart - stime.QuadPart);

         if (ctime.QuadPart < stime.QuadPart) { done = True; }
         else if (ticksLeft <= 0) { done = True; }
      }
   }
   else { Sleep (0); }
}


void
dmz::set_env (const String &Name, const String &Value) {

#if _MSC_VER >= 1400
   _putenv_s (Name.get_buffer (), Value.get_buffer ());
#else
   String invalue (Name + "=" + Value);
   putenv (invalue.get_buffer ());
#endif
}


dmz::String
dmz::get_env (const String &Name) {

   String result;
#if _MSC_VER >= 1400
   size_t size (0);
   getenv_s (&size, 0, 0, Name.get_buffer ());
   if (size > 0) {

      char *buffer = new char[size + 1];

      if (buffer) {

         buffer[size] = '\0';
         getenv_s (&size, buffer, size, Name.get_buffer ());

         result.set_buffer (buffer, size - 1);

         delete []buffer; buffer = 0;
      }
   }
#else
   result = getenv (Name.get_buffer ());
#endif

   return result;
}


dmz::Boolean
dmz::create_uuid (UUID &uuid) {

   Boolean result (False);

   ::UUID w32UUID;

   if (RPC_S_OK == UuidCreate (&w32UUID)) {

      UInt8 array[16];
      memcpy ((void *)&(array[0]), (void *)&(w32UUID.Data1), sizeof (long));
      memcpy ((void *)&(array[4]), (void *)&(w32UUID.Data2), sizeof (short));
      memcpy ((void *)&(array[6]), (void *)&(w32UUID.Data3), sizeof (short));
      memcpy ((void *)&(array[8]), (void *)&(w32UUID.Data4[0]), sizeof (char) * 8);
      uuid.from_array (array);
      result = True;
   }

   return result;
}

