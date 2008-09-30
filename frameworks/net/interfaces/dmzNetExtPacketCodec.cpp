/*!

\class dmz::NetExtPacketCodec
\ingroup Net
\brief Network packet codec interface.

\fn dmz::NetExtPacketCodec::NetExtPacketCodec (const PluginInfo &Info);
\brief Constructor.

\fn dmz::NetExtPacketCodec::~NetExtPacketCodec () {;}
\brief Destructor.

\fn dmz::String dmz::NetExtPacketCodec::get_net_ext_packet_codec_name () const;
\brief Gets packet codec's name.

\fn dmz::Boolean dmz::NetExtPacketCodec::decode (Unmarshal &data, Boolean &isLoopback)
\brief Decodes packet.
\param[in] data Unmarshal object containing the packet buffer to decode.
\param[out] isLoopback Boolean set to true if the packet to decode is looped back.
\return Returns dmz::True if the packet was successfully decoded.

\class dmz::NetExtPacketCodecObject
\ingroup Net
\brief Network packet codec for objects.

\fn dmz::NetExtPacketCodecObject::NetExtPacketCodecObject (const PluginInfo &Info);
\brief Constructor.

\fn dmz::NetExtPacketCodecObject::~NetExtPacketCodecObject ();
\brief Destructor.

\fn dmz::NetExtPacketCodecObject *dmz::NetExtPacketCodecObject::cast (
const Plugin *PluginPtr,
const String &PluginName);
\brief Casts Plugin pointer to an NetExtPacketCodecObject.
\details If the Plugin object implements the NetExtPacketCodecObject interface,
a pointer to the NetExtPacketCodecObject interface of the Plugin is returned.
\param[in] PluginPtr Pointer to the Plugin to cast.
\param[in] PluginName String containing the name of the desired NetExtPacketCodecObject.
\return Returns pointer to the NetExtPacketCodecObject. Returns NULL if the PluginPtr
does not implement the NetExtPacketCodecObject interface or the \a PluginName is not empty
and not equal to the Plugin's name.

\fn dmz::Boolean dmz::NetExtPacketCodecObject::encode_object (
const Handle ObjectHandle,
const NetObjectEncodeEnum EncodeMode,
Marshal &data)
\brief Encodes an object's values into a packet.
\param[in] ObjectHandle Handle of the object to encode.
\param[in] EncodeMode The mode to use when encoding the object. Use
dmz::NetObjectActivate when the object is being encoded for the first time. Use
dmz::NetObjectUpdate when the object's attributes have been updated. Use
dmz::NetObjectDeactivate when the object has been destroyed or it is being removed
from the network.
\param[out] data Marshal to store the object's encoded packet.
\return Returns dmz::True if the packet was successfully encoded.

\class dmz::NetExtPacketCodecEvent
\ingroup Net
\brief Network packet codec for events.

\fn dmz::NetExtPacketCodecEvent::NetExtPacketCodecEvent (const PluginInfo &Info);
\brief Constructor.

\fn dmz::NetExtPacketCodecEvent::~NetExtPacketCodecEvent ();
\brief Destructor.

\fn dmz::NetExtPacketCodecEvent *dmz::NetExtPacketCodecEvent::cast (
const Plugin *PluginPtr,
const String &PluginName)
\brief Casts Plugin pointer to an NetExtPacketCodecEvent.
\details If the Plugin object implements the NetExtPacketCodecEvent interface,
a pointer to the NetExtPacketCodecEvent interface of the Plugin is returned.
\param[in] PluginPtr Pointer to the Plugin to cast.
\param[in] PluginName String containing the name of the desired NetExtPacketCodecEvent.
\return Returns pointer to the NetExtPacketCodecEvent. Returns NULL if the PluginPtr
does not implement the NetExtPacketCodecEvent interface or the \a PluginName is not empty
and not equal to the Plugin's name.

\fn dmz::Boolean dmz::NetExtPacketCodecEvent::encode_event (
const Handle EventHandle,
Marshal &data)
\brief Encodes an event's values into a packet.
\param[in] EventHandle Handle of the event to encode.
\param[out] data Marshal to store the event's encoded packet.
\return Returns dmz::True if the packet was successfully encoded.

*/
