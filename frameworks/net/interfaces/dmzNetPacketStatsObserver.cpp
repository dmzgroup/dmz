/*!

\class dmz::NetPacketStatsObserver
\ingroup Net
\brief Interface for collection network packet statistics.

\fn dmz::NetPacketStatsObserver *dmz::NetPacketStatsObserver::cast (const Plugin *PluginPtr, const String &PluginName)
\brief Casts Plugin pointer to an NetPacketStatsObserver.
\details If the Plugin object implements the NetPacketStatsObserver interface,
a pointer to the NetPacketStatsObserver interface of the Plugin is returned.
\param[in] PluginPtr Pointer to the Plugin to cast.
\param[in] PluginName String containing the name of the desired NetPacketStatsObserver.
\return Returns pointer to the NetPacketStatsObserver. Returns NULL if the PluginPtr does not
implement the NetPacketStatsObserver interface or the \a PluginName is not empty
and not equal to the Plugin's name.

\fn dmz::NetPacketStatsObserver::NetPacketStatsObserver (const PluginInfo &Info)
\brief Constructor.
\param[in] Info PluginInfo containing initialization data.

\fn dmz::NetPacketStatsObserver::~NetPacketStatsObserver ()
\brief Destructor.

\fn dmz::String dmz::NetPacketStatsObserver::get_net_packet_stats_observer_name () const
\brief Gets object module's name.
\return Returns String containing module's name.

\fn dmz::Handle dmz::NetPacketStatsObserver::get_net_packet_stats_observer_handle () const
\brief Gets object module's runtime Handle.
\return Returns Handle containing module's runtime Handle.

\fn void dmz::NetPacketStatsObserver::add_write_packet_stat (const Handle SourceHandle, const Handle TargetHandle, const Int32 PacketSize, const char *Buffer)
\brief Called when a packet is written.
\param[in] SourceHandle Object or Event Handle used to create packet.
\param[in] TargetHandle Handle of Module used to write the packet.
\param[in] PacketSize Size of the packet in bytes.
\param[in] Buffer Packet being written.

\fn void dmz::NetPacketStatsObserver::add_read_packet_stat (const Handle SourceHandle, const Int32 PacketSize, const char *Buffer)
\brief Called when a packet is read.
\param[in] SourceHandle Handle of module that read the packet.
\param[in] PacketSize Size of the packet in bytes.
\param[in] Buffer Packet read.

*/
