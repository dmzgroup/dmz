#ifndef DMZ_RENDER_PLUGIN_MODE_OGRE_DOT_H
#define DMZ_RENDER_PLUGIN_MODE_OGRE_DOT_H

#include <dmzInputObserverUtil.h>
#include <dmzRenderModuleCoreOgre.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>

namespace Ogre { class Camera; }

namespace dmz {

   class RenderModuleCoreOgre;

   class RenderPluginModeOgre :
         public Plugin,
         public InputObserverUtil {

      public:
         RenderPluginModeOgre (const PluginInfo &Info, Config &local);
         ~RenderPluginModeOgre ();

         // Plugin Interface
         virtual void discover_plugin (const Plugin *PluginPtr);
         virtual void start_plugin ();
         virtual void stop_plugin ();
         virtual void shutdown_plugin ();
         virtual void remove_plugin (const Plugin *PluginPtr);

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

         String _name;

         RenderModuleCoreOgre *_core;
         Ogre::Camera *_camera;

         Log _log;

      private:
         RenderPluginModeOgre ();
         RenderPluginModeOgre (const RenderPluginModeOgre &);
         RenderPluginModeOgre &operator= (const RenderPluginModeOgre &);

   };
};

#endif // DMZ_RENDER_PLUGIN_MODE_OGRE_DOT_H
