#ifndef DMZ_NET_EXT_PACKET_CODEC_DOT_H
#define DMZ_NET_EXT_PACKET_CODEC_DOT_H

#include <dmzRuntimePlugin.h>
#include <dmzRuntimeRTTI.h>
#include <dmzTypesBase.h>

#define DMZ_NET_EXT_PACKET_CODEC_OBJECT_INTERFACE_NAME "NetExtPacketCodecObjectInterface"
#define DMZ_NET_EXT_PACKET_CODEC_EVENT_INTERFACE_NAME "NetExtPacketCodecEventInterface"

namespace dmz {

   class Data;
   class Marshal;
   class Message;
   class Unmarshal;

   enum NetObjectEncodeEnum { NetObjectActivate, NetObjectUpdate, NetObjectDeactivate, };

   class NetExtPacketCodec {

      public:
         String get_net_ext_packet_codec_name () const;

         virtual Boolean decode (Unmarshal &data, Boolean &isLoopback) = 0;

      protected:
         NetExtPacketCodec (const PluginInfo &Info);
         ~NetExtPacketCodec () {;}

         const PluginInfo &_Info;
   };

   class NetExtPacketCodecObject : public NetExtPacketCodec {

      public:
         static NetExtPacketCodecObject *cast (
            const Plugin *PluginPtr,
            const String &PluginName = "");

         virtual Boolean encode_object (
            const Handle ObjectHandle,
            const NetObjectEncodeEnum EncodeMode,
            Marshal &data) = 0;

      protected:
         NetExtPacketCodecObject (const PluginInfo &Info);
         ~NetExtPacketCodecObject ();

   };

   class NetExtPacketCodecEvent : public NetExtPacketCodec {

      public:
         static NetExtPacketCodecEvent *cast (
           const Plugin *PluginPtr,
           const String &PluginName = "");

         virtual Boolean encode_event (const Handle EventHandle, Marshal &data) = 0;

      protected:
         NetExtPacketCodecEvent (const PluginInfo &Info);
         ~NetExtPacketCodecEvent ();
   };
};


inline
dmz::NetExtPacketCodec::NetExtPacketCodec (const PluginInfo &Info) : _Info (Info) {;}


inline dmz::String
dmz::NetExtPacketCodec::get_net_ext_packet_codec_name () const {

   return _Info.get_name ();
}


inline dmz::NetExtPacketCodecObject *
dmz::NetExtPacketCodecObject::cast (const Plugin *PluginPtr, const String &PluginName) {

   return (NetExtPacketCodecObject *)lookup_rtti_interface (
      DMZ_NET_EXT_PACKET_CODEC_OBJECT_INTERFACE_NAME,
      PluginName,
      PluginPtr);
}


inline
dmz::NetExtPacketCodecObject::NetExtPacketCodecObject (const PluginInfo &Info) :
      NetExtPacketCodec (Info) {

   store_rtti_interface (
      DMZ_NET_EXT_PACKET_CODEC_OBJECT_INTERFACE_NAME,
      _Info,
      (void *)this);
}


inline
dmz::NetExtPacketCodecObject::~NetExtPacketCodecObject () {

   remove_rtti_interface (DMZ_NET_EXT_PACKET_CODEC_OBJECT_INTERFACE_NAME, _Info);
}


inline dmz::NetExtPacketCodecEvent *
dmz::NetExtPacketCodecEvent::cast (const Plugin *PluginPtr, const String &PluginName) {

   return (NetExtPacketCodecEvent *)lookup_rtti_interface (
      DMZ_NET_EXT_PACKET_CODEC_EVENT_INTERFACE_NAME,
      PluginName,
      PluginPtr);
}


inline
dmz::NetExtPacketCodecEvent::NetExtPacketCodecEvent (const PluginInfo &Info) :
      NetExtPacketCodec (Info) {

   store_rtti_interface (
      DMZ_NET_EXT_PACKET_CODEC_EVENT_INTERFACE_NAME,
      _Info,
      (void *)this);
}


inline
dmz::NetExtPacketCodecEvent::~NetExtPacketCodecEvent () {

   remove_rtti_interface (DMZ_NET_EXT_PACKET_CODEC_EVENT_INTERFACE_NAME, _Info);
}

#endif //  DMZ_NET_EXT_PACKET_CODEC_DOT_H
