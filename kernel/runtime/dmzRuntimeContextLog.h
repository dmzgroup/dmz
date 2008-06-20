#ifndef DMZ_RUNTIME_CONTEXT_LOG_DOT_H
#define DMZ_RUNTIME_CONTEXT_LOG_DOT_H

#include "dmzRuntimeContextThreadKey.h"
#include <dmzRuntimeLog.h>
#include <dmzSystemMutex.h>
#include <dmzSystemRefCount.h>
#include <dmzSystemThread.h>
#include <dmzTypesHashTableHandleTemplate.h>
#include <dmzTypesString.h>

namespace dmz {

   class LogBuffer : public ThreadCleanup {

      public:
         String buffer;
         ThreadStorageTemplate<LogBuffer> &ts;

         LogBuffer (ThreadStorageTemplate<LogBuffer> &theTs) : ts (theTs) {;}
         virtual ~LogBuffer () { ts.set_data (0); }
   };

   class RuntimeContextLog : public RefCountDeleteOnZero {

      public:
         RuntimeContextLog (RuntimeContextThreadKey *key);

         void update_time_slice ();

         void attach_log_observer (LogObserver &obs);
         void detach_log_observer (LogObserver &obs);

         String *get_buffer ();

         void write_message (
            const String &LogName,
            const LogLevelEnum Level,
            const String &Message);

      protected:
         struct EventStruct;
         String _mainBuffer; //!< Main buffer.
         ThreadStorageTemplate<LogBuffer> _bufferTable; //!< Thread buffers.
         Mutex _mutex; //!< Lock.
         RuntimeContextThreadKey *_threadKey; //!< Thread key.
         HashTableHandleTemplate<LogObserver> _obsTable; //!< Observer table.
         Boolean _eventsToProcess; //!< Flag.
         EventStruct *_events; //!< Messages from other threads to process.
         EventStruct *_eventsTail; //!< End of messages list.

      private:
         ~RuntimeContextLog ();
   };
};

#endif // DMZ_RUNTIME_CONTEXT_LOG_DOT_H
