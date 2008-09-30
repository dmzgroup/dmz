#ifndef DMZ_NET_EXT_PACKET_CODEC_OBJECT_BASIC_DOT_H
#define DMZ_NET_EXT_PACKET_CODEC_OBJECT_BASIC_DOT_H

#include <dmzNetExtPacketCodec.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimeTime.h>
#include <dmzTypesMask.h>
#include <dmzTypesHashTableHandleTemplate.h>
#include <dmzTypesUUID.h>

namespace dmz {

   class ObjectModule;
   class NetModuleAttributeMap;

   class NetExtPacketCodecObjectBasic :
         public Plugin,
         public NetExtPacketCodecObject {

      public:
         //! \cond
         NetExtPacketCodecObjectBasic (const PluginInfo &Info, Config &local);
         ~NetExtPacketCodecObjectBasic ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level) {;}

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

         const UUID _SysID;
         Log _log;

         Time _time;

         Handle _defaultHandle;
         Handle _lnvHandle;

         ObjectModule *_objMod;
         NetModuleAttributeMap *_attrMod;

         Mask _deactivateState;

         HashTableHandleTemplate<ScalarStruct> _scalarTable;
         //! \endcond

      private:
         NetExtPacketCodecObjectBasic ();
         NetExtPacketCodecObjectBasic (const NetExtPacketCodecObjectBasic &);
         NetExtPacketCodecObjectBasic &operator= (const NetExtPacketCodecObjectBasic &);

   };
};

#endif // DMZ_NET_EXT_PACKET_CODEC_OBJECT_BASIC_DOT_H
