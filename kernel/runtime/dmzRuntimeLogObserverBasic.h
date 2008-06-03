#ifndef DMZ_RUNTIME_LOG_OBSERVER_BASIC_DOT_H
#define DMZ_RUNTIME_LOG_OBSERVER_BASIC_DOT_H

#include <dmzKernelExport.h>
#include <dmzRuntimeLog.h>

namespace dmz {

   class RuntimeContext;

   class DMZ_KERNEL_LINK_SYMBOL LogObserverBasic : protected LogObserver {

      public:
         LogObserverBasic (RuntimeContext *context);
         ~LogObserverBasic ();

         void set_level (const LogLevelEnum Level);
         LogLevelEnum get_level ();

         void set_color_mode (const Boolean Mode);
         Boolean get_color_mode ();

      protected:
         virtual void store_log_message (
            const String &LogName,
            const LogLevelEnum Level,
            const String &Message);

         struct State;
         State &_state; //!< Internal state.

      private:
         LogObserverBasic ();
         LogObserverBasic (const LogObserverBasic &);
         LogObserverBasic &operator= (const LogObserverBasic &);
   };
};

#endif // DMZ_RUNTIME_LOG_OBSERVER_BASIC_DOT_H
