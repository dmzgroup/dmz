#ifndef DMZ_ENTITY_PLUGIN_RESTORE_HEALTH_DOT_H
#define DMZ_ENTITY_PLUGIN_RESTORE_HEALTH_DOT_H

#include <dmzObjectObserverUtil.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzTypesMask.h>

namespace dmz {

   class EntityPluginRestoreHealth :
         public Plugin,
         public ObjectObserverUtil {

      public:
         EntityPluginRestoreHealth (const PluginInfo &Info, Config &local);
         ~EntityPluginRestoreHealth ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level);

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // Object Observer Interface
         virtual void update_object_state (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Mask &Value,
            const Mask *PreviousValue);

      protected:
         void _init (Config &local);

         Log _log;

         Handle _defaultAttrHandle;
         Handle _healthAttrHandle;

         Mask _dead;

         Float64 _health;

      private:
         EntityPluginRestoreHealth ();
         EntityPluginRestoreHealth (const EntityPluginRestoreHealth &);
         EntityPluginRestoreHealth &operator= (const EntityPluginRestoreHealth &);

   };
};

#endif // DMZ_ENTITY_PLUGIN_RESTORE_HEALTH_DOT_H
