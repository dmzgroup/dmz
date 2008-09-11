#import "dmzLogObserverNSLog.h"
#import <Foundation/Foundation.h>

static char *localLevelStr[] = { "[D]:", "[I]:", "[W]:", "[E]:", "" };


dmz::LogObserverNSLog::LogObserverNSLog (RuntimeContext *context) :
      LogObserver (context),
      _level (LogLevelDebug) {;}


dmz::LogObserverNSLog::~LogObserverNSLog () {;}


void
dmz::LogObserverNSLog::store_log_message (
      const String &LogName,
      const LogLevelEnum Level,
      const String &Message) {

   if ((Level >= 0) && (Level >= _level)) {

      NSLog (@"%s%s:%s", localLevelStr[Level], LogName.get_buffer (), Message.get_buffer ());
   }
}


void
dmz::LogObserverNSLog::set_level (const LogLevelEnum Level) { _level = Level; }


dmz::LogLevelEnum
dmz::LogObserverNSLog::get_level () { return _level; }


