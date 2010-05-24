#ifndef DMZ_ENTITY_PLUGIN_DETACH_DOT_H
#define DMZ_ENTITY_PLUGIN_DETACH_DOT_H

#include <dmzObjectObserverUtil.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimeMessaging.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimeDataConverterTypesBase.h>

namespace dmz {

   class EntityPluginDetach :
         public Plugin,
         public MessageObserver,
         public ObjectObserverUtil {

      public:
         EntityPluginDetach (const PluginInfo &Info, Config &local);
         ~EntityPluginDetach ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level);

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // Message Observer Interface
         virtual void receive_message (
            const Message &Type,
            const UInt32 MessageSendHandle,
            const Handle TargetObserverHandle,
            const Data *InData,
            Data *outData);

         // Object Observer Interface
         virtual void destroy_object (const UUID &Identity, const Handle ObjectHandle);

      protected:
         void _init (Config &local);

         Log _log;
         DataConverterHandle _convert;
         Handle _attached;

         Message _attachMsg;

      private:
         EntityPluginDetach ();
         EntityPluginDetach (const EntityPluginDetach &);
         EntityPluginDetach &operator= (const EntityPluginDetach &);

   };
};

#endif // DMZ_ENTITY_PLUGIN_DETACH_DOT_H
