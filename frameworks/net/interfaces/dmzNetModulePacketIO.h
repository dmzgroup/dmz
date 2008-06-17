#ifndef DMZ_NET_MODULE_PACKET_IO_DOT_H
#define DMZ_NET_MODULE_PACKET_IO_DOT_H

#include <dmzRuntimePlugin.h>
#include <dmzRuntimeRTTI.h>
#include <dmzTypesBase.h>

#define DMZ_NET_MODULE_PACKET_IO_INTERFACE_NAME "NetModulePacketIOInterface"

namespace dmz {

   class NetPacketObserver {

      public:
         UInt32 get_net_packet_observer_handle ();

         virtual void read_packet (const Int32 Size, char *buffer) = 0;

      protected:
         NetPacketObserver (const PluginInfo &Info);
         ~NetPacketObserver () {;}

      private:
         const PluginInfo &__Info;

   };

   class NetModulePacketIO {

      public:
         static NetModulePacketIO *cast (
            const Plugin *PluginPtr,
            const String &PluginName = "");

         virtual Boolean register_packet_observer (NetPacketObserver &obs) = 0;
         virtual Boolean release_packet_observer (NetPacketObserver &obs) = 0;

         virtual Boolean write_packet (const Int32 Size, char *buffer) = 0;

      protected:
         NetModulePacketIO (const PluginInfo &Info);
         ~NetModulePacketIO ();

      private:
         const PluginInfo &__Info;
   };
};


inline
dmz::NetPacketObserver::NetPacketObserver (const PluginInfo &Info) : __Info (Info) {;}


inline dmz::UInt32
dmz::NetPacketObserver::get_net_packet_observer_handle () { return __Info.get_handle (); }


inline dmz::NetModulePacketIO *
dmz::NetModulePacketIO::cast (const Plugin *PluginPtr, const String &PluginName) {

   return (NetModulePacketIO *)lookup_rtti_interface (
      DMZ_NET_MODULE_PACKET_IO_INTERFACE_NAME,
      PluginName,
      PluginPtr);
}


inline
dmz::NetModulePacketIO::NetModulePacketIO (const PluginInfo &Info) :
      __Info (Info) {

   store_rtti_interface (DMZ_NET_MODULE_PACKET_IO_INTERFACE_NAME, __Info, (void *)this);
}


inline
dmz::NetModulePacketIO::~NetModulePacketIO () {

   remove_rtti_interface (DMZ_NET_MODULE_PACKET_IO_INTERFACE_NAME, __Info);
}

#endif //  DMZ_NET_MODULE_PACKET_IO_DOT_H
