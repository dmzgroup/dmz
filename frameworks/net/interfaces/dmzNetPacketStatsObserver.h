#ifndef DMZ_NET_PACKET_STATS_OBSERVER_DOT_H
#define DMZ_NET_PACKET_STATS_OBSERVER_DOT_H

#include <dmzRuntimePlugin.h>
#include <dmzRuntimeRTTI.h>
#include <dmzTypesBase.h>
#include <dmzTypesString.h>

namespace dmz {

   class NetPacketStatsObserver {

      public:
         static NetPacketStatsObserver *cast (
            const Plugin *PluginPtr,
            const String &PluginName = "");

         String get_net_packet_stats_observer_name () const;
         Handle get_net_packet_stats_observer_handle () const;

         // NetPacketStatsObserver Interface
         virtual void add_write_packet_stat (
            const Handle SourceHandle,
            const Handle TargetHandle,
            const Int32 PacketSize,
            const char *Buffer) = 0;

         virtual void add_read_packet_stat (
            const Handle SourceHandle,
            const Int32 PacketSize,
            const char *Buffer) = 0;

      protected:
         NetPacketStatsObserver (const PluginInfo &Info);
         ~NetPacketStatsObserver ();

      private:
         NetPacketStatsObserver ();
         NetPacketStatsObserver (const NetPacketStatsObserver &);
         NetPacketStatsObserver &operator= (const NetPacketStatsObserver &);

         const PluginInfo &__Info;
   };

   //! \cond
   const char NetPacketStatsObserverInterfaceName[] = "NetPacketStatsObserverInterface";
   //! \endcond
};


inline dmz::NetPacketStatsObserver *
dmz::NetPacketStatsObserver::cast (const Plugin *PluginPtr, const String &PluginName) {

   return (NetPacketStatsObserver *)lookup_rtti_interface (
      NetPacketStatsObserverInterfaceName,
      PluginName,
      PluginPtr);
}


inline
dmz::NetPacketStatsObserver::NetPacketStatsObserver (const PluginInfo &Info) :
      __Info (Info) {

   store_rtti_interface (NetPacketStatsObserverInterfaceName, __Info, (void *)this);
}


inline
dmz::NetPacketStatsObserver::~NetPacketStatsObserver () {

   remove_rtti_interface (NetPacketStatsObserverInterfaceName, __Info);
}


inline dmz::String
dmz::NetPacketStatsObserver::get_net_packet_stats_observer_name () const { return __Info.get_name (); }


inline dmz::Handle
dmz::NetPacketStatsObserver::get_net_packet_stats_observer_handle () const { return __Info.get_handle (); }

#endif // DMZ_NET_PACKET_STATS_OBSERVER_DOT_H
