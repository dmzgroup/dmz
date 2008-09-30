#ifndef DMZ_NET_EXT_PACKET_CODEC_EVENT_BASIC_DOT_H
#define DMZ_NET_EXT_PACKET_CODEC_EVENT_BASIC_DOT_H

#include <dmzNetExtPacketCodec.h>
#include <dmzRuntimeEventType.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimeTime.h>
#include <dmzTypesUUID.h>

namespace dmz {

   class EventModule;
   class NetModuleAttributeMap;
   class NetModuleIdentityMap;
   class ObjectModule;

   class NetExtPacketCodecEventBasic :
         public Plugin,
         public NetExtPacketCodecEvent {

      public:
         //! \cond
         NetExtPacketCodecEventBasic (const PluginInfo &Info, Config &local);
         ~NetExtPacketCodecEventBasic ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level) {;}

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // NetExtPacketCodecEvent Interface
         virtual Boolean decode (Unmarshal &data, Boolean &isLoopback);
         virtual Boolean encode_event (const Handle EventHandle, Marshal &data);

      protected:
         void _init (Config &local);

         const UUID _SysID;

         Log _log;

         Time _time;

         Handle _defaultHandle;
         Handle _sourceHandle;
         Handle _targetHandle;
         Handle _munitionsHandle;

         Handle _launchHandle;
         Handle _detonateHandle;
         Handle _collisionHandle;

         EventType _launchType;
         EventType _detonateType;
         EventType _collisionType;

         EventModule *_eventMod;
         NetModuleAttributeMap *_attrMod;
         ObjectModule *_objMod;
         //! \endcond

      private:
         NetExtPacketCodecEventBasic ();
         NetExtPacketCodecEventBasic (const NetExtPacketCodecEventBasic &);
         NetExtPacketCodecEventBasic &operator= (const NetExtPacketCodecEventBasic &);

   };
};

#endif // DMZ_NET_EXT_PACKET_CODEC_EVENT_BASIC_DOT_H

