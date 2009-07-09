#ifndef DMZ_NET_MODULE_PACKET_CODEC_BASIC_DOT_H
#define DMZ_NET_MODULE_PACKET_CODEC_BASIC_DOT_H

#include <dmzNetExtPacketCodec.h>
#include <dmzNetModulePacketCodec.h>
#include <dmzRuntimeEventType.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimeObjectType.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimePluginContainer.h>
#include <dmzTypesHandleContainer.h>
#include <dmzTypesHashTableStringTemplate.h>
#include <dmzTypesHashTableHandleTemplate.h>

namespace dmz {

   class NetModulePacketCodecBasic :
         public Plugin,
         public NetModulePacketCodec {

      public:
         //! \cond
         class HeaderElement {

            public:
               HeaderElement *next;
               virtual ~HeaderElement () { if (next) { delete next; next = 0; } }
               Boolean read_header (Unmarshal &data, Handle &handle);
               Boolean write_header (const Handle PacketID, Marshal &data);
               virtual Boolean read_element (Unmarshal &data, Handle &packetID) = 0;
               virtual Boolean write_element (
                  const Handle PacketID,
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
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level);

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // NetModulePacketCodec Interface
         virtual void get_supported_objects (ObjectTypeSet &objects);
         virtual void get_supported_events (EventTypeSet &events);

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
            const Handle PacketID;
            ObjectTypeSet objects;
            EventTypeSet events;
            PacketStruct (const String &TheName, const Handle ThePacketID) :
               ExtName (TheName),
               PacketID (ThePacketID) {;}

            ~PacketStruct () {;}
         };

         struct DecodeStruct {

            const Handle PacketID;
            NetExtPacketCodec &decoder;

            DecodeStruct (
                  const Handle ThePacketID,
                  NetExtPacketCodec &theCodec) :
                  PacketID (ThePacketID),
                  decoder (theCodec) {;}
         };

         struct EncodeObjectStruct {

            const Handle PacketID;
            NetExtPacketCodecObject &codec;
            HandleContainer objects;

            EncodeObjectStruct (
                  const Handle ThePacketID,
                  NetExtPacketCodecObject &theCodec) :
                  PacketID (ThePacketID),
                  codec (theCodec) {;}
         };

         struct EncodeEventStruct {

            const Handle PacketID;
            NetExtPacketCodecEvent &codec;

            EncodeEventStruct (
                  const Handle ThePacketID,
                  NetExtPacketCodecEvent &theCodec) :
                  PacketID (ThePacketID),
                  codec (theCodec) {;}
         };

         Boolean _write_object (
            const Handle ObjectHandle,
            const NetObjectEncodeEnum Mode,
            Marshal &outData);

         void _discover_codec (const Plugin *PluginPtr);
         void _remove_codec (const Plugin *PluginPtr);
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
         //! \endcond

      private:
         NetModulePacketCodecBasic (const NetModulePacketCodecBasic &);
         NetModulePacketCodecBasic &operator= (const NetModulePacketCodecBasic &);

   };
};

#endif // DMZ_NET_MODULE_PACKET_CODEC_BASIC_DOT_H
