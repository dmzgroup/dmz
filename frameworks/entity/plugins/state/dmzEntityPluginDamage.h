#ifndef DMZ_ENTITY_PLUGIN_DAMAGE_DOT_H
#define DMZ_ENTITY_PLUGIN_DAMAGE_DOT_H

#include <dmzEventObserverUtil.h>
#include <dmzObjectObserverUtil.h>
#include <dmzRuntimeEventType.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>

namespace dmz {

   class EntityPluginDamage :
         public Plugin,
         public ObjectObserverUtil,
         public EventObserverUtil {

      public:
         //! \cond
         EntityPluginDamage (const PluginInfo &Info, Config &local);
         ~EntityPluginDamage ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level) {;}

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr) {;}

         // Object Observer Interface
         virtual void destroy_object (const UUID &Identity, const Handle ObjectHandle);

         virtual void update_object_flag (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Boolean Value,
            const Boolean *PreviousValue);

         // Event Observer Interface
         virtual void close_event (
            const Handle EventHandle,
            const EventType &Type,
            const EventLocalityEnum Locality);

      protected:
         void _init (Config &local);

         Log _log;

         Handle _hil;
         Handle _hilHandle;
         Handle _defaultObjectHandle;
         Handle _targetHandle;

         Mask _deadState;

         EventType _detonationType;
         //! \endcond

      private:
         EntityPluginDamage ();
         EntityPluginDamage (const EntityPluginDamage &);
         EntityPluginDamage &operator= (const EntityPluginDamage &);

   };
};

#endif // DMZ_ENTITY_PLUGIN_DAMAGE_DOT_H
