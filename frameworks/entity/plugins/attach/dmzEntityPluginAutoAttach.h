#ifndef DMZ_ENTITY_PLUGIN_AUTO_ATTACH_DOT_H
#define DMZ_ENTITY_PLUGIN_AUTO_ATTACH_DOT_H

#include <dmzInputObserverUtil.h>
#include <dmzObjectObserverUtil.h>
#include <dmzRuntimeDataConverterTypesBase.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimeMessaging.h>
#include <dmzRuntimeObjectType.h>
#include <dmzRuntimePlugin.h>
#include <dmzTypesHandleContainer.h>

namespace dmz {

   class EntityPluginAutoAttach :
         public Plugin,
         public MessageObserver,
         public InputObserverUtil,
         public ObjectObserverUtil {

      public:
         EntityPluginAutoAttach (const PluginInfo &Info, Config &local);
         ~EntityPluginAutoAttach ();

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

         // Input Observer Interface
         virtual void update_channel_state (const Handle Channel, const Boolean State);

         // Object Observer Interface
         virtual void create_object (
            const UUID &Identity,
            const Handle ObjectHandle,
            const ObjectType &Type,
            const ObjectLocalityEnum Locality);

         virtual void destroy_object (const UUID &Identity, const Handle ObjectHandle);

      protected:
         void _find_object ();
         void _init (Config &local);

         Log _log;
         DataConverterHandle _convert;

         Int32 _active;

         Message _nextMsg;
         Message _attachMsg;
         Handle _attached;
         ObjectTypeSet _types;
         HandleContainer _list;

      private:
         EntityPluginAutoAttach ();
         EntityPluginAutoAttach (const EntityPluginAutoAttach &);
         EntityPluginAutoAttach &operator= (const EntityPluginAutoAttach &);

   };
};

#endif // DMZ_ENTITY_PLUGIN_AUTO_ATTACH_DOT_H
