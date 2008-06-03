#ifndef DMZ_RUNTIME_CONTEXT_TIME_DOT_H
#define DMZ_RUNTIME_CONTEXT_TIME_DOT_H

#include <dmzRuntimeSync.h>
#include <dmzSystemMutex.h>
#include <dmzSystemRefCount.h>
#include <dmzTypesBase.h>
#include <dmzTypesHashTableHandleTemplate.h>

namespace dmz {

   struct SyncStruct {

      const Handle SyncHandle;
      const Int32 &Index;

      Boolean active;

      SyncTypeEnum type;
      SyncModeEnum mode;
      Float64 timeInterval;
      Float64 nextSync;
      Sync &sync;

      Boolean continuous;
      Boolean system;

      SyncStruct *next;
      SyncStruct *prev;

      void update ()  {

         continuous = (is_zero64 (timeInterval) && (mode == SyncModeRepeating));
         system = (type == SyncTypeSystemTime);
      }

      SyncStruct (
            const Handle TheHandle,
            const Int32 &TheIndex,
            const SyncTypeEnum TheType,
            const SyncModeEnum TheMode,
            const Float64 TheTimeInterval,
            Sync &theSync) :
            SyncHandle (TheHandle),
            Index (TheIndex),
            active (False),
            type (TheType),
            mode (TheMode),
            timeInterval (TheTimeInterval),
            nextSync (0.0),
            sync (theSync),
            continuous (False),
            system (False),
            next (0),
            prev (0) { update (); }

      void set_type (SyncTypeEnum TheType) {

         type = TheType;
         update ();
      }

      void set_mode (SyncModeEnum TheMode) {

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

         void sync ();

         void set_current_time (const Float64 Value);
         void set_time_factor (const Float64 Value);
         void set_target_frequency (const Float64 Value);

         Int32 move_sync_to_end (const Handle TheHandle);

         SyncStruct *create_sync_struct (
            const Handle TheHandle,
            const SyncTypeEnum Type,
            const SyncModeEnum Mode,
            const Float64 TimeInterval,
            Sync &sync);

         Boolean start_sync (SyncStruct &sync);
         Boolean stop_sync (SyncStruct &sync);
         Boolean remove_sync (SyncStruct &sync);

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

         Int32 syncCount;
         HashTableHandleTemplate<Int32> syncIndexTable;
         SyncStruct *syncHead;
         SyncStruct *syncNext;

      private:
         ~RuntimeContextTime ();

         void _add_update (updateStruct *ptr);
         void _update_sync (const Float64 RealTime, const Float64 RealDelta);
   };
};

#endif // DMZ_RUNTIME_CONTEXT_TIME_DOT_H
