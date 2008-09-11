#ifndef DMZ_NET_PLUGIN_PACKET_DOT_H
#define DMZ_NET_PLUGIN_PACKET_DOT_H

#include <dmzEventObserverUtil.h>
#include <dmzNetModuleLocalDR.h>
#include <dmzNetModulePacketCodec.h>
#include <dmzNetModulePacketIO.h>
#include <dmzObjectObserverUtil.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimeObjectType.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimeTimeSlice.h>
#include <dmzSystem.h>
#include <dmzSystemMarshal.h>
#include <dmzSystemUnmarshal.h>
#include <dmzTypesHashTableHandleTemplate.h>

namespace dmz {

   class NetPluginPacket :
         public Plugin,
         public TimeSlice,
         public NetPacketObserver,
         public EventObserverUtil,
         public ObjectObserverUtil {

      public:
         NetPluginPacket (
            const PluginInfo &Info,
            const ByteOrderEnum Endian,
            Config &local);

         ~NetPluginPacket ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level);

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // Plugin Interface
         virtual void update_time_slice (const Float64 TimeDelta);

         // NetPacketObserver Interface
         virtual void read_packet (const Int32 Size, char *buffer);

         // EventObserverUtil Interface
         virtual void close_event (
            const Handle EventHandle,
            const EventType &Type,
            const EventLocalityEnum Locality);

         // ObjectObserverUtil Interface
         virtual void create_object (
            const UUID &Identity,
            const Handle ObjectHandle,
            const ObjectType &Type,
            const ObjectLocalityEnum Locality);

         virtual void destroy_object (const UUID &Identity, const Handle ObjectHandle);

         virtual void update_object_locality (
            const UUID &Identity,
            const Handle ObjectHandle,
            const ObjectLocalityEnum Locality,
            const ObjectLocalityEnum PrevLocality);

      protected:
         struct ObjStruct {

            const Handle ObjectHandle;
            ObjectType type;

            ObjStruct (
               const Handle TheHandle,
               const ObjectType &TheType) :
               ObjectHandle (TheHandle),
               type (TheType) {;}
         };

         void _init (Config &local);

         Log _log;

        NetModuleLocalDR *_drMod;
        NetModulePacketCodec *_codecMod;
        NetModulePacketIO *_ioMod;

        Marshal _outData;
        Unmarshal _inData;

        HashTableHandleTemplate<ObjStruct> _objTable;
        HashTableHandleTemplate<ObjStruct> _preRegObjTable;

      private:
         NetPluginPacket ();
         NetPluginPacket (const NetPluginPacket &);
         NetPluginPacket &operator= (const NetPluginPacket &);

   };
};

#endif // DMZ_NET_PLUGIN_PACKET_DOT_H
