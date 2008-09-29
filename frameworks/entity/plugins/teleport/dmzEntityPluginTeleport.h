#ifndef DMZ_ENTITY_PLUGIN_TELEPORT_DOT_H
#define DMZ_ENTITY_PLUGIN_TELEPORT_DOT_H

#include <dmzInputObserverUtil.h>
#include <dmzObjectObserverUtil.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzTypesMatrix.h>
#include <dmzTypesVector.h>

namespace dmz {

   class EntityPluginTeleport :
         public Plugin,
         public InputObserverUtil,
         public ObjectObserverUtil {

      public:
         //! \cond
         EntityPluginTeleport (const PluginInfo &Info, Config &local);
         ~EntityPluginTeleport ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level) {;}

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr) {;}

         // Input Observer Interface
         virtual void update_channel_state (const Handle Channel, const Boolean State);

         virtual void receive_button_event (
            const Handle Channel,
            const InputEventButton &Value);

         virtual void receive_key_event (
            const Handle Channel,
            const InputEventKey &Value);

         // Object Observer Interface
         virtual void destroy_object (const UUID &Identity, const Handle ObjectHandle);

         virtual void update_object_flag (
            const UUID &Identity,
            const Handle ObjectHandle,
            const Handle AttributeHandle,
            const Boolean Value,
            const Boolean *PreviousValue);

      protected:
         void _init (Config &local);

         const Handle _ResetButton;

         Handle _handle;
         Handle _defaultHandle;
         Handle _hilHandle;

         Int32 _active;

         Vector _startPos;
         Matrix _startOri;

         Log _log;
         //! \endcond

      private:
         EntityPluginTeleport ();
         EntityPluginTeleport (const EntityPluginTeleport &);
         EntityPluginTeleport &operator= (const EntityPluginTeleport &);

   };
};

#endif // DMZ_ENTITY_PLUGIN_TELEPORT_DOT_H
