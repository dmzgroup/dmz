#include "dmzRuntimeContextTime.h"
#include <dmzRuntimeTime.h>
#include <dmzSystem.h>
#include <dmzSystemStreamFile.h>
#include <math.h> // for modf

static const dmz::Float64 LocalMinFrequency (0.0001);

//! Constructor.
dmz::RuntimeContextTime::RuntimeContextTime () :
      firstUpdate (True),
      currentTime (0.0),
      previousTime (0.0),
      deltaTime (0.0),
      previousRealTime (get_time ()),
      timeFactor (1.0),
      targetFrequency (0.0),
      targetFrameLength (0.0),
      head (0),
      tail (0),
      timeSliceCount (0),
      timeSliceHead (0),
      timeSliceNext (0) {;}

//! Destructor.
dmz::RuntimeContextTime::~RuntimeContextTime () {

   if (head) { delete head; head = tail = 0; }

   timeSliceHead = 0;
   timeSliceNext = 0;
   timeSliceIndexTable.empty ();
}


//! Updates runtime time and updates TimeSlice derived classes.
void
dmz::RuntimeContextTime::update_time_slice () {

   updateStruct *top = head;
   Boolean currentTimeUpdated (False);

   if (top) {

      lock.lock ();
      head = tail = 0;
      lock.unlock ();

      updateStruct *tmp = top;

      while (tmp) {

         if (tmp->Which == Current) {

            currentTime = tmp->Value;
            currentTimeUpdated = True;
         }
         else if (tmp->Which == Factor) { timeFactor = tmp->Value; }
         else if (tmp->Which == Frequency) {

            targetFrequency = tmp->Value;

            if (targetFrequency > LocalMinFrequency) {

               targetFrameLength = 1.0 / targetFrequency;
            }
            else { targetFrameLength = 0.0; }
         }

         tmp = tmp->next;
      }

      delete top; top = 0;
   }

   Float64 sleepDelta (0.0);
   const Float64 EndFrameTime (get_time ());

   if (targetFrequency > LocalMinFrequency) {

      sleepDelta  = targetFrameLength - (EndFrameTime - previousRealTime);
      sleep (sleepDelta); // - 0.00007);
   }

   const Float64 StartFrameTime (get_time ());

   Float64 realDeltaTime (targetFrameLength);

   // First update prevents the first frame from being too large.
   if (!firstUpdate) {

      if (currentTimeUpdated) { realDeltaTime = currentTime - previousTime; }
      else {

         realDeltaTime = StartFrameTime - previousRealTime;
         deltaTime = realDeltaTime * timeFactor;
         previousTime = currentTime;
         currentTime += deltaTime;
      }
   }
   else { firstUpdate = False; }

   _update_time_slice (StartFrameTime, realDeltaTime);

   previousRealTime = StartFrameTime;
}


//! Sets current runtime time.
void
dmz::RuntimeContextTime::set_current_time (const Float64 Value) {

   _add_update (new updateStruct (Current, Value));
}


//! Sets time factor.
void
dmz::RuntimeContextTime::set_time_factor (const Float64 Value) {

   _add_update (new updateStruct (Factor, Value));
}


//! Sets target frequency.
void
dmz::RuntimeContextTime::set_target_frequency (const Float64 Value) {

   _add_update (new updateStruct (Frequency, Value));
}


dmz::Int32
dmz::RuntimeContextTime::move_time_slice_to_end (const Handle TheHandle) {

   Int32 result (0);

   Int32 *indexPtr = timeSliceIndexTable.lookup (TheHandle);

   if (!indexPtr) {

      indexPtr = new Int32 (timeSliceCount++);

      if (!timeSliceIndexTable.store (TheHandle, indexPtr)) {

         delete indexPtr; indexPtr = 0;
      }
   }
   else {

      *indexPtr = timeSliceCount++;
      TimeSliceStruct *found (0);
      TimeSliceStruct *current (timeSliceHead);

      while (current && !found) {

         if (current->TimeSliceHandle == TheHandle) { found = current; }
         current = current->next;
      }

      if (found) { remove_time_slice (*found); start_time_slice (*found); }
   }

   if (indexPtr) { result = *indexPtr; }

   return result;
}


