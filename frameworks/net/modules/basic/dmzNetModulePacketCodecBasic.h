#ifndef DMZ_NET_MODULE_PACKET_CODEC_BASIC_DOT_H
#define DMZ_NET_MODULE_PACKET_CODEC_BASIC_DOT_H

#include <dmzNetExtPacketCodec.h>
#include <dmzNetModulePacketCodec.h>
#include <dmzRuntimeEventType.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimeObjectType.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimePluginContainer.h>
#include <dmzTypesHashTableStringTemplate.h>
#include <dmzTypesHashTableHandleTemplate.h>

namespace dmz {

   class NetModulePacketCodecBasic :
         public Plugin,
         public NetModulePacketCodec {

      public:
         class HeaderElement {

            public:
               HeaderElement *next;
               virtual ~HeaderElement () { if (next) { delete next; next = 0; } }
               Boolean read_header (Unmarshal &data, Handle &handle);
               Boolean write_header (const Handle PacketHandle, Marshal &data);
               virtual Boolean read_element (Unmarshal &data, Handle &packetHandle) = 0;
               virtual Boolean write_element (
                  const Handle PacketHandle,
                  Marshal &data) = 0;

            protected:
               HeaderElement () : next (0) {;}

            private:
               HeaderElement (const HeaderElement &);
               HeaderElement &operator= (const HeaderElement &);
         };

         NetModulePacketCodecBasic (
            const PluginInfo &Info,
            Config &local,
            Config &global);
         ~NetModulePacketCodecBasic ();

         // Plugin Interface
         virtual void discover_plugin (const Plugin *PluginPtr);
         virtual void start_plugin ();
         virtual void stop_plugin ();
         virtual void shutdown_plugin ();
         virtual void remove_plugin (const Plugin *PluginPtr);

         // NetModulePacketCodec Interface
         virtual Boolean decode (Unmarshal &inData, Boolean &isLoopback);

         virtual Boolean register_object (
            const Handle ObjectHandle,
            const ObjectType &Type,
            Marshal &outData);

         virtual Boolean encode_object (
            const Handle ObjectHandle,
            Marshal &outData);

         virtual Boolean release_object (
            const Handle ObjectHandle,
            Marshal &outData);

         virtual Boolean encode_event (
            const EventType &Type,
            const Handle EventHandle,
            Marshal &outData);

      protected:
         struct PacketStruct {

            const String ExtName;
            const Handle PacketHandle;
            ObjectTypeSet objects;
            EventTypeSet events;
            PacketStruct (const String &TheName, const Handle TheHandle) :
               ExtName (TheName),
               PacketHandle (TheHandle) {;}

            ~PacketStruct () {;}
         };

         struct DecodeStruct {

            const Handle PacketHandle;
            NetExtPacketCodec &decoder;

            DecodeStruct (
                  const Handle TheHandle,
                  NetExtPacketCodec &theCodec) :
                  PacketHandle (TheHandle),
                  decoder (theCodec) {;}
         };

         struct EncodeObjectStruct {

            const Handle PacketHandle;
            NetExtPacketCodecObject &codec;

            EncodeObjectStruct (
                  const Handle TheHandle,
                  NetExtPacketCodecObject &theCodec) :
                  PacketHandle (TheHandle),
                  codec (theCodec) {;}
         };

         struct EncodeEventStruct {

            const Handle PacketHandle;
            NetExtPacketCodecEvent &codec;

            EncodeEventStruct (
                  const Handle TheHandle,
                  NetExtPacketCodecEvent &theCodec) :
                  PacketHandle (TheHandle),
                  codec (theCodec) {;}
         };

         Boolean _write_object (
            const Handle ObjectHandle,
            const NetObjectEncodeEnum Mode,
            Marshal &outData);

         void _discover_codec (const Plugin *PluginPtr);
         void _init (Config &local, Config &global);
         void _build_header_codec (Config &local);

         Log _log;

         PluginContainer _extensions;

         HeaderElement *_headerCodec;

         HashTableStringTemplate<PacketStruct> _packetTable;
         HashTableHandleTemplate<DecodeStruct> _decodeTable;
         HashTableHandleTemplate<EncodeObjectStruct> _objEncodeTable;
         HashTableHandleTemplate<EncodeObjectStruct> _objTypeTable;
         HashTableHandleTemplate<EncodeObjectStruct> _objTable;
         HashTableHandleTemplate<EncodeEventStruct> _eventEncodeTable;
         HashTableHandleTemplate<EncodeEventStruct> _eventTypeTable;

      private:
         NetModulePacketCodecBasic (const NetModulePacketCodecBasic &);
         NetModulePacketCodecBasic &operator= (const NetModulePacketCodecBasic &);

   };
};

#endif // DMZ_NET_MODULE_PACKET_CODEC_BASIC_DOT_H
