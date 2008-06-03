#ifndef DMZ_NET_MODULE_PACKET_CODEC_DOT_H
#define DMZ_NET_MODULE_PACKET_CODEC_DOT_H

#include <dmzRuntimePlugin.h>
#include <dmzRuntimeRTTI.h>
#include <dmzTypesBase.h>

#define DMZ_NET_MODULE_PACKET_CODEC_INTERFACE_NAME "NetModulePacketCodecInterface"

namespace dmz {

   class Data;
   class EventType;
   class Marshal;
   class MessageType;
   class ObjectType;
   class Unmarshal;
   
   class NetModulePacketCodec {

      public:
         static NetModulePacketCodec *cast (
            const Plugin *PluginPtr,
            const String &PluginName = "");

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
      DMZ_NET_MODULE_PACKET_CODEC_INTERFACE_NAME,
      PluginName,
      PluginPtr);
}


inline
dmz::NetModulePacketCodec::NetModulePacketCodec (const PluginInfo &Info) :
      __Info (Info) {

   store_rtti_interface (
      DMZ_NET_MODULE_PACKET_CODEC_INTERFACE_NAME,
      __Info,
      (void *)this);
}


inline
dmz::NetModulePacketCodec::~NetModulePacketCodec () {

   remove_rtti_interface (DMZ_NET_MODULE_PACKET_CODEC_INTERFACE_NAME, __Info);
}

#endif //  DMZ_NET_MODULE_PACKET_CODEC_DOT_H
