#ifndef DMZ_RUNTIME_SYNC_DOT_H
#define DMZ_RUNTIME_SYNC_DOT_H

#include <dmzKernelExport.h>
#include <dmzTypesBase.h>

namespace dmz {

   class RuntimeContext;
   class PluginInfo;

   enum TimeSliceTypeEnum {
      TimeSliceTypeSystemTime, //!< Uses system time.
      TimeSliceTypeRuntime, //!< Uses runtime time.
   };

   enum TimeSliceModeEnum {
      TimeSliceModeRepeating, //!< Repeating time slice.
      TimeSliceModeSingle, //!< Single time slice.
   };

   DMZ_KERNEL_LINK_SYMBOL Boolean
   reserve_time_slice_place (const Handle TheHandle, RuntimeContext *context);

   class DMZ_KERNEL_LINK_SYMBOL TimeSlice {

      public:
         Handle get_time_slice_handle () const;

         void set_time_slice_type (const TimeSliceTypeEnum Type);
         TimeSliceTypeEnum get_time_slice_type () const;

         void set_time_slice_mode (const TimeSliceModeEnum Mode);
         TimeSliceModeEnum get_time_slice_mode () const;

         void set_time_slice_interval (const Float64 TimeInterval);
         Float64 get_time_slice_interval () const;

         void start_time_slice ();
         void stop_time_slice ();
         void remove_time_slice ();

         virtual void update_time_slice (const Float64 DeltaTime) = 0;

      protected:
         TimeSlice (const Handle TimeSliceHandle, RuntimeContext *context);

         TimeSlice (
            const Handle TimeSliceHandle,
            const TimeSliceTypeEnum Type,
            const TimeSliceModeEnum Mode,
            const Float64 TimeInterval,
            RuntimeContext *context);

         TimeSlice (const PluginInfo &Info);

         TimeSlice (
            const PluginInfo &Info,
            const TimeSliceTypeEnum Type,
            const TimeSliceModeEnum Mode,
            const Float64 TimeInterval);

         ~TimeSlice ();

      private:
         struct State;
         TimeSlice ();
         TimeSlice (const TimeSlice &);
         TimeSlice &operator= (const TimeSlice &);
         State &__state;
   };
};

#endif // DMZ_RUNTIME_SYNC_DOT_H