dmz::TimeSliceStruct *
dmz::RuntimeContextTime::create_time_slice_struct (
      const Handle TheHandle,
      const TimeSliceTypeEnum Type,
      const TimeSliceModeEnum Mode,
      const Float64 TimeInterval,
      TimeSlice &timeSlice) {

   TimeSliceStruct *result (0);

   Int32 *indexPtr = timeSliceIndexTable.lookup (TheHandle);

   if (!indexPtr) {

      indexPtr = new Int32 (timeSliceCount++);

      if (!timeSliceIndexTable.store (TheHandle, indexPtr)) {

         delete indexPtr; indexPtr = 0;
      }
   }

   if (indexPtr) {

      result = new TimeSliceStruct (
         TheHandle,
         *indexPtr,
         Type,
         Mode,
         TimeInterval,
         timeSlice);

      if (result && (result->mode == TimeSliceModeRepeating)) {

         start_time_slice (*result);
      }
   }

   return result;
}


dmz::Boolean
dmz::RuntimeContextTime::start_time_slice (TimeSliceStruct &timeSlice) {

   if (!timeSlice.next && !timeSlice.prev && (timeSliceHead != &timeSlice)) {

      if (!timeSliceHead) { timeSliceHead = &timeSlice; }
      else {

         TimeSliceStruct *current (timeSliceHead);

         while (current) {

            if (current->Index > timeSlice.Index) {

               if (!current->prev) { timeSlice.prev = 0; timeSliceHead = &timeSlice; }
               else {

                  current->prev->next = &timeSlice;
                  timeSlice.prev = current->prev;
               }

               timeSlice.next = current;
               current->prev = &timeSlice;
               current = 0;
            }
            else {

               if (!current->next) {

                  current->next = &timeSlice;
                  timeSlice.prev = current;
                  timeSlice.next = 0;
                  current = 0;
               }
               else { current = current->next; }
            }
         }
      }
   }

   timeSlice.active = True;

   if (!timeSlice.continuous) {

      timeSlice.nextTimeSlice =
         timeSlice.timeInterval + (timeSlice.system ? get_time () : currentTime);
   }

   return True;
}


dmz::Boolean
dmz::RuntimeContextTime::stop_time_slice (TimeSliceStruct &timeSlice) {

   timeSlice.active = False;

   return True;
}


dmz::Boolean
dmz::RuntimeContextTime::remove_time_slice (TimeSliceStruct &timeSlice) {

   stop_time_slice (timeSlice);

   if (&timeSlice == timeSliceNext) { timeSliceNext = timeSlice.next; }

   if (timeSlice.next) { timeSlice.next->prev = timeSlice.prev; }
   if (timeSlice.prev) { timeSlice.prev->next = timeSlice.next; }

   if (&timeSlice == timeSliceHead) { timeSliceHead = timeSlice.next; }

   timeSlice.next = timeSlice.prev = 0;

   return True;
}


//! Adds update struct to end of list.
void
dmz::RuntimeContextTime::_add_update (updateStruct *ptr) {

   if (ptr) {

      lock.lock ();
      if (!tail) { head = tail = ptr; }
      else { tail->next = ptr; tail = ptr; }
      lock.unlock ();
   }
}


void
dmz::RuntimeContextTime::_update_time_slice (
      const Float64 RealTime,
      const Float64 RealDelta) {

   if (timeSliceHead) {

      TimeSliceStruct *current = timeSliceHead;

      while (current) {

         timeSliceNext = current->next;

         if (current->active) {

            if (current->continuous) {

               current->timeSlice.update_time_slice (
                  current->system ? RealDelta : deltaTime);

               if (current->mode == TimeSliceModeSingle) { stop_time_slice (*current); }
            }
            else {

               const Float64 TheTime (current->system ? RealTime : currentTime);

               if (current->nextTimeSlice <= TheTime) {

                  const Float64 TempDelta (
                     current->timeInterval + (TheTime - current->nextTimeSlice));

                  if (current->mode == TimeSliceModeSingle) {

                     stop_time_slice (*current);
                  }
                  else {

                     current->nextTimeSlice += current->timeInterval;

                     if (current->nextTimeSlice <= TheTime) {

                        if (current->timeInterval > 0.0) {

                           double multiplier (0.0);

                           modf (
                              (TheTime - current->nextTimeSlice) / current->timeInterval,
                              &multiplier);

                           current->nextTimeSlice +=
                              (multiplier + 1.0) * current->timeInterval;
                        }
                     }
                  }

                  // This must be the last usage of current because it may be deleted in
                  // the update_time_slice call
                  current->timeSlice.update_time_slice (TempDelta);
               }
            }
         }

         current = timeSliceNext;
      }
   }
}
