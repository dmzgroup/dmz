#ifndef DMZ_RUNTIME_LOG_OBSERVER_FILE_DOT_H
#define DMZ_RUNTIME_LOG_OBSERVER_FILE_DOT_H

#include <dmzKernelExport.h>
#include <dmzRuntimeLog.h>

namespace dmz {

   class RuntimeContext;

   class DMZ_KERNEL_LINK_SYMBOL LogObserverFile : protected LogObserver {

      public:
         LogObserverFile (const String &FileName, RuntimeContext *context);
         ~LogObserverFile ();

      protected:
         virtual void store_log_message (
            const String &LogName,
            const LogLevelEnum Level,
            const String &Message);

         struct State;
         State &_state; //!< Internal state.

      private:
         LogObserverFile ();
         LogObserverFile (const LogObserverFile &);
         LogObserverFile &operator= (const LogObserverFile &);
   };
};

#endif // DMZ_RUNTIME_LOG_OBSERVER_FILE_DOT_H
