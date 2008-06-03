#ifndef DMZ_ENTITY_PLUGIN_HORN_DOT_H
#define DMZ_ENTITY_PLUGIN_HORN_DOT_H

#include <dmzInputObserverUtil.h>
#include <dmzObjectObserverUtil.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>

namespace dmz {

   class EntityPluginHorn :
         public Plugin,
         public InputObserverUtil,
         public ObjectObserverUtil {

      public:
         EntityPluginHorn (const PluginInfo &Info, Config &local);
         ~EntityPluginHorn ();

         // Plugin Interface
         virtual void discover_plugin (const Plugin *PluginPtr);
         virtual void start_plugin ();
         virtual void stop_plugin ();
         virtual void shutdown_plugin ();
         virtual void remove_plugin (const Plugin *PluginPtr);

         // Input Observer Interface
         virtual void update_channel_state (const Handle Channel, const Boolean State);

         virtual void receive_button_event (
            const Handle Channel,
            const InputEventButton &Value);

         virtual void receive_key_event (
            const Handle Channel,
            const InputEventKey &Value);

         // Object Observer Interface
         virtual void update_object_flag (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Boolean Value,
            const Boolean *PreviousValue);

      protected:
         void _init (Config &local);

         Handle _handle;
         Handle _defaultHandle;
         Handle _hilHandle;
         Mask _hornState;

         Log _log;

      private:
         EntityPluginHorn ();
         EntityPluginHorn (const EntityPluginHorn &);
         EntityPluginHorn &operator= (const EntityPluginHorn &);

   };
};

#endif // DMZ_ENTITY_PLUGIN_HORN_DOT_H
