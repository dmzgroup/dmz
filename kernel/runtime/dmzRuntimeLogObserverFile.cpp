#include <dmzRuntimeLogObserverFile.h>
#include <dmzSystemFile.h>
#include <dmzSystemStreamFile.h>
#include <stdio.h>

/*!

\class dmz::LogObserverFile
\ingroup Runtime
\brief File log observer that writes log message to the specified file.

*/

namespace {

static const dmz::Int32 LocalMaxLevels = 4;
static const char *LocalLevelStr[] = { 
   "Debug:",
   "Info:",
   "Warn:",
   "Error:",
   "Out:",
 };

};


struct dmz::LogObserverFile::State {

   FILE *file;
   StreamFile out;

   State (FILE *theFile) : file (theFile), out (theFile) {;}

   ~State () { if (file) { close_file (file); file = 0; } }
};


/*!

\brief Constructor.
\param[in] FileName String containing the name of the file to write the log.
\param[in] context Pointer to the runtime context.

*/
dmz::LogObserverFile::LogObserverFile (const String &FileName, RuntimeContext *context) :
      LogObserver (context),
      _state (*(new State (open_file (FileName, "wb")))) {

   if (!_state.file) { detach_log_observer (); }
}


//! Destructor.
dmz::LogObserverFile::~LogObserverFile () { delete &_state; }


void
dmz::LogObserverFile::store_log_message (
      const String &LogName,
      const LogLevelEnum Level,
      const String &Message) {

   if (_state.file && (Level >= 0)) {

      if (Level <= LocalMaxLevels) { _state.out << LocalLevelStr[Level]; }

      if (LogName) { _state.out << LogName.get_buffer () << ":"; }

      if (Message) { _state.out << Message.get_buffer (); }

      _state.out << endl;
   }
}

