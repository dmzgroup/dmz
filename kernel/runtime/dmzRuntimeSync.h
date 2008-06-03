#ifndef DMZ_RUNTIME_SYNC_DOT_H
#define DMZ_RUNTIME_SYNC_DOT_H

#include <dmzKernelExport.h>
#include <dmzTypesBase.h>

namespace dmz {

   class RuntimeContext;
   class PluginInfo;

   enum SyncTypeEnum {
      SyncTypeSystemTime, //!< Uses system time.
      SyncTypeRuntime, //!< Uses runtime time.
   };

   enum SyncModeEnum {
      SyncModeRepeating, //!< Repeating sync.
      SyncModeSingle, //!< Single sync.
   };

   DMZ_KERNEL_LINK_SYMBOL Boolean
   reserve_sync_place (const Handle TheHandle, RuntimeContext *context);

   class DMZ_KERNEL_LINK_SYMBOL Sync {

      public:
         Handle get_sync_handle () const;

         void set_sync_type (const SyncTypeEnum Type);
         SyncTypeEnum get_sync_type () const;

         void set_sync_mode (const SyncModeEnum Mode);
         SyncModeEnum get_sync_mode () const;

         void set_sync_interval (const Float64 TimeInterval);
         Float64 get_sync_interval () const;

         void start_sync ();
         void stop_sync ();
         void remove_sync ();

         virtual void update_sync (const Float64 DeltaTime) = 0;

      protected:
         Sync (const Handle SyncHandle, RuntimeContext *context);

         Sync (
            const Handle SyncHandle,
            const SyncTypeEnum Type,
            const SyncModeEnum Mode,
            const Float64 TimeInterval,
            RuntimeContext *context);

         Sync (const PluginInfo &Info);

         Sync (
            const PluginInfo &Info,
            const SyncTypeEnum Type,
            const SyncModeEnum Mode,
            const Float64 TimeInterval);

         ~Sync ();

      private:
         struct State;
         Sync ();
         Sync (const Sync &);
         Sync &operator= (const Sync &);
         State &__state;
   };
};

#endif // DMZ_RUNTIME_SYNC_DOT_H
