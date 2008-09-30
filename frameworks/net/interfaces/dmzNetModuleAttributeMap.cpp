/*!

\class dmz::NetModuleAttributeMap
\ingroup Net
\brief Maps between network and DMZ native types an enumerations.

\fn dmz::NetModuleAttributeMap *dmz::NetModuleAttributeMap::cast (
const Plugin *PluginPtr,
const String &PluginName)
\brief Casts Plugin pointer to a NetModuleAttributeMap.
\details If the Plugin object implements the NetModuleAttributeMap interface, a pointer to
the NetModuleAttributeMap interface of the Plugin is returned.
\param[in] PluginPtr Pointer to the Plugin to cast.
\param[in] PluginName String containing the name of the desired NetModuleAttributeMap.
\return Returns pointer to the NetModuleAttributeMap. Returns NULL if the PluginPtr does
not implement the NetModuleAttributeMap interface or the \a PluginName is not empty
and not equal to the Plugin's name.

\fn dmz::NetModuleAttributeMap::NetModuleAttributeMap (const PluginInfo &Info);
\brief Constructor.

\fn dmz::NetModuleAttributeMap::~NetModuleAttributeMap ()
\brief Destructor.

\fn dmz::Boolean dmz::NetModuleAttributeMap::to_net_object_mask (
const ObjectType &Type,
const Mask &InValue,
ArrayUInt32 &outValue)
\brief Converts a dmz::Mask to a network bitmask.
\param[in] Type ObjectType of object.
\param[in] InValue Mask to convert.
\param[out] outValue ArrayUInt32 containing the bitmasks.
\return Returns dmz::True if the dmz::Mask was successfully converted.

\fn dmz::Boolean dmz::NetModuleAttributeMap::to_internal_object_mask (
const ObjectType &Type,
const ArrayUInt32 &InValue,
Mask &outValue)
\brief Converts a network bitmask to a dmz::Mask.
\param[in] Type ObjectType of object.
\param[in] InValue ArrayUInt32 of network bitmasks to convert.
\param[out] outValue Mask with the converted network bitmasks.
\return Returns dmz::True if the network bitmask was successfully converted.

\fn dmz::Boolean dmz::NetModuleAttributeMap::to_net_object_type (
const ObjectType &Type,
ArrayUInt32 &array)
\brief Converts a dmz::ObjectType to the network enumeration.
\param[in] Type ObjectType to convert.
\param[out] array ArrayUInt32 containing the object's network type enumeration.
\return Returns dmz::True if the ObjectType was successfully converted.

\fn dmz::Boolean dmz::NetModuleAttributeMap::to_internal_object_type (
const ArrayUInt32 &Array,
ObjectType &type)
\brief Converts a network enumeration to a dmz::ObjectType.
\param[in] Array ArrayUInt32 containing the object's network type enumeration.
\param[out] type converted ObjectType.
\return Returns dmz::True if the network enumeration was successfully converted.

\fn dmz::Boolean dmz::NetModuleAttributeMap::to_net_event_mask (
const EventType &Type,
const Mask &InValue,
ArrayUInt32 &outValue)
\brief Converts a dmz::Mask to a network bitmask.
\param[in] Type ObjectType of object.
\param[in] InValue Mask to convert.
\param[out] outValue ArrayUInt32 containing the bitmasks.
\return Returns dmz::True if the dmz::Mask was successfully converted.

\fn dmz::Boolean dmz::NetModuleAttributeMap::to_internal_event_mask (
const EventType &Type,
const ArrayUInt32 &InValue,
Mask &outValue)
\brief Converts a network bitmask to a dmz::Mask.
\param[in] Type ObjectType of object.
\param[in] InValue ArrayUInt32 of network bitmasks to convert.
\param[out] outValue Mask with the converted network bitmasks.
\return Returns dmz::True if the network bitmask was successfully converted.

\fn dmz::Boolean dmz::NetModuleAttributeMap::to_net_event_type (
const EventType &Type,
ArrayUInt32 &array)
\brief Converts a dmz::EventType to the network enumeration.
\param[in] Type EventType to convert.
\param[out] array ArrayUInt32 containing the object's network type enumeration.
\return Returns dmz::True if the EventType was successfully converted.

\fn dmz::Boolean dmz::NetModuleAttributeMap::to_internal_event_type (
const ArrayUInt32 &Array,
EventType &type)
\brief Converts a network enumeration to a dmz::EventType.
\param[in] Array ArrayUInt32 containing the object's network type enumeration.
\param[out] type converted EventType.
\return Returns dmz::True if the network enumeration was successfully converted.

*/
