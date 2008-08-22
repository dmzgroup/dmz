#ifndef DMZ_NET_EXT_PACKET_CODEC_OBJECT_NATIVE_DOT_H
#define DMZ_NET_EXT_PACKET_CODEC_OBJECT_NATIVE_DOT_H

#include <dmzNetExtPacketCodec.h>
#include <dmzObjectModule.h>
#include <dmzNetModuleAttributeMap.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimeTime.h>
#include <dmzTypesUUID.h>

namespace dmz {

   class RuntimeContext;
   class NetModuleAttributeMap;

   class NetExtPacketCodecObjectNative :
         public Plugin,
         public NetExtPacketCodecObject {

      public:
         class ObjectAttributeAdapter {

            public:
               ObjectAttributeAdapter (Config &local, RuntimeContext *context);
               virtual ~ObjectAttributeAdapter ();

               virtual void discover_plugin (
                  const PluginDiscoverEnum Mode,
                  const Plugin *PluginPtr) {;}

               virtual void decode (
                  const Handle ObjectHandle,
                  Unmarshal &data,
                  ObjectModule &objMod) = 0;

               virtual void encode (
                  const Handle ObjectHandle,
                  ObjectModule &objMod,
                  Marshal &data) = 0;

               ObjectAttributeAdapter *next;

            protected:
               const Handle _AttributeHandle;
               const Handle _LNVHandle;
         };

         static ObjectAttributeAdapter *create_object_adapter (
            Config &local,
            RuntimeContext *context,
            Log &log);

         NetExtPacketCodecObjectNative (const PluginInfo &Info, Config &local);
         ~NetExtPacketCodecObjectNative ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level);

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // NetExtPacketCodecObject Interface
         virtual Boolean decode (Unmarshal &data, Boolean &isLoopback);

         virtual Boolean encode_object (
            const Handle ObjectHandle,
            const NetObjectEncodeEnum EncodeMode,
            Marshal &data);

      protected:
         void _init (Config &local);

         const UUID _SysID;
         Log _log;
         Time _time;

         Handle _defaultHandle;
         Handle _lnvHandle;

         ObjectModule *_objMod;
         NetModuleAttributeMap *_attrMod;
         ObjectAttributeAdapter *_adapterList;

      private:
         NetExtPacketCodecObjectNative ();
         NetExtPacketCodecObjectNative (const NetExtPacketCodecObjectNative &);
         NetExtPacketCodecObjectNative &operator= (const NetExtPacketCodecObjectNative &);

   };
};

#endif // DMZ_NET_EXT_PACKET_CODEC_OBJECT_NATIVE_DOT_H
