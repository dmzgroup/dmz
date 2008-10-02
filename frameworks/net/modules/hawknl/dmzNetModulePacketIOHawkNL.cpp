#include "dmzNetModulePacketIOHawkNL.h"
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>

/*!

\class dmz::NetModulePacketIOHawkNL
\ingroup Net
\brief HawkNL implementation of the Network Packet I/O Module.
\details Creates a broadcast socket for reading and writing. Defaults to port 3001 and
a max packet size of 1024 bytes.
\code
<local-scope>
   <socket port="Port Number"/>
   <buffer size="Buffer Size"/>
</local-scope>
\endcode

*/

//! \cond
dmz::NetModulePacketIOHawkNL::NetModulePacketIOHawkNL (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      TimeSlice (Info),
      NetModulePacketIO (Info),
      _log (Info.get_name (), Info.get_context ()),
      _sock (NL_INVALID),
      _bufferSize (1024),
      _buffer (0) {

   _init (local);
}


dmz::NetModulePacketIOHawkNL::~NetModulePacketIOHawkNL () {

   if (_sock != NL_INVALID) { nlClose (_sock); _sock = NL_INVALID; }
   nlShutdown ();
   _obsTable.clear ();
   if (_buffer) { delete []_buffer; _buffer = 0; }
}


// TimeSlice Interface
void
dmz::NetModulePacketIOHawkNL::update_time_slice (const Float64 TimeDelta) {

   if (_buffer && (_sock != NL_INVALID)) {

      Int32 count (0);
      Int32 size (1);

      do {

         size = nlRead (_sock, _buffer, _bufferSize);

         if (size) {

            count++;

            HashTableUInt32Iterator it;

            NetPacketObserver *obs (_obsTable.get_first (it));

            while (obs) {

               obs->read_packet (size, _buffer);
               obs = _obsTable.get_next (it);
            }
         }
      } while (size && (count < 32));
   }
}


// Net Module Packet IO Interface
dmz::Boolean
dmz::NetModulePacketIOHawkNL::register_packet_observer (NetPacketObserver &obs) {

   return _obsTable.store (obs.get_net_packet_observer_handle (), &obs);
}


dmz::Boolean
dmz::NetModulePacketIOHawkNL::release_packet_observer (NetPacketObserver &obs) {

   return _obsTable.remove (obs.get_net_packet_observer_handle ()) ==  &obs;
}


dmz::Boolean
dmz::NetModulePacketIOHawkNL::write_packet (const Int32 Size, char *buffer) {

   Boolean result (False);

   if (_sock != NL_INVALID) {

      result = (Size == nlWrite (_sock, buffer, Size));
   }

   return result;
}


void
dmz::NetModulePacketIOHawkNL::_init (Config &local) {

   nlInit ();

   if (nlSelectNetwork (NL_IP) == NL_TRUE) {

      UInt32 port (config_to_int32 ("socket.port", local, 3001));
      _log.info << "Using port: " << port << endl;

      nlHint (NL_REUSE_ADDRESS, NL_TRUE);
      _sock = nlOpen ((NLushort)port, NL_BROADCAST);

      if (_sock == NL_INVALID) {

         _log.error << "Failed creating socket" << endl;
         _log.error << _get_error () << endl;
      }

      _bufferSize = config_to_int32 ("buffer.size", local, _bufferSize);

      if (_bufferSize > 0) {

         _log.info << "Read buffer size: " << _bufferSize << endl;
         _buffer = new char[_bufferSize];
      }
   }
   else {

      _log.error << "Unable to init Hawk NL: " << _get_error () << endl;
   }
}


const char *
dmz::NetModulePacketIOHawkNL::_get_error () {

   return (const char *)(nlGetError () == NL_SYSTEM_ERROR ?
      nlGetSystemErrorStr (nlGetSystemError ()) : nlGetErrorStr (nlGetError ()));
}
//! \endcond


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzNetModulePacketIOHawkNL (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::NetModulePacketIOHawkNL (Info, local);
}

};
