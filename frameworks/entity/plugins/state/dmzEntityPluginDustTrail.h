#ifndef DMZ_ENTITY_PLUGIN_DUST_TRAIL_DOT_H
#define DMZ_ENTITY_PLUGIN_DUST_TRAIL_DOT_H

#include <dmzObjectObserverUtil.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzTypesMask.h>
#include <dmzTypesBase.h>

namespace dmz {

   class EntityPluginDustTrail :
         public Plugin,
         public ObjectObserverUtil {

      public:
         //! \cond
         EntityPluginDustTrail (const PluginInfo &Info, Config &local);
         ~EntityPluginDustTrail ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level);

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // Object Observer Interface
         virtual void destroy_object (const UUID &Identity, const Handle ObjectHandle);

         virtual void update_object_flag (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Boolean Value,
            const Boolean *PreviousValue);

         virtual void update_object_velocity (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Vector &Value,
            const Vector *PreviousValue);

      protected:
         void _init (Config &local);

         Log _log;

         Handle _hil;
         Handle _hilHandle;
         Handle _defaultObjectHandle;

         Mask _dustState;
         //! \endcond

      private:
         EntityPluginDustTrail ();
         EntityPluginDustTrail (const EntityPluginDustTrail &);
         EntityPluginDustTrail &operator= (const EntityPluginDustTrail &);

   };
};

#endif // DMZ_ENTITY_PLUGIN_DUST_TRAIL_DOT_H
