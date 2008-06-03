#ifndef DMZ_NET_EXT_PACKET_CODEC_EVENT_BASIC_DOT_H
#define DMZ_NET_EXT_PACKET_CODEC_EVENT_BASIC_DOT_H

#include <dmzNetExtPacketCodec.h>
#include <dmzRuntimeEventType.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimeTime.h>

namespace dmz {

   class EventModule;
   class NetModuleAttributeMap;
   class NetModuleIdentityMap;

   class NetExtPacketCodecEventBasic :
         public Plugin,
         public NetExtPacketCodecEvent {

      public:
         NetExtPacketCodecEventBasic (const PluginInfo &Info, Config &local);
         ~NetExtPacketCodecEventBasic ();

         // Plugin Interface
         virtual void discover_plugin (const Plugin *PluginPtr);
         virtual void start_plugin ();
         virtual void stop_plugin ();
         virtual void shutdown_plugin ();
         virtual void remove_plugin (const Plugin *PluginPtr);

         // NetExtPacketCodecEvent Interface
         virtual Boolean decode (Unmarshal &data, Boolean &isLoopback);
         virtual Boolean encode_event (const Handle EventHandle, Marshal &data);

      protected:
         void _init (Config &local);

         Log _log;

         Time _time;

         Handle _defaultHandle;
         Handle _sourceHandle;
         Handle _targetHandle;
         Handle _munitionHandle;

         Handle _fireHandle;
         Handle _detonateHandle;
         Handle _collisionHandle;

         EventType _fireType;
         EventType _detonateType;
         EventType _collisionType;

         EventModule *_eventMod;
         NetModuleAttributeMap *_attrMod;
         NetModuleIdentityMap *_idMod;

      private:
         NetExtPacketCodecEventBasic ();
         NetExtPacketCodecEventBasic (const NetExtPacketCodecEventBasic &);
         NetExtPacketCodecEventBasic &operator= (const NetExtPacketCodecEventBasic &);

   };
};

#endif // DMZ_NET_EXT_PACKET_CODEC_EVENT_BASIC_DOT_H

