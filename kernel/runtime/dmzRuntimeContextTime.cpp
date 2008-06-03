#include "dmzRuntimeContextTime.h"
#include <dmzRuntimeTime.h>
#include <dmzSystem.h>
#include <dmzSystemStreamFile.h>
#include <math.h> // for modf

static const dmz::Float64 LocalMinFrequency (0.0001);

//! Constructor.
dmz::RuntimeContextTime::RuntimeContextTime () :
      currentTime (0.0),
      previousTime (0.0),
      deltaTime (0.0),
      previousRealTime (get_time ()),
      timeFactor (1.0),
      targetFrequency (0.0),
      targetFrameLength (0.0),
      head (0),
      tail (0),
      syncCount (0),
      syncHead (0),
      syncNext (0) {;}

//! Destructor.
dmz::RuntimeContextTime::~RuntimeContextTime () {

   if (head) { delete head; head = tail = 0; }

   syncHead = 0;
   syncNext = 0;
   syncIndexTable.empty ();
}


//! Updates runtime time and updates Sync derived classes.
void
dmz::RuntimeContextTime::sync () {

   const Float64 RealTime (get_time ());
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

   Float64 realDeltaTime (0.0);

   if (currentTimeUpdated) { realDeltaTime = currentTime - previousTime; }
   else {

      realDeltaTime = StartFrameTime - previousRealTime;
      deltaTime = realDeltaTime * timeFactor;
      previousTime = currentTime;
      currentTime += deltaTime;
   }

   _update_sync (StartFrameTime, realDeltaTime);

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
dmz::RuntimeContextTime::move_sync_to_end (const Handle TheHandle) {

   Int32 result (0);

   Int32 *indexPtr = syncIndexTable.lookup (TheHandle);

   if (!indexPtr) {

      indexPtr = new Int32 (syncCount++);

      if (!syncIndexTable.store (TheHandle, indexPtr)) {

         delete indexPtr; indexPtr = 0;
      }
   }
   else {

      *indexPtr = syncCount++;
      SyncStruct *found (0);
      SyncStruct *current (syncHead);

      while (current && !found) {

         if (current->SyncHandle == TheHandle) { found = current; }
         current = current->next;
      }

      if (found) { remove_sync (*found); start_sync (*found); }
   }

   if (indexPtr) { result = *indexPtr; }

   return result;
}


dmz::SyncStruct *
dmz::RuntimeContextTime::create_sync_struct (
      const Handle TheHandle,
      const SyncTypeEnum Type,
      const SyncModeEnum Mode,
      const Float64 TimeInterval,
      Sync &sync) {

   SyncStruct *result (0);

   Int32 *indexPtr = syncIndexTable.lookup (TheHandle);

   if (!indexPtr) {

      indexPtr = new Int32 (syncCount++);

      if (!syncIndexTable.store (TheHandle, indexPtr)) {

         delete indexPtr; indexPtr = 0;
      }
   }

   if (indexPtr) {

      result = new SyncStruct (
         TheHandle,
         *indexPtr,
         Type,
         Mode,
         TimeInterval,
         sync);

      if (result && (result->mode == SyncModeRepeating)) { start_sync (*result); }
   }

   return result;
}


dmz::Boolean
dmz::RuntimeContextTime::start_sync (SyncStruct &sync) {

   if (!sync.next && !sync.prev && (syncHead != &sync)) {

      if (!syncHead) { syncHead = &sync; }
      else {

         SyncStruct *current (syncHead);

         while (current) {

            if (current->Index > sync.Index) {

               if (!current->prev) { sync.prev = 0; syncHead = &sync; }
               else {

                  current->prev->next = &sync;
                  sync.prev = current->prev;
               }

               sync.next = current;
               current->prev = &sync;
               current = 0;
            }
            else {

               if (!current->next) {

                  current->next = &sync;
                  sync.prev = current;
                  sync.next = 0;
                  current = 0;
               }
               else { current = current->next; }
            }
         }
      }
   }

   sync.active = True;

   if (!sync.continuous) {

      sync.nextSync = sync.timeInterval + (sync.system ? get_time () : currentTime);
   }

   return True;
}


dmz::Boolean
dmz::RuntimeContextTime::stop_sync (SyncStruct &sync) {

   sync.active = False;

   return True;
}


dmz::Boolean
dmz::RuntimeContextTime::remove_sync (SyncStruct &sync) {

   stop_sync (sync);

   if (&sync == syncNext) { syncNext = sync.next; }

   if (sync.next) { sync.next->prev = sync.prev; }
   if (sync.prev) { sync.prev->next = sync.next; }

   if (&sync == syncHead) { syncHead = sync.next; }

   sync.next = sync.prev = 0;

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
dmz::RuntimeContextTime::_update_sync (const Float64 RealTime, const Float64 RealDelta) {

   if (syncHead) {

      SyncStruct *current = syncHead;

      while (current) {

         syncNext = current->next;

         if (current->active) {

            if (current->continuous) {

               current->sync.update_sync (current->system ? RealDelta : deltaTime);
               if (current->mode == SyncModeSingle) { stop_sync (*current); }
            }
            else {

               const Float64 TheTime (current->system ? RealTime : currentTime);

               if (current->nextSync <= TheTime) {

                  const Float64 TempDelta (
                     current->timeInterval + (TheTime - current->nextSync));

                  if (current->mode == SyncModeSingle) { stop_sync (*current); }
                  else {

                     current->nextSync += current->timeInterval;

                     if (current->nextSync <= TheTime) {

                        if (current->timeInterval > 0.0) {

                           double multiplier (0.0);

                           modf (
                              (TheTime - current->nextSync) / current->timeInterval,
                              &multiplier);

                           current->nextSync +=
                              (multiplier + 1.0) * current->timeInterval;
                        }
                     }
                  }

                  // This must be the last usage of current because it may be deleted in
                  // the update_sync call
                  current->sync.update_sync (TempDelta);
               }
            }
         }

         current = syncNext;
      }
   }
}
