#ifndef DMZ_ENTITY_PLUGIN_PORTAL_ORBIT_DOT_H
#define DMZ_ENTITY_PLUGIN_PORTAL_ORBIT_DOT_H

#include <dmzInputObserverUtil.h>
#include <dmzObjectObserverUtil.h>
#include <dmzRuntimeDataConverterTypesBase.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimeMessaging.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimeTimeSlice.h>
#include <dmzTypesVector.h>

namespace dmz {

   class EntityModulePortal;

   class EntityPluginPortalOrbit :
         public Plugin,
         public TimeSlice,
         public MessageObserver,
         public InputObserverUtil,
         public ObjectObserverUtil {

      public:
         //! \cond
         EntityPluginPortalOrbit (const PluginInfo &Info, Config &local);
         ~EntityPluginPortalOrbit ();

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
            const Handle MessageSendHandle,
            const Handle TargetObserverHandle,
            const Data *InData,
            Data *outData);

         // Input Observer Interface
         virtual void update_channel_state (const Handle Channel, const Boolean State);

         virtual void receive_axis_event (
            const Handle Channel,
            const InputEventAxis &Value);

         virtual void receive_mouse_event (
            const Handle Channel,
            const InputEventMouse &Value);

         // Object Observer Interface
         virtual void update_object_flag (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Boolean Value,
            const Boolean *PreviousValue);

      protected:
         void _update_radius ();
         void _init (Config &local);

         Log _log;
         DataConverterHandle _convert;

         Int32 _active;
         Handle _hil;
         Handle _target;
         Handle _defaultAttrHandle;
         Handle _bvrAttrHandle;
         Handle _updateRadius;

         Float64 _heading;
         Float64 _pitch;
         Float64 _radius;

         EntityModulePortal *_portal;
         //! \endcond

      private:
         EntityPluginPortalOrbit ();
         EntityPluginPortalOrbit (const EntityPluginPortalOrbit &);
         EntityPluginPortalOrbit &operator= (const EntityPluginPortalOrbit &);

   };
};

#endif // DMZ_ENTITY_PLUGIN_PORTAL_ORBIT_DOT_H
