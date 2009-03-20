#ifndef DMZ_ENTITY_PLUGIN_HIDE_DOT_H
#define DMZ_ENTITY_PLUGIN_HIDE_DOT_H

#include <dmzInputObserverUtil.h>
#include <dmzObjectObserverUtil.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>

namespace dmz {

   class EntityPluginHide :
         public Plugin,
         public InputObserverUtil,
         public ObjectObserverUtil {

      public:
         EntityPluginHide (const PluginInfo &Info, Config &local);
         ~EntityPluginHide ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level);

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

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
         void _update ();
         void _init (Config &local);

         Log _log;

         Int32 _active;
         Handle _hil;
         Handle _hideHandle;

      private:
         EntityPluginHide ();
         EntityPluginHide (const EntityPluginHide &);
         EntityPluginHide &operator= (const EntityPluginHide &);

   };
};

#endif // DMZ_ENTITY_PLUGIN_HIDE_DOT_H
