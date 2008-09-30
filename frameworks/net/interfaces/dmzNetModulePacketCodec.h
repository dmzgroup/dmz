#ifndef DMZ_NET_MODULE_PACKET_CODEC_DOT_H
#define DMZ_NET_MODULE_PACKET_CODEC_DOT_H

#include <dmzRuntimePlugin.h>
#include <dmzRuntimeRTTI.h>
#include <dmzTypesBase.h>

namespace dmz {

   //! \cond
   const char NetModulePacketCodecInterfaceName[] =  "NetModulePacketCodecInterface";
   //! \endcond

   class Data;
   class EventType;
   class EventTypeSet;
   class Marshal;
   class Message;
   class ObjectType;
   class ObjectTypeSet;
   class Unmarshal;

   class NetModulePacketCodec {

      public:
         static NetModulePacketCodec *cast (
            const Plugin *PluginPtr,
            const String &PluginName = "");

         virtual void get_supported_objects (ObjectTypeSet &objects) = 0;
         virtual void get_supported_events (EventTypeSet &events) = 0;

         virtual Boolean decode (Unmarshal &data, Boolean &isLoopback) = 0;

         virtual Boolean register_object (
            const Handle ObjectHandle,
            const ObjectType &Type,
            Marshal &outData) = 0;

         virtual Boolean encode_object (
            const Handle ObjectHandle,
            Marshal &outData) = 0;

         virtual Boolean release_object (
            const Handle ObjectHandle,
            Marshal &outData) = 0;

         virtual Boolean encode_event (
            const EventType &Type,
            const Handle EventHandle,
            Marshal &data) = 0;

      protected:
         NetModulePacketCodec (const PluginInfo &Info);
         ~NetModulePacketCodec ();

      private:
         const PluginInfo &__Info;
   };
};


inline dmz::NetModulePacketCodec *
dmz::NetModulePacketCodec::cast (const Plugin *PluginPtr, const String &PluginName) {

   return (NetModulePacketCodec *)lookup_rtti_interface (
      NetModulePacketCodecInterfaceName,
      PluginName,
      PluginPtr);
}


inline
dmz::NetModulePacketCodec::NetModulePacketCodec (const PluginInfo &Info) :
      __Info (Info) {

   store_rtti_interface (NetModulePacketCodecInterfaceName, __Info, (void *)this);
}


inline
dmz::NetModulePacketCodec::~NetModulePacketCodec () {

   remove_rtti_interface (NetModulePacketCodecInterfaceName, __Info);
}

#endif //  DMZ_NET_MODULE_PACKET_CODEC_DOT_H
