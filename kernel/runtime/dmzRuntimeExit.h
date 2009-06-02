#ifndef DMZ_RUNTIME_EXIT_DOT_H
#define DMZ_RUNTIME_EXIT_DOT_H

#include <dmzKernelExport.h>
#include <dmzRuntimeTimeSlice.h>
#include <dmzTypesBase.h>
#include <dmzTypesString.h>

namespace dmz {

   class PluginInfo;

   enum ExitStatusEnum {
      ExitStatusError = 0, //!< Exit requested due to error condition.
      ExitStatusNormal, //!< Standard exit requested.
   };

   class DMZ_KERNEL_LINK_SYMBOL Exit {

      public:
         Exit (RuntimeContext *context);
         Exit (const PluginInfo &Info);
         ~Exit ();

         void request_exit (const ExitStatusEnum Status, const String &ExitReason);

      protected:
         struct State;
         State &_state; //!< Internal state.

      private:
         Exit ();
         Exit (const Exit &);
         Exit &operator= (const Exit &);
   };

   class DMZ_KERNEL_LINK_SYMBOL TimedExit : public TimeSlice, public Exit {

      public:
         TimedExit (
            const Float64 TimeInterval,
            const ExitStatusEnum Status,
            RuntimeContext *context);

         ~TimedExit ();

         ExitStatusEnum get_status () const;
         void set_status (const ExitStatusEnum Status);

      protected:
         struct State;
         void update_time_slice (const Float64 DeltaTime);
         State &_state; //!< Internal state.

      private:
         TimedExit ();
         TimedExit (const TimedExit &);
         TimedExit &operator= (const TimedExit &);
   };

   class DMZ_KERNEL_LINK_SYMBOL ExitObserver {

      public:
         virtual void exit_requested (
            const ExitStatusEnum Status,
            const String &ExitReason) = 0;

      protected:
         ExitObserver (const PluginInfo &Info);
         ExitObserver (RuntimeContext *context);
         ~ExitObserver ();

      private:
         struct State;
         ExitObserver ();
         ExitObserver (const ExitObserver &);
         ExitObserver &operator= (const ExitObserver &);
         State &__state; //!< Internal state.
   };

   class DMZ_KERNEL_LINK_SYMBOL ExitObserverBasic : protected ExitObserver {

      public:
         ExitObserverBasic (RuntimeContext *context);
         ~ExitObserverBasic ();

         Boolean is_exit_requested ();
         ExitStatusEnum get_status ();
         String get_reason ();

      protected:
         struct State;

         void exit_requested (const ExitStatusEnum Status, const String &ExitReason);

         State &_state; //!< Internal state.

      private:
         ExitObserverBasic ();
         ExitObserverBasic (const ExitObserverBasic &);
         ExitObserverBasic &operator= (const ExitObserverBasic &);
   };
};

#endif // DMZ_RUNTIME_EXIT_DOT_H
