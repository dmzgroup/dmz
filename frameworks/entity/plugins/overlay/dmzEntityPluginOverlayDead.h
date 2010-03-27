#ifndef DMZ_ENTITY_PLUGIN_OVERLAY_DEAD_DOT_H
#define DMZ_ENTITY_PLUGIN_OVERLAY_DEAD_DOT_H

#include <dmzObjectObserverUtil.h>
#include <dmzRenderPortalSize.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimeTimeSlice.h>
#include <dmzTypesMask.h>

namespace dmz {

   class RenderModuleOverlay;

   class EntityPluginOverlayDead :
         public Plugin,
         public TimeSlice,
         public ObjectObserverUtil,
         public PortalSizeObserver {

      public:
         //! \cond
         EntityPluginOverlayDead (const PluginInfo &Info, Config &local);
         ~EntityPluginOverlayDead ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level);

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // Time Slice Interface
         virtual void update_time_slice (const Float64 DeltaTime);

         // Object Observer Interface
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

         // Render Portal Observer Interface
         virtual void update_portal_size (
            const Handle PortalHandle,
            const Int32 ValueX,
            const Int32 ValueY);

      protected:
         void _init (Config &local);

         Log _log;

         RenderModuleOverlay *_overlay;

         Handle _hil;
         Handle _hilAttrHandle;
         String _overlaySwitchName;
         Handle _overlaySwitch;
         String _overlayScaleName;
         Handle _overlayScale;
         Mask _deadState;

         Float64 _currentRed;
         Float64 _currentAlpha;
         //! \endcond

      private:
         EntityPluginOverlayDead ();
         EntityPluginOverlayDead (const EntityPluginOverlayDead &);
         EntityPluginOverlayDead &operator= (const EntityPluginOverlayDead &);

   };
};

#endif // DMZ_ENTITY_PLUGIN_OVERLAY_DEAD_DOT_H
