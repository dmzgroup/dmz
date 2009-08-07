/*!

\defgroup Object DMZ Object Framework

\class dmz::ObjectModule
\ingroup Object
\brief Object module interface.
\details The object module is a runtime database that stores attributes of
objects. These objects maybe owned locally or remotely. Each object may have multiple
attributes of the same type. For example an object may have a current position and
a position that represents the last position that was transmitted over the network.
An attribute handle is used to access the different attributes of a given type.
The object module supports both a push and pull model for attribute updates. The
lookup_* functions may be used to pull object attributes from the object module.
An object observer is used to push attribute updates.
An object observer is registered with the object module and
may subscribe to specific attributes. After registration, the object observer
will be notified whenever an object's attributes are updated.
Attribute handles are what allow multiple values to be stored in one
attribute. Most attributes relating to the current object state are stored with the
default attribute handle defined by the String variable dmz::ObjectAttributeDefaultName.
Attributes that represent the last network value transmitted or received are stored
with the attribute handle defined by the String variable
dmz::ObjectAttributeLastNetworkValueName.
Attribute handles are named handles allocated by the runtime.
Since attribute handles are runtime allocated, the
attribute handle value should never be hard coded as it mostly like be a different
number from run to run.
Zero is considered an invalid attribute handle.
\sa dmz::ObjectObserver \n dmz::Definitions::create_named_handle()

\fn dmz::ObjectModule *dmz::ObjectModule::cast (
const Plugin *PluginPtr,
const dmz::String &PluginName);
\brief Casts Plugin pointer to an ObjectModule.
\details If the Plugin object implements the ObjectModule interface, a pointer to
the ObjectModule interface of the Plugin is returned.
\param[in] PluginPtr Pointer to the Plugin to cast.
\param[in] PluginName String containing the name of the desired ObjectModule.
\return Returns pointer to the ObjectModule. Returns NULL if the PluginPtr does not
implement the ObjectModule interface or the \a PluginName is not empty
and not equal to the Plugin's name.

\fn dmz::String dmz::ObjectModule::get_object_module_name () const
\brief Gets object module's name.
\return Returns String containing object module's name.

\fn dmz::Handle dmz::ObjectModule::get_object_module_handle () const
\brief Gets object module's runtime Handle.
\return Returns Handle containing object module's runtime Handle.


\fn dmz::Boolean dmz::ObjectModule::register_global_object_observer (
ObjectObserver &observer)
\brief Registers a global object observer.
\details A global object observer receives \b all object attribute updates. Use with
caution.
\param[in] observer ObjectObserver to register as a global observer.
\return Returns dmz::True if the observer was successfully registered.

\fn dmz::Boolean dmz::ObjectModule::release_global_object_observer (
ObjectObserver &observer)
\brief Releases a global object observer.
\param[in] observer ObjectObserver to release as a global observer.
\return Returns dmz::True if the observer was successfully released.
\sa dmz::ObjectModule::register_global_object_observer()

\fn dmz::Boolean dmz::ObjectModule::register_object_observer (
const dmz::Handle AttributeHandle,
const Mask &AttributeMask,
ObjectObserver &observer)
\brief Registers object observer.
\details Allows an object observer to register for attributes associated with a
certain attribute handle. The same observer may be passed in multiple time to register
for attributes with different attribute handle. An observer may only register for any
given attribute handle and attribute once.
\param[in] AttributeHandle Attribute handle.
\param[in] AttributeMask Mask containing the attributes desired for subscription.
\param[in] observer ObjectObserver to register.
\return Returns dmz::True if the object observer was successfully registered.

\fn dmz::Boolean dmz::ObjectModule::release_object_observer (
const dmz::Handle AttributeHandle,
const dmz::Mask &AttributeMask,
dmz::ObjectObserver &observer)
\brief Release an object observer.
\details This function will only release the observer from the attributes defined
in the \a AttributeMask. If an observer has subscribed to position and orientation
updates and the AttributeMask only contains the orientation bit, the observer will
continue to receive position updates.
\param[in] AttributeHandle Attribute handle.
\param[in] AttributeMask Mask containing the attributes desired for unsubscription.
\param[in] observer ObjectObserver to release.
\return Returns dmz::True if the object observer was successfully released.

\fn dmz::Boolean dmz::ObjectModule::release_object_observer_all (
dmz::ObjectObserver &observer)
\brief Releases object observer from all attribute subscriptions.
\param[in] observer ObjectObserver to be released.
\return Returns dmz::True if the observer is released from all attribute subscriptions.

\fn dmz::Boolean dmz::ObjectModule::is_object (const dmz::Handle ObjectHandle)
\brief Tests if a handle is associated with an object stored in the module.
\param[in] ObjectHandle Handle to test.
\return Returns dmz::True if the Handle is associated with an object. The object does not
need to be activated for this function to return dmz::True.

\fn dmz::Boolean dmz::ObjectModule::is_activated (const Handle ObjectHandle)
\brief Tests if a handle is associated with an activated object stored in the module.
\param[in] ObjectHandle Handle to test.
\return Returns dmz::True if the Handle is associated with an activated object. Returns
dmz::False if the object has not be activated or if the Handle is not associated with an
object.

\fn dmz::Boolean dmz::ObjectModule::is_link (const dmz::Handle LinkHandle)
\brief Tests if a handle is associated with linked objects stored in the module.
\param[in] LinkHandle Handle to test.
\return Returns dmz::True if the Handle is associated with a link.

\fn dmz::Boolean dmz::ObjectModule::get_object_handles (dmz::HandleContainer &container)
\brief Creates a list of all objects in the module.
\details The list will contain both local and remote objects. Objects that have
not been activated yet will \b not be in the list.
\param[out] container HandleContainer used to store all object handles.
\return Returns dmz::True if the container was filled with object handles.

\fn dmz::Boolean dmz::ObjectModule::dump_object_attributes (
const dmz::Handle ObjectHandle,
dmz::ObjectObserver &Observer)
\brief Dumps subscribed object attributes.
\details This function will dump all attributes for which the ObjectObserver has
a subscription. If the object observer has not subscribed to any attributes, nothing
will be dumped. This function may be used to force the object module push all
object attribute values to the object observer even if they have not changed.
\param[in] ObjectHandle Object handle for object to dump to observer.
\param[in] Observer ObjectObserver to receive object attribute values.
\return Returns dmz::True if any attributes were dumped.

\fn dmz::Boolean dmz::ObjectModule::dump_all_object_attributes (
const dmz::Handle ObjectHandle,
dmz::ObjectObserver &Observer)
\brief Dumps all object attributes.
\details This function does not check if the observer has subscribed for any specific
attributes but will instead dump the values of any attribute that has been stored
with the object.
\param[in] ObjectHandle Object handle for object to dump to observer.
\param[in] Observer ObjectObserver to receive \b all object attribute values.
\return Returns dmz::True if any attributes were dumped.

\fn dmz::Handle dmz::ObjectModule::create_object (
const dmz::ObjectType &Type,
const dmz::ObjectLocalityEnum Locality)
\brief Creates object.
\details Creates an object in the object module. The object is not active after this
call. Calls to store_* functions will store the values in the object module but will
not propagate the attribute values to subscribing ObjectObserver until the object
is activated with a call to dmz::ObjectModule::activate_object(). The object type
must be valid. An object can \b not be create with an empty object type.
\param[in] Type ObjectType of the object being created.
\param[in] Locality dmz::ObjectLocalityEnum specifying if the object is owned locally or
remotely.
\return Returns the object handle of the newly created object. Will return zero if
the creation fails.

\fn dmz::Boolean dmz::ObjectModule::activate_object (const dmz::Handle ObjectHandle)
\brief Activates a created object.
\details Any attribute values that have been set since the object was created will
be pushed to subscribing object observers. The message defined by
dmz::ObjectCreateMessageName will be send with a Data object containing the object
handle. DataConverterObjectHandle should be used to extract the object handle
from the Data object.
\note If the object is locally owned and does not have a UUID stored at the time of
activation, the object module will assign it a new UUID.
\param[in] ObjectHandle dmz::Handle of object to activate.
\return Returns dmz::True if the object was activated.
\sa DataConverterObjectHandle \n Messaging

\fn dmz::Boolean dmz::ObjectModule::destroy_object (const dmz::Handle ObjectHandle)
\brief Destroys object.
\details The message defined by dmz::ObjectRemoveMessageName will be send with a
Data object containing the object handle. DataConverterObjectHandle should be used to
extract the object handle from the Data object. The object's attributes may still be
queried when the object observer receives the dmz::ObjectObserver::destroy_object()
callback.
\param[in] ObjectHandle dmz::Handle of object to destroy.
\return Returns dmz::True if the object is destroyed.

\fn dmz::Handle dmz::ObjectModule::clone_object (
const dmz::Handle ObjectHandle,
const dmz::ObjectLinkRetentionEnum LinkRetention)
\brief Clones object.
\details Creates a clone of the object specified by the object handle. The clone will
have a different ObjectHandle and UUID. Links will be cloned if the LinkRetention
parameter is set to dmz::ObjectRetainLinks. Otherwise, links are \b not cloned.
\note Cloned object is not active. It must be activated by a call to
dmz::ObjectModule::activate_object().
\param[in] ObjectHandle dmz::Handle of object to clone.
\param[in] LinkRetention dmz::ObjectLinkRetentionEnum that specifies if object links
should also be cloned.
\return Returns the dmz::Handle of the newly cloned object.

\fn dmz::ObjectType dmz::ObjectModule::lookup_object_type (const Handle ObjectHandle)
\brief Looks up object's type.
\param[in] ObjectHandle dmz::Handle of object.
\return Returns the dmz::ObjectType of the object. Returns an invalid dmz::ObjectType
if the object does not exist.

\fn dmz::Boolean dmz::ObjectModule::store_uuid (
const dmz::Handle ObjectHandle,
const dmz::UUID &Value)
\brief Stores object's UUID.
\param[in] ObjectHandle dmz::Handle of object to update.
\param[in] Value UUID object to store in object.
\return Returns dmz::True if the UUID was successfully stored.

\fn dmz::Boolean dmz::ObjectModule::lookup_uuid (
const dmz::Handle ObjectHandle,
UUID &value)
\brief Looks up object's UUID.
\param[in] ObjectHandle dmz::Handle of object.
\param[out] value UUID object to store object's uuid.
\return Returns dmz::True if the UUID was successfully found.

\fn dmz::Handle dmz::ObjectModule::lookup_handle_from_uuid (const UUID &Value)
\brief Converts UUID to dmz::Handle.
\details This functions find the dmz::Handle associated with the given UUID.
\param[in] Value UUID to convert.
\return Returns dmz::Handle of the object associated with the UUID. Returns
zero if no object is associated with the UUID.

\fn dmz::Boolean dmz::ObjectModule::remove_attribute (
const dmz::Handle ObjectHandle,
const dmz::Handle AttributeHandle,
const dmz::Mask &AttrMask)
\brief Remove attribute.
\details Removes attribute from object. After the attribute has been removed,
the lookup_* function will return dmz::False as if the attribute had never been
set.
\param[in] ObjectHandle dmz::Handle of object to remove attributes.
\param[in] AttributeHandle Attribute handle of attributes to remove.
\param[in] AttrMask Mask containing flags indicating which attributes to remove.
\return Returns dmz::True if the attributes were removed.

\fn dmz::Handle dmz::ObjectModule::link_objects (
const dmz::Handle AttributeHandle,
const dmz::Handle SuperHandle,
const dmz::Handle SubHandle)
\brief Links objects.
\details Creates a link between two objects. A link is automatically broken
when one of the linked objects is destroyed.
\param[in] AttributeHandle Attribute handle of the link.
\param[in] SuperHandle dmz::Handle of the super object in the link.
\param[in] SubHandle dmz::Handle of the sub object in the link.
\return Returns the dmz::Handle associated with the link.

\fn dmz::Handle dmz::ObjectModule::lookup_link_handle (
const dmz::Handle AttributeHandle,
const dmz::Handle SuperHandle,
const dmz::Handle SubHandle)
\brief Finds the link handle for two linked objects.
\param[in] AttributeHandle Attribute handle of the link.
\param[in] SuperHandle Handle for the super object in the link.
\param[in] SubHandle Handle for the sub object in the link.
\return Returns the Handle for the link.

\fn dmz::Boolean dmz::ObjectModule::lookup_linked_objects (
const dmz::Handle LinkHandle,
dmz::Handle &attributeHandle,
dmz::Handle &superHandle,
dmz::Handle &subHandle)
\brief Finds the objects associated with the link handle.
\param[in] LinkHandle Link handle as returned by dmz::ObjectModule::link_objects()..
\param[out] attributeHandle Attribute handle of the link.
\param[out] superHandle Handle for the super object in the link.
\param[out] subHandle Handle for the sub object in the link.
\return Returns the Handle for the link.
\fn dmz::Boolean dmz::ObjectModule::unlink_objects (const dmz::Handle LinkHandle)
\brief Unlinks objects.
\param[in] LinkHandle Link handle as returned by dmz::ObjectModule::link_objects()..
\return Returns dmz::True if the objects are unlinked.

\fn dmz::Boolean dmz::ObjectModule::unlink_super_links (
const Handle ObjectHandle,
const Handle AttributeHandle)
\brief Unlinks all super linked objects with the given attribute handle.
\details All super links to the object of the given link attribute are unlinked.
\param[in] ObjectHandle Handle of object to unlink.
\param[in] AttributeHandle The link's attribute handle.
\return Returns dmz::True if the super links were unlinked.

\fn dmz::Boolean dmz::ObjectModule::unlink_sub_links (
const Handle ObjectHandle,
const Handle AttributeHandle) 
\brief Unlinks all sub linked objects with the given attribute handle.
\details All sub links to the object of the given link attribute are unlinked.
\param[in] ObjectHandle Handle of object to unlink.
\param[in] AttributeHandle The link's attribute handle.
\return Returns dmz::True if the sub links were unlinked.


\fn dmz::Boolean dmz::ObjectModule::store_link_attribute_object (
const dmz::Handle LinkHandle,
const dmz::Handle AttributeObjectHandle)
\brief Stores link attribute object.
\details Links may be associated with an object in the object module. This object
contains the links attributes. This object may define the lines color, thickness,
direction, etc. The same attribute object may be associated with multiple links.
There is nothing special about a links attribute object. It is an object like any
other object in the module. Set \a AttributeObjectHandle to zero to remove the
attribute object from the link.
\param[in] LinkHandle Link handle as returned by dmz::ObjectModule::link_objects()..
\param[in] AttributeObjectHandle dmz::Handle of the link's attribute object.
\return Returns dmz::True if attribute object was associated with the link.

\fn dmz::Handle dmz::ObjectModule::lookup_link_attribute_object (
const dmz::Handle LinkHandle)
\brief Gets object handle to link's attribute object.
\param[in] LinkHandle Link handle as returned by dmz::ObjectModule::link_objects()..
\return Returns the dmz::Handle of the link's attribute object. Returns zero
it the link does not have an attribute object.

\fn dmz::Boolean dmz::ObjectModule::lookup_super_links (
const dmz::Handle ObjectHandle,
const dmz::Handle AttributeHandle,
dmz::HandleContainer &container)
\brief Gets all super links of the object.
\details Stores the dmz::Handle of all objects that are super links to the given
object for a specific attribute handle.
\param[in] ObjectHandle dmz::Handle of sub object.
\param[in] AttributeHandle Attribute handle of the link.
\param[out] container HandleContainer to store the dmz::Handles of the
super links.
\return Returns dmz::True if the object has super links.

\fn dmz::Boolean dmz::ObjectModule::lookup_sub_links (
const dmz::Handle ObjectHandle,
const dmz::Handle AttributeHandle,
dmz::HandleContainer &container)
\brief Gets all sub links of the object.
\details Stores the dmz::Handle of all objects that are sub links to the given
object for a specific attribute handle.
\param[in] ObjectHandle dmz::Handle of super object.
\param[in] AttributeHandle Attribute handle of the link.
\param[out] container HandleContainer to store the dmz::Handles of the
sub links.
\return Returns dmz::True if the object has sub links.

\fn dmz::Boolean dmz::ObjectModule::store_counter (
const Handle ObjectHandle,
const Handle AttributeHandle,
const Int64 Value)
\brief Stores counter value.
\note If the \b Value is greater than the maximum value or
less than the minimum value, it is clamped to the limit.
\param[in] ObjectHandle dmz::Handle of object.
\param[in] AttributeHandle Attribute handle.
\param[in] Value Object's counter value.
\return Returns dmz::True if the object's counter was set.

\fn dmz::Boolean dmz::ObjectModule::lookup_counter (
const Handle ObjectHandle,
const Handle AttributeHandle,
Int64 &value)
\brief Looks up counter value.
\param[in] ObjectHandle dmz::Handle of the object.
\param[in] AttributeHandle Attribute handle.
\param[out] value Object's counter value.
\return Returns dmz::True if the object's counter was found.

\fn dmz::Int64 dmz::ObjectModule::add_to_counter (
const Handle ObjectHandle,
const Handle AttributeHandle,
const Int64 Value)
\brief Adds value to the counter.
\details If the added value takes the counter over the maximum or under the minimum,
the counter is ether clamped is the rollover state is dmz::False or the value rollover
if the rollover state is set to dmz::True.
\param[in] ObjectHandle dmz::Handle of the object.
\param[in] AttributeHandle Attribute handle.
\param[in] Value Value to add the object's counter.
\return Returns the new value of the counter.

\fn dmz::Boolean dmz::ObjectModule::store_counter_minimum (
const Handle ObjectHandle,
const Handle AttributeHandle,
const Int64 Value)
\brief Stores the minimum value the counter may have.
\param[in] ObjectHandle dmz::Handle of the object.
\param[in] AttributeHandle Attribute handle.
\param[in] Value Object's minimum counter value.
\return Returns dmz::True if the counters minimum was set.

\fn dmz::Boolean dmz::ObjectModule::lookup_counter_minimum (
const Handle ObjectHandle,
const Handle AttributeHandle,
Int64 &value)
\brief Looks up the minimum value the counter may have.
\param[in] ObjectHandle dmz::Handle of the object.
\param[in] AttributeHandle Attribute handle.
\param[out] value Object's minimum counter value.
\return Returns dmz::True if the counters minimum was found.

\fn dmz::Boolean dmz::ObjectModule::store_counter_maximum (
const Handle ObjectHandle,
const Handle AttributeHandle,
const Int64 Value)
\brief Stores the maximum value the counter may have.
\param[in] ObjectHandle dmz::Handle of the object.
\param[in] AttributeHandle Attribute handle.
\param[in] Value Object's maximum counter value.
\return Returns dmz::True if the counters maximum was set.

\fn dmz::Boolean dmz::ObjectModule::lookup_counter_maximum (
const Handle ObjectHandle,
const Handle AttributeHandle,
Int64 &value)
\brief Looks up the maximum value the counter may have.
\param[in] ObjectHandle dmz::Handle of the object.
\param[in] AttributeHandle Attribute handle.
\param[out] value Object's maximum counter value.
\return Returns dmz::True if the counters maximum was found.

\fn dmz::Boolean dmz::ObjectModule::store_counter_rollover (
const Handle ObjectHandle,
const Handle AttributeHandle,
const Boolean Value)
\brief Stores the rollover state of the counter.
\param[in] ObjectHandle dmz::Handle of the object.
\param[in] AttributeHandle Attribute handle.
\param[in] Value The rollover state.
\return Returns dmz::True if the rollover state was updated.

\fn dmz::Boolean dmz::ObjectModule::lookup_counter_rollover (
const Handle ObjectHandle,
const Handle AttributeHandle,
Boolean &value)
\brief Looks up the rollover state of the counter.
\param[in] ObjectHandle dmz::Handle of the object.
\param[in] AttributeHandle Attribute handle.
\param[out] value Contains the counter's rollover state.
\return Returns dmz::True if the counters rollover state was found.

\fn dmz::Boolean dmz::ObjectModule::store_locality (
const dmz::Handle ObjectHandle,
const dmz::ObjectLocalityEnum Locality)
\brief Stores object's locality.
\details The locality defines whether the object is locally or remotely.
\param[in] ObjectHandle dmz::Handle of object.
\param[in] Locality Objects locality.
\return Returns dmz::True if the objects attribute was updated.

\fn dmz::ObjectLocalityEnum dmz::ObjectModule::lookup_locality (
const dmz::Handle ObjectHandle)
\brief Looks up object's locality.
\details The locality defines whether the object is locally or remotely.
\param[in] ObjectHandle dmz::Handle of object.
\return Returns dmz::ObjectLocal if object is local. Returns dmz::ObjectRemote if object
is remote. Returns dmz::ObjectLocalityUnknown if the object's handle is unknown.

\fn dmz::ObjectLocalityEnum dmz::ObjectModule::lookup_locality (const dmz::UUID &Identity)
\brief Looks up object's locality.
\details The locality defines whether the object is locally or remotely.
\param[in] Identity UUID of object.
\return Returns dmz::ObjectLocal if object is local. Returns dmz::ObjectRemote if object
is remote. Returns dmz::ObjectLocalityUnknown if the UUID is unknown.

\fn dmz::Boolean dmz::ObjectModule::store_alternate_object_type (
const dmz::Handle ObjectHandle,
const dmz::Handle AttributeHandle,
const dmz::ObjectType &Value)
\brief Stores an ObjectType with the object.
\param[in] ObjectHandle dmz::Handle of object.
\param[in] AttributeHandle Attribute handle.
\param[in] Value Object's alternate type value.
\return Returns dmz::True if the attribute was successfully updated.

\fn dmz::Boolean dmz::ObjectModule::lookup_alternate_object_type (
const dmz::Handle ObjectHandle,
const dmz::Handle AttributeHandle,
dmz::ObjectType &value)
\brief Looks up ObjectType stored with the object.
\param[in] ObjectHandle dmz::Handle of object.
\param[in] AttributeHandle Attribute handle.
\param[out] value Object type value.
\return Returns dmz::True if the attribute was found.

\fn dmz::Boolean dmz::ObjectModule::store_state (
const dmz::Handle ObjectHandle,
const dmz::Handle AttributeHandle,
const dmz::Mask &Value)
\brief Stores a Mask containing state with the object.
\param[in] ObjectHandle dmz::Handle of object.
\param[in] AttributeHandle Attribute handle.
\param[in] Value Object's new state value.
\return Returns dmz::True if the attribute was successfully updated.

\fn dmz::Boolean dmz::ObjectModule::lookup_state (
const dmz::Handle ObjectHandle,
const dmz::Handle AttributeHandle,
dmz::Mask &value)
\brief Looks up state stored with the object.
\param[in] ObjectHandle dmz::Handle of object.
\param[in] AttributeHandle Attribute handle.
\param[out] value Object state value.
\return Returns dmz::True if the attribute was found.

\fn dmz::Boolean dmz::ObjectModule::store_flag (
const dmz::Handle ObjectHandle,
const dmz::Handle AttributeHandle,
const dmz::Boolean Value)
\brief Stores object's flag value.
\param[in] ObjectHandle dmz::Handle of object.
\param[in] AttributeHandle Attribute handle.
\param[in] Value Object flag's new value.
\return Returns dmz::True if the attribute was successfully updated.

\fn dmz::Boolean dmz::ObjectModule::lookup_flag (
const dmz::Handle ObjectHandle,
const dmz::Handle AttributeHandle)
\brief Looks up flag stored with the object.
\param[in] ObjectHandle dmz::Handle of object.
\param[in] AttributeHandle Attribute handle.
\return Returns dmz::True if the flag was set. Returns dmz::False if the flag is not set
or the object does not exist.

\fn dmz::Boolean dmz::ObjectModule::store_time_stamp (
const dmz::Handle ObjectHandle,
const dmz::Handle AttributeHandle,
const dmz::Float64 Value)
\brief Stores time stamp with the object.
\param[in] ObjectHandle dmz::Handle of object.
\param[in] AttributeHandle Attribute handle.
\param[in] Value Object's new time stamp value.
\return Returns dmz::True if the attribute was successfully updated.

\fn dmz::Boolean dmz::ObjectModule::lookup_time_stamp (
const dmz::Handle ObjectHandle,
const dmz::Handle AttributeHandle,
Float64 &value)
\brief Looks up time stamp stored with the object.
\param[in] ObjectHandle dmz::Handle of object.
\param[in] AttributeHandle Attribute handle.
\param[out] value Object time stamp value.
\return Returns dmz::True if the attribute was found.

\fn dmz::Boolean dmz::ObjectModule::store_position (
const dmz::Handle ObjectHandle,
const dmz::Handle AttributeHandle,
const dmz::Vector &Value)
\brief Stores position with the object.
\param[in] ObjectHandle dmz::Handle of object.
\param[in] AttributeHandle Attribute handle.
\param[in] Value Object's new position value.
\return Returns dmz::True if the attribute was successfully updated.

\fn dmz::Boolean dmz::ObjectModule::lookup_position (
const dmz::Handle ObjectHandle,
const dmz::Handle AttributeHandle,
dmz::Vector &value)
\brief Looks up position stored with the object.
\param[in] ObjectHandle dmz::Handle of object.
\param[in] AttributeHandle Attribute handle.
\param[out] value Object position value.
\return Returns dmz::True if the attribute was found.

\fn dmz::Boolean dmz::ObjectModule::store_orientation (
const dmz::Handle ObjectHandle,
const dmz::Handle AttributeHandle,
const dmz::Matrix &Value)
\brief Stores orientation with the object.
\param[in] ObjectHandle dmz::Handle of object.
\param[in] AttributeHandle Attribute handle.
\param[in] Value Object's new orientation value.
\return Returns dmz::True if the attribute was successfully updated.

\fn dmz::Boolean dmz::ObjectModule::lookup_orientation (
const dmz::Handle ObjectHandle,
const dmz::Handle AttributeHandle,
dmz::Matrix &value)
\brief Looks up orientation stored with the object.
\param[in] ObjectHandle dmz::Handle of object.
\param[in] AttributeHandle Attribute handle.
\param[out] value Object orientation value.
\return Returns dmz::True if the attribute was found.

\fn dmz::Boolean dmz::ObjectModule::store_velocity (
const dmz::Handle ObjectHandle,
const dmz::Handle AttributeHandle,
const dmz::Vector &Value)
\brief Stores velocity with the object.
\param[in] ObjectHandle dmz::Handle of object.
\param[in] AttributeHandle Attribute handle.
\param[in] Value Object's new velocity value.
\return Returns dmz::True if the attribute was successfully updated.

\fn dmz::Boolean dmz::ObjectModule::lookup_velocity (
const dmz::Handle ObjectHandle,
const dmz::Handle AttributeHandle,
dmz::Vector &value)
\brief Looks up velocity stored with the object.
\param[in] ObjectHandle dmz::Handle of object.
\param[in] AttributeHandle Attribute handle.
\param[out] value Object velocity value.
\return Returns dmz::True if the attribute was found.

\fn dmz::Boolean dmz::ObjectModule::store_acceleration (
const dmz::Handle ObjectHandle,
const dmz::Handle AttributeHandle,
const dmz::Vector &Value)
\brief Stores acceleration with the object.
\param[in] ObjectHandle dmz::Handle of object.
\param[in] AttributeHandle Attribute handle.
\param[in] Value Object's new acceleration value.
\return Returns dmz::True if the attribute was successfully updated.

\fn dmz::Boolean dmz::ObjectModule::lookup_acceleration (
const dmz::Handle ObjectHandle,
const dmz::Handle AttributeHandle,
dmz::Vector &value)
\brief Looks up acceleration stored with the object.
\param[in] ObjectHandle dmz::Handle of object.
\param[in] AttributeHandle Attribute handle.
\param[out] value Object acceleration value.
\return Returns dmz::True if the attribute was found.

\fn dmz::Boolean dmz::ObjectModule::store_scale (
const dmz::Handle ObjectHandle,
const dmz::Handle AttributeHandle,
const dmz::Vector &Value)
\brief Stores scale with the object.
\param[in] ObjectHandle dmz::Handle of object.
\param[in] AttributeHandle Attribute handle.
\param[in] Value Object's new scale value.
\return Returns dmz::True if the attribute was successfully updated.

\fn dmz::Boolean dmz::ObjectModule::lookup_scale (
const dmz::Handle ObjectHandle,
const dmz::Handle AttributeHandle,
dmz::Vector &value)
\brief Looks up scale stored with the object.
\param[in] ObjectHandle dmz::Handle of object.
\param[in] AttributeHandle Attribute handle.
\param[out] value Object scale value.
\return Returns dmz::True if the attribute was found.

\fn dmz::Boolean dmz::ObjectModule::store_vector (
const dmz::Handle ObjectHandle,
const dmz::Handle AttributeHandle,
const dmz::Vector &Value)
\brief Stores a vector with the object.
\details The vector attribute is intended as a catch all for vector types that
aren't Position, Velocity, Acceleration, or Scale.
\param[in] ObjectHandle dmz::Handle of object.
\param[in] AttributeHandle Attribute handle.
\param[in] Value Object's new vector value.
\return Returns dmz::True if the attribute was successfully updated.

\fn dmz::Boolean dmz::ObjectModule::lookup_vector (
const dmz::Handle ObjectHandle,
const dmz::Handle AttributeHandle,
dmz::Vector &value)
\brief Looks up vector stored with the object.
\param[in] ObjectHandle dmz::Handle of object.
\param[in] AttributeHandle Attribute handle.
\param[out] value Object vector value.
\return Returns dmz::True if the attribute was found.

\fn dmz::Boolean dmz::ObjectModule::store_scalar (
const dmz::Handle ObjectHandle,
const dmz::Handle AttributeHandle,
const dmz::Float64 Value)
\brief Stores a scalar value with the object
\param[in] ObjectHandle dmz::Handle of object.
\param[in] AttributeHandle Attribute handle.
\param[in] Value Object's new scalar value.
\return Returns dmz::True if the attribute was successfully updated.

\fn dmz::Boolean dmz::ObjectModule::lookup_scalar (
const dmz::Handle ObjectHandle,
const dmz::Handle AttributeHandle,
dmz::Float64 &value)
\brief Looks up scalar stored with the object.
\param[in] ObjectHandle dmz::Handle of object.
\param[in] AttributeHandle Attribute handle.
\param[out] value Object scalar value.
\return Returns dmz::True if the attribute was found.

\fn dmz::Boolean dmz::ObjectModule::store_text (
const dmz::Handle ObjectHandle,
const dmz::Handle AttributeHandle,
const dmz::String &Value)
\brief Stores a text value with the object
\param[in] ObjectHandle dmz::Handle of object.
\param[in] AttributeHandle Attribute handle.
\param[in] Value Object's new text value.
\return Returns dmz::True if the attribute was successfully updated.

\fn dmz::Boolean dmz::ObjectModule::lookup_text (
const dmz::Handle ObjectHandle,
const dmz::Handle AttributeHandle,
dmz::String &value)
\brief Looks up text stored with the object.
\param[in] ObjectHandle dmz::Handle of object.
\param[in] AttributeHandle Attribute handle.
\param[out] value Object text value.
\return Returns dmz::True if the attribute was found.

\fn dmz::Boolean dmz::ObjectModule::store_data (
const dmz::Handle ObjectHandle,
const dmz::Handle AttributeHandle,
const dmz::Data &Value)
\brief Stores a Data value with the object
\param[in] ObjectHandle dmz::Handle of object.
\param[in] AttributeHandle Attribute handle.
\param[in] Value Object's new Data value.
\return Returns dmz::True if the attribute was successfully updated.

\fn dmz::Boolean dmz::ObjectModule::lookup_data (
const dmz::Handle ObjectHandle,
const dmz::Handle AttributeHandle,
dmz::Data &value)
\brief Looks up Data object stored with the object.
\param[in] ObjectHandle dmz::Handle of object.
\param[in] AttributeHandle Attribute handle.
\param[out] value Object Data object value.
\return Returns dmz::True if the attribute was found.


\fn dmz::ObjectModule::ObjectModule (const dmz::PluginInfo &Info);
\brief Constructor.
\param[in] Info PluginInfo containing initialization data.

\fn dmz::ObjectModule::~ObjectModule ();
\brief Destructor

*/
