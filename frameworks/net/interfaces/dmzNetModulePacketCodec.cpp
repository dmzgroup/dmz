/*!

\class dmz::NetModulePacketCodec
\ingroup Net
\brief Module for encoding and decoding object and event network packets.

\fn dmz::NetModulePacketCodec *dmz::NetModulePacketCodec::cast (
const Plugin *PluginPtr,
const String &PluginName)
\brief Casts Plugin pointer to an NetModulePacketCodec.
\details If the Plugin object implements the NetModulePacketCodec interface, a pointer to
the NetModulePacketCodec interface of the Plugin is returned.
\param[in] PluginPtr Pointer to the Plugin to cast.
\param[in] PluginName String containing the name of the desired NetModulePacketCodec.
\return Returns pointer to the NetModulePacketCodec. Returns NULL if the PluginPtr does
not implement the NetModulePacketCodec interface or the \a PluginName is not empty
and not equal to the Plugin's name.

\fn dmz::NetModulePacketCodec::NetModulePacketCodec (const PluginInfo &Info);
\brief Constructor.

\fn dmz::NetModulePacketCodec::~NetModulePacketCodec ();
\brief Destructor.

\fn void dmz::NetModulePacketCodec::get_supported_objects (ObjectTypeSet &objects)
\brief Gets list of ObjectTypes supported for encoding and decoding.
\param[out] objects ObjectTypeSet containing the supported ObjectTypes.

\fn void dmz::NetModulePacketCodec::get_supported_events (EventTypeSet &events)
\brief Gets list of EventTypes supported for encoding and decoding.
\param[out] events EventTypeSet containing the supported EventTypes.

\fn dmz::Boolean dmz::NetModulePacketCodec::decode (Unmarshal &data, Boolean &isLoopback)
\brief Decodes the packet.
\details Decodes both objects and events. There is no way to tell if a packet contains
an event or an object until it is decoded.
\param[in] data Unmarshal object containing the packet to decode.
\param[out] isLoopback Returned Boolean indicating if the decoded packet was looped back.
\return Returns dmz::True if the packet was successfully decoded.

\fn dmz::Boolean dmz::NetModulePacketCodec::register_object (
const Handle ObjectHandle,
const ObjectType &Type,
Marshal &outData)
\brief Registers an object for network packet encoding.
\param[in] ObjectHandle Handle of the object to register.
\param[in] Type ObjectType of the object to register.
\param[out] outData Marshal containing the encoded object packet.
\return Returns dmz::True of the object was successfully registered and encoded.

\fn dmz::Boolean dmz::NetModulePacketCodec::encode_object (
const Handle ObjectHandle,
Marshal &outData)
\brief Encodes a registered object.
\param[in] ObjectHandle Handle of the registered object to encode.
\param[out] outData Marshal containing the encoded object packet.
\return Returns dmz::True of the object was successfully encoded.

\fn dmz::Boolean dmz::NetModulePacketCodec::release_object (
const Handle ObjectHandle,
Marshal &outData)
\brief Releases a registered object.
\param[in] ObjectHandle Handle of the registered object to release.
\param[out] outData Marshal containing the encoded object packet.
\return Returns dmz::True of the object was successfully encoded and released.

\fn dmz::Boolean dmz::NetModulePacketCodec::encode_event (
const EventType &Type,
const Handle EventHandle,
Marshal &data)
\brief Encodes and event.
\param[in] Type EventType of the event to encode.
\param[in] EventHandle Handle of the event to encode.
\param[out] data Marshal containing the encoded event.
\return Returns dmz::True if the object was successfully encoded.

*/
