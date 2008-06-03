#ifndef DMZ_NET_EXT_PACKET_CODEC_OBJECT_BASIC_DOT_H
#define DMZ_NET_EXT_PACKET_CODEC_OBJECT_BASIC_DOT_H

#include <dmzNetExtPacketCodec.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimeTime.h>
#include <dmzTypesMask.h>
#include <dmzTypesHashTableHandleTemplate.h>

namespace dmz {

   class ObjectModule;
   class NetModuleAttributeMap;

   class NetExtPacketCodecObjectBasic :
         public Plugin,
         public NetExtPacketCodecObject {

      public:
         NetExtPacketCodecObjectBasic (const PluginInfo &Info, Config &local);
         ~NetExtPacketCodecObjectBasic ();

         // Plugin Interface
         virtual void discover_plugin (const Plugin *PluginPtr);
         virtual void start_plugin ();
         virtual void stop_plugin ();
         virtual void shutdown_plugin ();
         virtual void remove_plugin (const Plugin *PluginPtr);

         // NetExtPacketCodecObject Interface
         virtual Boolean decode (Unmarshal &data, Boolean &isLoopback);
         virtual Boolean encode_object (
            const Handle ObjectHandle,
            const NetObjectEncodeEnum EncodeMode,
            Marshal &data);

      protected:
         struct ScalarStruct {

            const Handle NetHandle;
            const Handle AttrHandle;
            const Handle LNVAttrHandle;

            ScalarStruct (
                  const Handle TheHandle,
                  const Handle &TheAttrHandle,
                  const Handle &TheLNVAttrHandle) :
                  NetHandle (TheHandle),
                  AttrHandle (TheAttrHandle),
                  LNVAttrHandle (TheLNVAttrHandle) {;}
         };

         void _init (Config &local);

         Log _log;

         Time _time;

         Handle _defaultHandle;
         Handle _lnvHandle;

         ObjectModule *_objMod;
         NetModuleAttributeMap *_attrMod;

         Mask _deactivateState;

         HashTableHandleTemplate<ScalarStruct> _scalarTable;

      private:
         NetExtPacketCodecObjectBasic ();
         NetExtPacketCodecObjectBasic (const NetExtPacketCodecObjectBasic &);
         NetExtPacketCodecObjectBasic &operator= (const NetExtPacketCodecObjectBasic &);

   };
};

#endif // DMZ_NET_EXT_PACKET_CODEC_OBJECT_BASIC_DOT_H
