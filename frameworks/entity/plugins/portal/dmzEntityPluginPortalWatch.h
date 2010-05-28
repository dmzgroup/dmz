#ifndef DMZ_ENTITY_PLUGIN_PORTAL_WATCH_DOT_H
#define DMZ_ENTITY_PLUGIN_PORTAL_WATCH_DOT_H

#include <dmzInputObserverUtil.h>
#include <dmzObjectObserverUtil.h>
#include <dmzRuntimeDataConverterTypesBase.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimeMessaging.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimeTimeSlice.h>

namespace dmz {

   class EntityModulePortal;

   class EntityPluginPortalWatch :
         public Plugin,
         public TimeSlice,
         public MessageObserver,
         public InputObserverUtil,
         public ObjectObserverUtil {

      public:
         //! \cond
         EntityPluginPortalWatch (const PluginInfo &Info, Config &local);
         ~EntityPluginPortalWatch ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level);

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // TimeSlice Interface
         virtual void update_time_slice (const Float64 TimeDelta);

         // Message Observer Interface
         virtual void receive_message (
            const Message &Type,
            const UInt32 MessageSendHandle,
            const Handle TargetObserverHandle,
            const Data *InData,
            Data *outData);

         // Input Observer Interface
         virtual void update_channel_state (const Handle Channel, const Boolean State);

         // Object Observer Interface
         virtual void update_object_flag (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Boolean Value,
            const Boolean *PreviousValue);

      protected:
         void _init (Config &local);

         Log _log;
         DataConverterHandle _convert;

         Message _targetMessage;

         EntityModulePortal *_portal;
         Handle _hil;
         Handle _target;
         Handle _defaultAttrHandle;

         Int32 _active;
         //! \endcond

      private:
         EntityPluginPortalWatch ();
         EntityPluginPortalWatch (const EntityPluginPortalWatch &);
         EntityPluginPortalWatch &operator= (const EntityPluginPortalWatch &);

   };
};

#endif // DMZ_ENTITY_PLUGIN_PORTAL_WATCH_DOT_H
