#ifndef DMZ_RUNTIME_CONTEXT_TIME_DOT_H
#define DMZ_RUNTIME_CONTEXT_TIME_DOT_H

#include <dmzRuntimeTimeSlice.h>
#include <dmzSystemMutex.h>
#include <dmzSystemRefCount.h>
#include <dmzTypesBase.h>
#include <dmzTypesHashTableHandleTemplate.h>

namespace dmz {

   struct TimeSliceStruct {

      const Handle TimeSliceHandle;
      const Int32 &Index;

      Boolean active;

      TimeSliceTypeEnum type;
      TimeSliceModeEnum mode;
      Float64 timeInterval;
      Float64 nextTimeSlice;
      TimeSlice &timeSlice;

      Boolean continuous;
      Boolean system;

      TimeSliceStruct *next;
      TimeSliceStruct *prev;

      void update ()  {

         continuous = (is_zero64 (timeInterval) && (mode == TimeSliceModeRepeating));
         system = (type == TimeSliceTypeSystemTime);
      }

      TimeSliceStruct (
            const Handle TheHandle,
            const Int32 &TheIndex,
            const TimeSliceTypeEnum TheType,
            const TimeSliceModeEnum TheMode,
            const Float64 TheTimeInterval,
            TimeSlice &theTimeSlice) :
            TimeSliceHandle (TheHandle),
            Index (TheIndex),
            active (False),
            type (TheType),
            mode (TheMode),
            timeInterval (TheTimeInterval),
            nextTimeSlice (0.0),
            timeSlice (theTimeSlice),
            continuous (False),
            system (False),
            next (0),
            prev (0) { update (); }

      void set_type (TimeSliceTypeEnum TheType) {

         type = TheType;
         update ();
      }

      void set_mode (TimeSliceModeEnum TheMode) {

         mode = TheMode;
         update ();
      }

      void set_interval (const Float64 TheInterval) {

         timeInterval = TheInterval;
         update ();
      }
   };

   class RuntimeContextTime : public RefCountDeleteOnZero {

      public:
         //! Which attribute to update enum.
         enum WhichEnum { Current, Factor, Frequency };

         struct updateStruct {

            const WhichEnum Which;
            const Float64 Value;
            updateStruct *next;

            updateStruct (const WhichEnum TheWhich, const Float64 TheValue) :
                  Which (TheWhich),
                  Value (TheValue),
                  next (0) {;}

            ~updateStruct () { if (next) { delete next; next = 0; } }
         };

         RuntimeContextTime ();

         void update_time_slice ();

         void set_current_time (const Float64 Value);
         void set_time_factor (const Float64 Value);
         void set_target_frequency (const Float64 Value);

         Int32 move_time_slice_to_end (const Handle TheHandle);

         TimeSliceStruct *create_time_slice_struct (
            const Handle TheHandle,
            const TimeSliceTypeEnum Type,
            const TimeSliceModeEnum Mode,
            const Float64 TimeInterval,
            TimeSlice &timeSlice);

         Boolean start_time_slice (TimeSliceStruct &timeSlice);
         Boolean stop_time_slice (TimeSliceStruct &timeSlice);
         Boolean remove_time_slice (TimeSliceStruct &timeSlice);

         Float64 currentTime; //!< Current frame time.
         Float64 previousTime; //!< Previous frame time.
         Float64 deltaTime; //!< Delta frame time.
         Float64 previousRealTime; //!< Previous frame system time.
         Float64 timeFactor; //!< Time factor.
         Float64 targetFrequency; //!< Target frequency.
         Float64 targetFrameLength; //!< Target frame length.

         Mutex lock; //!< Lock.
         updateStruct *head; //!< Head of update list.
         updateStruct *tail; //!< Tail of update list.

         Int32 timeSliceCount;
         HashTableHandleTemplate<Int32> timeSliceIndexTable;
         TimeSliceStruct *timeSliceHead;
         TimeSliceStruct *timeSliceNext;

      private:
         ~RuntimeContextTime ();

         void _add_update (updateStruct *ptr);
         void _update_time_slice (const Float64 RealTime, const Float64 RealDelta);
   };
};

#endif // DMZ_RUNTIME_CONTEXT_TIME_DOT_H
