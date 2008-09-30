/*!

\defgroup Net DMZ Network Framework.

\class dmz::NetPacketObserver
\ingroup Net
\brief Observer read network packets.

\fn dmz::NetPacketObserver::NetPacketObserver (const PluginInfo &Info)
\brief Constructor.

\fn dmz::NetPacketObserver::~NetPacketObserver ()
\brief Destructor.

\fn dmz::UInt32 dmz::NetPacketObserver::get_net_packet_observer_handle ()
\brief Gets network packet observer handle.

\fn void dmz::NetPacketObserver::read_packet (const Int32 Size, char *buffer)
\brief Callback made when a network packet has been read.
\param[in] Size Number of bytes in the network packet buffer.
\param[in] buffer pointer to the read packet.


\class dmz::NetModulePacketIO
\ingroup Net
\brief Reads and writes network packets.

\fn dmz::NetModulePacketIO::NetModulePacketIO (const PluginInfo &Info)
\brief Constructor.

\fn dmz::NetModulePacketIO::~NetModulePacketIO ()
\brief Destructor.

\fn dmz::NetModulePacketIO *dmz::NetModulePacketIO::cast (
const Plugin *PluginPtr,
const String &PluginName)
\brief Casts Plugin pointer to an NetModulePacketIO.
\details If the Plugin object implements the NetModulePacketIO interface, a pointer to
the NetModulePacketIO interface of the Plugin is returned.
\param[in] PluginPtr Pointer to the Plugin to cast.
\param[in] PluginName String containing the name of the desired NetModulePacketIO.
\return Returns pointer to the NetModulePacketIO. Returns NULL if the PluginPtr does not
implement the NetModulePacketIO interface or the \a PluginName is not empty
and not equal to the Plugin's name.

\fn dmz::Boolean dmz::NetModulePacketIO::register_packet_observer (NetPacketObserver &obs)
\brief Register packet observer.

\fn dmz::Boolean dmz::NetModulePacketIO::release_packet_observer (NetPacketObserver &obs)
\brief Releases packet observer.

\fn dmz::Boolean dmz::NetModulePacketIO::write_packet (const Int32 Size, char *buffer)
\brief Write packet to the network.
\param[in] Size Number of bytes in the packet.
\param[in] buffer pointer to the packet to be written.
\return Returns dmz::True if the packet was successfully written.

*/
