#include <dmzRuntimeLogObserverBasic.h>
#include <stdio.h>

#ifdef _WIN32
#include <windows.h>
#endif

/*!

\class dmz::LogObserverBasic
\ingroup Runtime
\brief Simple log observer that prints log message to stderr.
\details This class can use ANSI terminal commands to create colored output.
Error level log messages are printed in red and warning level log messages are
printed in yellow. In addition, the class provides support for the unit test
functionality by changing the string "FAILED" to red and "PASSED" to green.

*/

static const dmz::String LocalPassed ("PASSED");
static const dmz::String LocalFailed ("FAILED");
static char *localLevelStr[] = { "[D]:", "[I]:", "[W]:", "[E]:", "" };

struct dmz::LogObserverBasic::State {

   LogLevelEnum level;
   Boolean useColor;

#ifdef _WIN32
   CONSOLE_SCREEN_BUFFER_INFO defaultInfo;
   HANDLE out;
#endif

   State () : level (LogLevelDebug), useColor (True) {
#ifdef _WIN32
      out = GetStdHandle (STD_ERROR_HANDLE);
      GetConsoleScreenBufferInfo (out, &defaultInfo);
#endif
   }

   void set_white (const Boolean Bright = False) {

      if (useColor) {
#ifdef _WIN32
         SetConsoleTextAttribute (
            out,
//            FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | (Bright ? FOREGROUND_INTENSITY : 0));
            FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
#else
         if (Bright) { fprintf (stderr, "\033[0;1;37m"); }
         else { fprintf (stderr, "\033[0;37m"); }
#endif
      }
   }

   void set_blue (const Boolean Bright = False) {

      if (useColor) {
#ifdef _WIN32
         SetConsoleTextAttribute (
            out,
//            FOREGROUND_BLUE | (Bright ? FOREGROUND_INTENSITY : 0));
            FOREGROUND_BLUE | FOREGROUND_INTENSITY);
#else
         if (Bright) { fprintf (stderr, "\033[0;1;34m"); }
         else { fprintf (stderr, "\033[0;34m"); }
#endif
      }
   }

   void set_green (const Boolean Bright = False) {

      if (useColor) {
#ifdef _WIN32
         SetConsoleTextAttribute (
            out,
//            FOREGROUND_GREEN | (Bright ? FOREGROUND_INTENSITY : 0));
            FOREGROUND_GREEN | FOREGROUND_INTENSITY);
#else
         if (Bright) { fprintf (stderr, "\033[0;1;32m"); }
         else { fprintf (stderr, "\033[0;32m"); }
#endif
      }
   }

   void set_yellow (const Boolean Bright = False) {

      if (useColor) {
#ifdef _WIN32
         SetConsoleTextAttribute (
            out,
//            FOREGROUND_RED | FOREGROUND_GREEN | (Bright ? FOREGROUND_INTENSITY : 0));
            FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
#else
         if (Bright) { fprintf (stderr, "\033[0;1;33m"); }
         else { fprintf (stderr, "\033[0;33m"); }
#endif
      }
   }

   void set_red (const Boolean Bright = False) {

      if (useColor) {
#ifdef _WIN32
         SetConsoleTextAttribute (
            out,
//            FOREGROUND_RED | (Bright ? FOREGROUND_INTENSITY : 0));
            FOREGROUND_RED | FOREGROUND_INTENSITY);
#else
         if (Bright) { fprintf (stderr, "\033[0;1;31m"); }
         else { fprintf (stderr, "\033[0;31m"); }
#endif
      }
   }

   void set_default () {

      if (useColor) {
#ifdef _WIN32
         SetConsoleTextAttribute (out, defaultInfo.wAttributes);
#else
         fprintf (stderr, "\033[0m");
#endif
      }
   }
};


/*!

\brief Constructor.
\param[in] context Pointer to the runtime context.

*/
dmz::LogObserverBasic::LogObserverBasic (RuntimeContext *context) :
      LogObserver (context),
      _state (*(new State)) {;}


//! Destructor.
dmz::LogObserverBasic::~LogObserverBasic () { delete &_state; }


void
dmz::LogObserverBasic::store_log_message (
      const String &LogName,
      const LogLevelEnum Level,
      const String &Message) {

   if ((Level >= 0) && (Level >= _state.level)) {

//      if (Level == LogLevelDebug) { _state.set_blue (); }
//      else if (Level == LogLevelInfo) { _state.set_green (); }
//      else if (Level == LogLevelWarn) { _state.set_yellow (); }
//      else if (Level == LogLevelError) { _state.set_red ();  }
//      else if (Level == LogLevelOut) { _state.set_white ();  }

      if (Level == LogLevelWarn) { _state.set_yellow (); }
      else if (Level == LogLevelError) { _state.set_red ();  }

      fprintf (stderr, "%s", localLevelStr[Level]);

      if (LogName) { fprintf (stderr, "%s:", LogName.get_buffer ()); }

      Int32 remainder (0);

      if (Message.get_sub (0, LocalPassed.get_length () - 1) == LocalPassed) {

         _state.set_green (True);
         fprintf (stderr, "%s", LocalPassed.get_buffer ());
         _state.set_default ();
         remainder = LocalPassed.get_length ();
      }
      else if (Message.get_sub (0, LocalFailed.get_length () - 1) == LocalFailed) {

         _state.set_red (True);
         fprintf (stderr, "%s", LocalFailed.get_buffer ());
         _state.set_default ();
         remainder = LocalFailed.get_length ();
      }

      const String Out (Message.get_sub (remainder).get_buffer ());

      if (!Out) { fprintf (stderr, "\n"); }
      else { fprintf (stderr, "%s\n", Out.get_buffer ()); }

      _state.set_default ();
   }
}


/*!

\brief Sets log message level that the observer will print to stderr.
\details This function controls the level of message that is printed to stderr.
\param[in] Level Minimum dmz::LogLevelEnum that will be output to stderr.

*/
void
dmz::LogObserverBasic::set_level (const LogLevelEnum Level) { _state.level = Level; }


/*!

\brief Gets current log stream level used for output.
\return Returns dmz::LogLevelEnum currently used to determine log message level set
to stderr.

*/
dmz::LogLevelEnum
dmz::LogObserverBasic::get_level () { return _state.level; }


/*!

\brief Sets color mode.
\param[in] Mode Color mode. dmz::True turns colored output on. dmz::False turns color
off.

*/
void
dmz::LogObserverBasic::set_color_mode (const Boolean Mode) { _state.useColor = Mode; }


/*!

\brief Gets color mode.
\return Returns dmz::True if colored output is on, dmz::False if colored output is off.

*/
dmz::Boolean
dmz::LogObserverBasic::get_color_mode () { return _state.useColor; }

