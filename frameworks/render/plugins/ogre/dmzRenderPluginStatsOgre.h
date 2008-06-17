#ifndef DMZ_RENDER_PLUGIN_STATS_OGRE_DOT_H
#define DMZ_RENDER_PLUGIN_STATS_OGRE_DOT_H

#include <dmzInputObserverUtil.h>
#include <dmzRenderModuleCoreOgre.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimeTimeSlice.h>

namespace Ogre { class Overlay; class OverlayElement; }

namespace dmz {

   class RenderModuleCoreOgre;

   class RenderPluginStatsOgre :
         public Plugin,
         public TimeSlice,
         public InputObserverUtil {

      public:
         RenderPluginStatsOgre (const PluginInfo &Info, Config &local);
         ~RenderPluginStatsOgre ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level) {;}

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);
         
         // TimeSlice Interface
         virtual void update_time_slice (const Float64 TimeDelta);

         // Input Observer Interface
         virtual void update_channel_state (const UInt32 Channel, const Boolean State);

         virtual void receive_axis_event (
            const UInt32 Channel,
            const InputEventAxis &Value) {;}

         virtual void receive_button_event (
            const UInt32 Channel,
            const InputEventButton &Value) {;}

         virtual void receive_key_event (
            const UInt32 Channel,
            const InputEventKey &Value);

         virtual void receive_mouse_event (
            const UInt32 Channel,
            const InputEventMouse &Value) {;}

         virtual void receive_switch_event (
            const UInt32 Channel,
            const InputEventSwitch &Value) {;}

      protected:
         void _init (Config &local);
         void _update_stats ();
         
         String _portalName;

         RenderModuleCoreOgre *_core;
         Ogre::Overlay *_debugOverlay;
         Ogre::OverlayElement *_statsOverlay;

         Log _log;

      private:
         RenderPluginStatsOgre ();
         RenderPluginStatsOgre (const RenderPluginStatsOgre &);
         RenderPluginStatsOgre &operator= (const RenderPluginStatsOgre &);

   };
};

#endif // DMZ_RENDER_PLUGIN_STATS_OGRE_DOT_H
