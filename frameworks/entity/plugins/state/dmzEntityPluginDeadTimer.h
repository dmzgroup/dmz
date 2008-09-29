#ifndef DMZ_ENTITY_PLUGIN_DEAD_TIMER_DOT_H
#define DMZ_ENTITY_PLUGIN_DEAD_TIMER_DOT_H

#include <dmzObjectObserverUtil.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimeTimeSlice.h>

namespace dmz {

   class EntityPluginDeadTimer :
         public Plugin,
         public TimeSlice,
         public ObjectObserverUtil {

      public:
         //! \cond
         EntityPluginDeadTimer (const PluginInfo &Info, Config &local);
         ~EntityPluginDeadTimer ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level) {;}

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr) {;}

         // TimeSlice Interface
         virtual void update_time_slice (const Float64 TimeDelta);

         // Object Observer Interface
         virtual void destroy_object (const UUID &Identity, const Handle ObjectHandle);

         virtual void update_object_state (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Mask &Value,
            const Mask *PreviousValue);

         virtual void update_object_flag (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Boolean Value,
            const Boolean *PreviousValue);

      protected:
         void _init (Config &local);

         Log _log;

         Handle _hil;
         Handle _hilHandle;
         Handle _defaultHandle;

         Mask _deadState;
         //! \endcond

      private:
         EntityPluginDeadTimer ();
         EntityPluginDeadTimer (const EntityPluginDeadTimer &);
         EntityPluginDeadTimer &operator= (const EntityPluginDeadTimer &);

   };
};

#endif // DMZ_ENTITY_PLUGIN_DEAD_TIMER_DOT_H
