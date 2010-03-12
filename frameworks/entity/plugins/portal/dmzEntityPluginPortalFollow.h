#ifndef DMZ_ENTITY_PLUGIN_PORTAL_FOLLOW_DOT_H
#define DMZ_ENTITY_PLUGIN_PORTAL_FOLLOW_DOT_H

#include <dmzAudioModulePortal.h>
#include <dmzInputObserverUtil.h>
#include <dmzObjectModule.h>
#include <dmzObjectObserverUtil.h>
#include <dmzRenderModulePortal.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimeTimeSlice.h>
#include <dmzTypesMatrix.h>
#include <dmzTypesVector.h>

namespace dmz {

   class EntityPluginPortalFollow :
         public Plugin,
         public TimeSlice,
         public ObjectObserverUtil,
         public InputObserverUtil {

      public:
         //! \cond
         EntityPluginPortalFollow (const PluginInfo &Info, Config &local);
         ~EntityPluginPortalFollow ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level) {;}

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // TimeSlice Interface
         virtual void update_time_slice (const Float64 TimeDelta);

         // Object Observer Interface
         virtual void update_object_flag (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Boolean Value,
            const Boolean *PreviousValue);

         // Input Observer Interface
         virtual void update_channel_state (const Handle Channel, const Boolean State);

      protected:
         void _init (Config &local);

         RenderModulePortal *_renderPortal;
         AudioModulePortal *_audioPortal;
         Handle _handle;
         Handle _defaultHandle;
         Handle _hilHandle;

         Vector _lastHeadVec;
         Vector _lastPitchVec;

         Vector _offset;

         Int32 _active;

         Log _log;
         //! \endcond

      private:
         EntityPluginPortalFollow ();
         EntityPluginPortalFollow (const EntityPluginPortalFollow &);
         EntityPluginPortalFollow &operator= (const EntityPluginPortalFollow &);

   };
};

#endif // DMZ_ENTITY_PLUGIN_PORTAL_FOLLOW_DOT_H
