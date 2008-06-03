#ifndef DMZ_NET_MODULE_PACKET_IO_HAWK_NL_DOT_H
#define DMZ_NET_MODULE_PACKET_IO_HAWK_NL_DOT_H

#include <dmzNetModulePacketIO.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <dmzRuntimeSync.h>
#include <dmzTypesHashTableUInt32Template.h>
#include <nl.h>

namespace dmz {

   class NetModulePacketIOHawkNL : public Plugin, public Sync, public NetModulePacketIO {

      public:
         NetModulePacketIOHawkNL (const PluginInfo &Info, Config &local);
         ~NetModulePacketIOHawkNL ();

         // Plugin Interface
         virtual void discover_plugin (const Plugin *PluginPtr);
         virtual void start_plugin ();
         virtual void stop_plugin ();
         virtual void shutdown_plugin ();
         virtual void remove_plugin (const Plugin *PluginPtr);

         // Sync Interface
         virtual void update_sync (const Float64 TimeDelta);

         // Net Module Packet IO Interface
         virtual Boolean register_packet_observer (NetPacketObserver &obs);
         virtual Boolean release_packet_observer (NetPacketObserver &obs);

         virtual Boolean write_packet (const Int32 Size, char *buffer);

      protected:
         void _init (Config &local);
         const char *_get_error ();

         Log _log;
         NLsocket _sock;
         HashTableUInt32Template<NetPacketObserver> _obsTable;
         Int32 _bufferSize;
         char *_buffer;

      private:
         NetModulePacketIOHawkNL (const NetModulePacketIOHawkNL &);
         NetModulePacketIOHawkNL &operator= (const NetModulePacketIOHawkNL &);
   };
};

#endif // DMZ_NET_MODULE_PACKET_IO_HAWK_NL_DOT_H
