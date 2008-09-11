#ifndef DMZ_NET_EXT_PACKET_CODEC_OBJECT_NATIVE_DOT_H
#define DMZ_NET_EXT_PACKET_CODEC_OBJECT_NATIVE_DOT_H

#include <dmzNetExtPacketCodec.h>
#include <dmzEventModule.h>
#include <dmzNetModuleAttributeMap.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimeTime.h>
#include <dmzTypesUUID.h>

namespace dmz {

   class RuntimeContext;
   class NetModuleAttributeMap;

   class NetExtPacketCodecEventNative :
         public Plugin,
         public NetExtPacketCodecEvent {

      public:
         class EventAttributeAdapter {

            public:
               EventAttributeAdapter (Config &local, RuntimeContext *context);
               virtual ~EventAttributeAdapter ();

               virtual void discover_plugin (
                  const PluginDiscoverEnum Mode,
                  const Plugin *PluginPtr) {;}

               virtual void decode (
                  const Handle EventHandle,
                  Unmarshal &data,
                  EventModule &eventMod) = 0;

               virtual void encode (
                  const Handle EventHandle,
                  EventModule &eventMod,
                  Marshal &data) = 0;

               EventAttributeAdapter *next;

            protected:
               const Handle _AttributeHandle;
         };

         static EventAttributeAdapter *create_event_adapter (
            Config &local,
            RuntimeContext *context,
            Log &log);

         NetExtPacketCodecEventNative (const PluginInfo &Info, Config &local);
         ~NetExtPacketCodecEventNative ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level);

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // NetExtPacketCodecEvent Interface
         virtual Boolean decode (Unmarshal &data, Boolean &isLoopback);

         virtual Boolean encode_event (
            const Handle EventHandle,
            Marshal &data);

      protected:
         void _init (Config &local);

         const UUID _SysID;
         Log _log;
         Time _time;

         Handle _defaultHandle;
         Handle _lnvHandle;

         EventModule *_eventMod;
         NetModuleAttributeMap *_attrMod;
         EventAttributeAdapter *_adapterList;

      private:
         NetExtPacketCodecEventNative ();
         NetExtPacketCodecEventNative (const NetExtPacketCodecEventNative &);
         NetExtPacketCodecEventNative &operator= (const NetExtPacketCodecEventNative &);

   };
};

#endif // DMZ_NET_EXT_PACKET_CODEC_OBJECT_NATIVE_DOT_H
