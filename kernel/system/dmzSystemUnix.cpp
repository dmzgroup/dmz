#include <dmzSystem.h>
#include <dmzTypesUUID.h>

#if defined (__linux)
#include <time.h>
#endif // __linux

#include <sys/time.h> // for time functions
#include <stdlib.h> // for setenv
#include <unistd.h>

#if  defined (__APPLE__) || defined (MACOSX)
#include <mach/mach.h>
#include <mach/mach_time.h>
#endif

#include <uuid/uuid.h>

/*!

\addtogroup System
@{

*/

/*!

\brief Gets the name of the operating system
\details Defined in dmzSystem.h. These are the current strings that may be returned:
- "win32"
- "macos"
- "linux"
- "unknown"

\return Returns the name of current operating system store in a String.

*/
dmz::String
dmz::get_system_name () {

#if defined (__linux)
   String result ("linux");
#elif  defined (__APPLE__) || defined (MACOSX)
   String result ("macos");
#else
   String result ("unknown");
#endif
   return result;
}


/*!

\brief Gets time at the highest resolution supported by the system.
\details Defined in dmzSystem.h. Returns a dmz::Float64 containing the number of seconds
since the application was started.
\return Returns the time in seconds as a dmz::Float64.

*/
dmz::Float64
dmz::get_time () {
#if defined (__APPLE__) || defined (MACOSX)
   static Boolean firstCall (True);
   static uint64_t initTime (0);
   static mach_timebase_info_data_t info;
   static Float64 factor (0.0);

   if (firstCall) {

      mach_timebase_info (&info);
      factor = (Float64 (info.numer) / Float64 (info.denom)) * 1e-9;
      initTime = mach_absolute_time ();
      firstCall = False;
   }

   return Float64 (mach_absolute_time () - initTime) * factor;
#elif defined (__linux)
   static Boolean firstCall (True);
   static Float64 firstTime (0.0);

   struct timespec tv;
   clock_gettime (CLOCK_REALTIME, &tv);

   if (firstCall) {

      firstTime = Float64 (tv.tv_sec) + (Float64 (tv.tv_nsec) * Float64 (1.0e-9));
      firstCall = False;
   }

   return Float64 (tv.tv_sec) + (Float64 (tv.tv_nsec) * Float64 (1.0e-9)) - firstTime;
#else
   static Boolean firstCall (True);
   static Float64 firstTime (0.0);
   timeval tv;
   gettimeofday (&tv, 0);

   if (firstCall) {

      firstTime = Float64 (tv.tv_sec) + (Float64 (tv.tv_usec) * Float64 (1.0e-6));
      firstCall = False;
   }

   return Float64 (tv.tv_sec) + (Float64 (tv.tv_usec) * Float64 (1.0e-6)) - firstTime;
#endif
}


/*!

\brief Puts current thread to sleep using the highest resolution supported by the system.
\details Defined in dmzSystem.h. This function typically supports a higher resolution
of time than most system level Sleep function calls. It does this by calculating the
granularity of the system's Sleep function and then only using the system call to
sleep for an interval that will allow it to wake up before the requested time to
sleep has elapsed. The function then goes in to a busy wait loop until the remaining
time has elapsed. While this may cause a slight CPU usage spike while in the busy wait,
The function is continuously yielding control of the thread so that system responsiveness
is minimally affected.
\param[in] Time Number of seconds to sleep.

*/
void
dmz::sleep (const Float64 Time) {
#if defined (__linux)
   if (Time > 0.00001) {

      struct timespec ctime;
      clock_gettime (CLOCK_REALTIME, &ctime);

      struct timespec mstop = ctime;
      const Int32 MSec = Int32 (Time);
      mstop.tv_sec += MSec;
      mstop.tv_nsec += Int32 ((Time - Float64 (MSec) - 0.01) * 1.0e9);
      if (mstop.tv_nsec > Int32 (1e9)) { mstop.tv_sec++; mstop.tv_nsec -= Int32 (1e9); }

      struct timespec stop = ctime;
      const Int32 CSec = Int32 (Time);
      stop.tv_sec += CSec;
      stop.tv_nsec += Int32 ((Time - Float64 (CSec)) * 1.0e9);
      if (stop.tv_nsec > Int32 (1e9)) { stop.tv_sec++; stop.tv_nsec -= Int32 (1e9); }

      Boolean done (False);

      struct timespec ms;
      ms.tv_sec = 0;
      ms.tv_nsec = 6000000;

      while (!done) {

         if ((ctime.tv_sec > mstop.tv_sec) ||
               ((ctime.tv_sec == mstop.tv_sec) && (ctime.tv_nsec >= mstop.tv_nsec))) {

            if ((ctime.tv_sec > stop.tv_sec) ||
                  ((ctime.tv_sec == stop.tv_sec) && (ctime.tv_nsec >= stop.tv_nsec))) {

               done = True;
            }
            else { for (int ix = 0; ix < 100; ix++) { ::sleep (0); } }
         }
         else { nanosleep (&ms, 0); }

         if (!done) { clock_gettime (CLOCK_REALTIME, &ctime); }
      }
   }
   else { ::sleep (0); }
#else
   struct timespec tv;
   tv.tv_sec = UInt32 (Time);
   tv.tv_nsec = UInt32 ((Time - Float64 (tv.tv_sec)) * Float64 (1.0e9));
   nanosleep (&tv, 0);
#endif
}


/*!

\brief Sets an environment variable.
\details Defined in dmzSystem.h.
\param[in] Name String containing the name of the environment variable to set.
\param[in] Value String containing the value of the environment variable to set.

*/
void
dmz::set_env (const String &Name, const String &Value) {

   setenv (Name.get_buffer (), Value.get_buffer (), 1);
}


/*!

\brief Gets an environment variable.
\details Defined in dmzSystem.h.
\param[in] Name String containing the name of the environment variable to get.
\return Returns string containing the value of the environment variable.

*/
dmz::String
dmz::get_env (const String &Name) {

   String result (getenv (Name.get_buffer ()));
   return result;
}


/*!

\brief Creates Universally Unique Identifier (UUID).
\details Defined in dmzSystem.h.
Creates a uuid and stores is in \a uuid. Returns False when creation fails.
@see dmz::UUID

 */
dmz::Boolean
dmz::create_uuid (UUID &uuid) {

   Boolean result (False);

   uuid_t value = {
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

   uuid_generate (value);
   uuid.from_array (value);

   if (uuid) { result = True; }
   else {

      uuid_generate (value);
      uuid.from_array (value);

      if (uuid) { result = True; }
   }

   return result;
}

//! @}
