#ifndef DMZ_LOG_OBSERVER_NSLOG_DOT_H
#define DMZ_LOG_OBSERVER_NSLOG_DOT_H

#import <dmzRuntimeLog.h>

namespace dmz {

   class RuntimeContext;

   class LogObserverNSLog : protected LogObserver {

      public:
         LogObserverNSLog (RuntimeContext *context);
         ~LogObserverNSLog ();

         void set_level (const LogLevelEnum Level);
         LogLevelEnum get_level ();

      protected:
         virtual void store_log_message (
            const String &LogName,
            const LogLevelEnum Level,
            const String &Message);

         LogLevelEnum _level;

      private:
         LogObserverNSLog ();
         LogObserverNSLog (const LogObserverNSLog &);
         LogObserverNSLog &operator= (const LogObserverNSLog &);
   };
};

#endif // DMZ_LOG_OBSERVER_NSLOG_DOT_H
