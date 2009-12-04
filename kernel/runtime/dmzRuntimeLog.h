#ifndef DMZ_RUNTIME_LOG_DOT_H
#define DMZ_RUNTIME_LOG_DOT_H

#include <dmzKernelExport.h>
#include <dmzSystemStream.h>

namespace dmz {

   class RuntimeContext;
   class PluginInfo;

   enum LogLevelEnum {
      LogLevelNever = -1, //!< Level never. Print nothing.
      LogLevelDebug, //!< Print to debug level.
      LogLevelInfo, //!< Print to info level.
      LogLevelWarn, //!< Print to warn level.
      LogLevelError,  //!< Print to error level.
      LogLevelOut, //!< Print to out level.
   };

   DMZ_KERNEL_LINK_SYMBOL LogLevelEnum string_to_log_level (const String &Level);

   class DMZ_KERNEL_LINK_SYMBOL StreamLog : public Stream {

      public:
         StreamLog (
            const String &LogName,
            const LogLevelEnum Level,
            RuntimeContext *context);

         virtual ~StreamLog ();

         virtual Stream &write_raw_data (const UInt8 *Data, const Int32 Size);
         virtual Stream &flush ();
         virtual Stream &newline ();

         virtual Stream &operator<< (const UInt16 Value);
         virtual Stream &operator<< (const UInt32 Value);
         virtual Stream &operator<< (const UInt64 Value);

         virtual Stream &operator<< (const Int16 Value);
         virtual Stream &operator<< (const Int32 Value);
         virtual Stream &operator<< (const Int64 Value);

         virtual Stream &operator<< (const Float32 Value);
         virtual Stream &operator<< (const Float64 Value);

         virtual Stream &operator<< (const String &Value);
         virtual Stream &operator<< (const char Value);
         virtual Stream &operator<< (const char *Value);

         virtual Stream &operator<< (const void *Value);
         virtual Stream &operator<< (stream_operator_func func);

      protected:
         struct State;
         State &_state; //!< Internal state.

      private:
         StreamLog ();
         StreamLog (const StreamLog &);
         const StreamLog &operator= (const StreamLog &);
   };

   class DMZ_KERNEL_LINK_SYMBOL Log {

      public:
         Log (const PluginInfo &Info);
         Log (const String &LogName, RuntimeContext *context);
         ~Log ();

         StreamLog out; //!< dmz::LogLevelOut StreamLog.
         StreamLog error; //!< dmz::LogLevelError StreamLog.
         StreamLog warn; //!< dmz::LogLevelWarn StreamLog.
         StreamLog info; //!< dmz::LogLevelInfo StreamLog.
         StreamLog debug; //!< dmz::LogLevelDebug StreamLog.

      private:
         Log ();
         Log (const Log &);
         const Log &operator= (const Log &);
   };

   class DMZ_KERNEL_LINK_SYMBOL LogObserver {

      public:
         LogObserver (RuntimeContext *context);

         Handle get_log_observer_handle ();

         void attach_log_observer ();
         void detach_log_observer ();

         virtual void store_log_message (
            const String &LogName,
            const LogLevelEnum Level,
            const String &Message) = 0;

      protected:
         struct LogObsState;
         ~LogObserver ();
         LogObsState &_logObsState; //!< Internal state.

      private:
         LogObserver ();
         LogObserver (const LogObserver &);
         const LogObserver &operator= (const LogObserver &);
   };
};

#endif // DMZ_RUNTIME_LOG_DOT_H

