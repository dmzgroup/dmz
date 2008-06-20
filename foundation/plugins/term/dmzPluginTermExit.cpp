#include "dmzPluginTermExit.h"
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzRuntimeExit.h>
#ifdef _WIN32
#   include <windows.h>

static dmz::Boolean localInit = dmz::False;
static HANDLE localConsoleStdin = 0;

#else
#   include <fcntl.h>
#   include <termios.h>
#   include <signal.h>
#   include <stdlib.h>
#   include <stdio.h>
#   include <unistd.h>
#   include <sys/types.h>

static dmz::Boolean localInit = dmz::False;
static int localTTYFD = 0;
static termios localAttr;

static void
local_sig_handler (int sig) {

   fprintf (stderr, "Calling sighandler for: ");
   if (sig == SIGILL) { fprintf (stderr, "SIGILL"); }
   else if (sig == SIGINT) { fprintf (stderr, "SIGINT"); }
   else if (sig == SIGSEGV) { fprintf (stderr, "SIGSEGV"); }
   else if (sig == SIGBUS) { fprintf (stderr, "SIGBUS"); }
   else if (sig == SIGQUIT) { fprintf (stderr, "SIGQUIT"); }
   else { fprintf (stderr, "UNKNOWN Signal"); }
   fprintf (stderr, "\n\n");

   if (localInit) {

      if (localTTYFD) {

         if (tcsetattr (localTTYFD, TCSAFLUSH, &localAttr) != 0) {;}

         close (localTTYFD); localTTYFD = 0;
      }

      localInit = dmz::False;
   }

   struct sigaction act;
   act.sa_handler = 0;
   sigemptyset (&(act.sa_mask));
   act.sa_flags = 0;
   sigaction (sig, &act, 0);
   raise (sig);
}
#endif

/*!

\class dmz::PluginTermExit
\ingroup Foundation
\brief Exit plugin.
\details This Plugin reads the terminal I/O. When the specified key is presses
in the terminal, and exit request is made. If running with out a terminal, this Plugin
serves no purpose. The default key to request an exit is the "esc" key.

*/


//! \cond
dmz::PluginTermExit::PluginTermExit (const PluginInfo &Info, Config &local) :
      Plugin (Info),
      TimeSlice (Info),
      _log (Info) {

   _init (local);
}


dmz::PluginTermExit::~PluginTermExit () {

#ifdef _WIN32
#else
   if (localInit) {

      if (localTTYFD) {

         if (tcsetattr (localTTYFD, TCSAFLUSH, &localAttr) != 0) {;}

         close (localTTYFD); localTTYFD = 0;
      }

      localInit = False;
   }
#endif
}


// TimeSlice Interface
void
dmz::PluginTermExit::update_time_slice (const Float64 TimeDelta) {

#ifdef _WIN32
   DWORD eventCount (0);

   GetNumberOfConsoleInputEvents (localConsoleStdin, &eventCount);

   if (eventCount > 0) {

      INPUT_RECORD iRec;

      DWORD blank (0);

      for (DWORD ix = 0; ix < eventCount; ix++) {

         ReadConsoleInput (localConsoleStdin, &iRec, 1, &blank);

         if (iRec.EventType == KEY_EVENT) {

            if (iRec.Event.KeyEvent.bKeyDown) {

               if (iRec.Event.KeyEvent.uChar.AsciiChar == char (27)) {

                  Exit exit (get_plugin_runtime_context ());
                  exit.request_exit (ExitStatusNormal, "Exit key pressed");
               }
            }
         }
      }
   }
#else

   Boolean done (False);
   char buf[32];

   do {

      const int Count = read (localTTYFD, buf, 32);

      if (Count <= 0) { done = True; }
      else if (Count == 1) {

         if ((buf[0] == char (27))) {

            Exit exit (get_plugin_runtime_context ());
            exit.request_exit (ExitStatusNormal, "Exit key pressed");
         }
      }
   } while (!done);
#endif
}


//! Init function.
void
dmz::PluginTermExit::_init (Config &local) {

   // This function sets the terminal I/O to be non-blocking.

#ifdef _WIN32
   if (!localInit) {

      localInit = True;

      localConsoleStdin = GetStdHandle (STD_INPUT_HANDLE);

      if (localConsoleStdin) {

         SetConsoleMode (localConsoleStdin, ENABLE_PROCESSED_INPUT);
      }
   }
#else
   if (!localInit) {

      localInit = True;

      struct sigaction oldAct;

      oldAct.sa_handler = 0;
      sigemptyset (&(oldAct.sa_mask));
      oldAct.sa_flags = 0;

      struct sigaction act;

      act.sa_handler = local_sig_handler;
      sigemptyset (&(act.sa_mask));
      act.sa_flags = 0;

      sigaction (SIGINT, &act, &oldAct);
      sigaction (SIGILL, &act, &oldAct);
      sigaction (SIGSEGV, &act, &oldAct);
      sigaction (SIGBUS, &act, &oldAct);
      sigaction (SIGQUIT, &act, &oldAct);

      termios attr;

      localTTYFD = open ("/dev/tty", O_NONBLOCK);

      if (tcgetattr (localTTYFD, &attr) != 0) {

         _log.error << "Could not get FTDIN_FILENO attributes." << endl;
      }
      else {

         localAttr = attr;

         attr.c_lflag = ISIG;
         attr.c_lflag &= ~(ECHO);

         attr.c_cc[VMIN] = 0;
         attr.c_cc[VTIME] = 0;

         if (tcsetattr (localTTYFD, TCSAFLUSH, &attr) != 0) {

            _log.error << "Could not set FTDIN_FILENO attributes." << endl;
         }
      }
   }
#endif
}
//! \endcond


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzPluginTermExit (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return localInit ? 0 : new dmz::PluginTermExit (Info, local);
}

};
