/*!

\defgroup Event DMZ Event Framework

\class dmz::EventModule
\ingroup Event
\brief Event module interface.
\details The event module is a runtime database that stores attributes of
events. These events may have been generated locally or remotely.
Each event may have multiple attributes of the same type. For example an event
may have a source object and a target object.
An attribute handle is used to access the different attributes of a given type.
The event module supports both a push model for creating and closing events and a
pull model for attribute updates. The
lookup_* functions may be used to pull object attributes from the object module.
An event observer is used to push when an event is created and closed.
An event observer is registered with the event module and
may subscribe to event create and/or close. After registration, the object observer
will be notified whenever an event is created or closed.
Attribute handles are what allow multiple values to be stored in one
attribute. 
The default attribute handle is defined by the String variable
dmz::EventAttributeDefaultName.
The source attributes handle is defined by the String variable
dmz::EventAttributeSourceName.
The target attributes handle is defined by the String variable
dmz::EventAttributeTargetName.
The munitions attributes handle is defined by the String variable
dmz::EventAttributeMunitionsName.
Attribute handles are named handles allocated by the runtime.
Since attribute handles are runtime allocated, the
attribute handle value should never be hard coded as it mostly like be a different
number from run to run.
Zero is considered an invalid attribute handle.
An event's attributes can \b not be updated after the event has been closed.
\sa dmz::EventObserver \n dmz::Definitions::create_named_handle()

\fn dmz::EventModule * dmz::EventModule::cast (
const Plugin *PluginPtr,
const String &PluginName)
\brief Casts Plugin pointer to an EventModule.
\details If the Plugin object implements the EventModule interface, a pointer to
the EventModule interface of the Plugin is returned.
\param[in] PluginPtr Pointer to the Plugin to cast.
\param[in] PluginName String containing the name of the desired EventModule.
\return Returns pointer to the EventModule. Returns NULL if the PluginPtr does not
implement the EventModule interface or the \a PluginName is not empty
and not equal to the Plugin's name.

\fn dmz::EventModule::EventModule (const PluginInfo &Info)
\brief Constructor.
\param[in] Info PluginInfo containing initialization data.


\fn dmz::EventModule::~EventModule ()
\brief Destructor

\fn dmz::Boolean dmz::EventModule::register_event_observer (
const EventType &Type,
const Mask &CallbackMask,
EventObserver &observer)
\brief Registers event observer.
\details Allows an event observer to register 
for event creation and close callbacks. The same observer may be passed in multiple times
to register
for with different event types. An observer may only register for any
given event type once.
\param[in] Type EventType to register for callbacks.
\param[in] CallbackMask Mask containing the callbacks desired for subscription.
\param[in] observer EventObserver to register.
\return Returns dmz::True if the event observer was successfully registered.

\fn dmz::Boolean dmz::EventModule::release_event_observer (
const EventType &Type,
const Mask &CallbackMask,
EventObserver &observer)
\brief Release an event observer.
\details This function will only release the observer from the  defined
in the \a CallbackMask. If an observer has subscribed to create and close
callbacks and the CallbackMask only contains the create bit, the observer will
continue to receive close callbacks.
\param[in] Type EventType to register for callbacks.
\param[in] CallbackMask Mask containing the callbacks desired for subscription.
\param[in] observer EventObserver to register.
\return Returns dmz::True if the event observer was successfully registered.

\fn dmz::Boolean dmz::EventModule::release_event_observer_all (EventObserver &observer)
\brief Releases event observer from all event type subscriptions.
\param[in] observer EventObserver to be released.
\return Returns dmz::True if the observer is released from all subscriptions.

\fn void dmz::EventModule::dump_event (const Handle EventHandle, EventDump &dump)
\brief Dumps the attributes of a given event.
\details This function can be used to archive or inspect events when the event
attribute layout is not known. \sa dmz::EventPluginDump
\param[in] EventHandle Handle of the event to dump.
\param[in] dump The EventDump that the event attributes are passed to.

\fn dmz::Handle dmz::EventModule::create_event (
const EventType &Type,
const EventLocalityEnum Locality)
\brief Creates an event.
\param[in] Type The EventType of the event to create.
\param[in] Locality dmz::EventLocalityEnum specifying if the event was created locally
or remotely.
\return Returns the Handle of the created event. A zero Handle is returned if the event
creation fails.

\fn dmz::Boolean dmz::EventModule::close_event (const Handle EventHandle)
\brief Closes an open event.
\details Once an event is closed, its attributes may not be updated. Closing an event
causes both the create and close callbacks to be called. The create callback is made
before the event is actually closed so that the event may be modified. If the event
is closed from with in a create or close callback from a different event, the close
is cached and performed after the current event is completely closed and all its
callbacks have been made.
\param[in] EventHandle Handle of the event to close
\return Returns dmz::True if the event was closed or scheduled to be closed.

\fn dmz::Boolean dmz::EventModule::lookup_event_type (
const Handle EventHandle,
EventType &value)
\brief Looks up the EventType of the event.
\param[in] EventHandle Handle of the event.
\param[out] value EventType that is used to return the event type.
\return Returns dmz::True if the event type was returned.

\fn dmz::EventLocalityEnum dmz::EventModule::lookup_locality (const Handle EventHandle)
\brief Looks up event's locality.
\details The locality defines whether the event is locally or remotely.
\param[in] EventHandle dmz::Handle of event.
\return Returns dmz::EventLocal if event is local. Returns dmz::EventRemote if event
is remote. Returns dmz::ObjectLocalityUnknown if the event's handle is unknown.

\fn dmz::Boolean dmz::EventModule::store_object_handle (
const Handle EventHandle,
const Handle AttributeHandle,
const Handle Value)
\brief Stores a Handle with the event.
\details This attribute is used to associate objects with events. No validation is done
to confirm that the object handle is for a valid object.
\param[in] EventHandle dmz::Handle of event.
\param[in] AttributeHandle Attribute handle.
\param[in] Value Object's Handle.
\return Returns dmz::True if the attribute was successfully updated.

\fn dmz::Boolean dmz::EventModule::lookup_object_handle (
const Handle EventHandle,
const Handle AttributeHandle,
Handle &value)
\brief Looks up a Handle stored with the event.
\param[in] EventHandle dmz::Handle of the event.
\param[in] AttributeHandle Attribute handle.
\param[out] value Object's Handle.
\return Returns dmz::True if the attribute was found.

\fn dmz::Boolean dmz::EventModule::store_object_type (
const Handle EventHandle,
const Handle AttributeHandle,
const ObjectType &Value)
\brief Stores an ObjectType with the event.
\param[in] EventHandle dmz::Handle of event.
\param[in] AttributeHandle Attribute handle.
\param[in] Value ObjectType.
\return Returns dmz::True if the attribute was successfully updated.

\fn dmz::Boolean dmz::EventModule::lookup_object_type (
const Handle EventHandle,
const Handle AttributeHandle,
ObjectType &value)
\brief Looks up an ObjectType stored with the event.
\param[in] EventHandle dmz::Handle of the event.
\param[in] AttributeHandle Attribute handle.
\param[out] value ObjectType.
\return Returns dmz::True if the attribute was found.

\fn dmz::Boolean dmz::EventModule::store_state (
const Handle EventHandle,
const Handle AttributeHandle,
const Mask &Value)
\brief Stores a state with the event.
\param[in] EventHandle dmz::Handle of event.
\param[in] AttributeHandle Attribute handle.
\param[in] Value Mask containing the state.
\return Returns dmz::True if the attribute was successfully updated.

\fn dmz::Boolean dmz::EventModule::lookup_state (
const Handle EventHandle,
const Handle AttributeHandle,
Mask &value)
\brief Looks up a state stored with the event.
\param[in] EventHandle dmz::Handle of the event.
\param[in] AttributeHandle Attribute handle.
\param[out] value Mask containing the state.
\return Returns dmz::True if the attribute was found.

\fn dmz::Boolean dmz::EventModule::store_time_stamp (
const Handle EventHandle,
const Handle AttributeHandle,
const Float64 &Value)
\brief Stores a time stamp with the event.
\param[in] EventHandle dmz::Handle of event.
\param[in] AttributeHandle Attribute handle.
\param[in] Value Float64 containing the time stamp.
\return Returns dmz::True if the attribute was successfully updated.

\fn dmz::Boolean dmz::EventModule::lookup_time_stamp (
const Handle EventHandle,
const Handle AttributeHandle,
Float64 &value)
\brief Looks up a time stamp stored with the event.
\param[in] EventHandle dmz::Handle of the event.
\param[in] AttributeHandle Attribute handle.
\param[in] value Float64 containing the time stamp.
\return Returns dmz::True if the attribute was found.

\fn dmz::Boolean dmz::EventModule::store_position (
const Handle EventHandle,
const Handle AttributeHandle,
const Vector &Value)
\brief Stores a position with the event.
\param[in] EventHandle dmz::Handle of event.
\param[in] AttributeHandle Attribute handle.
\param[in] Value Vector containing the position.
\return Returns dmz::True if the attribute was successfully updated.

\fn dmz::Boolean dmz::EventModule::lookup_position (
const Handle EventHandle,
const Handle AttributeHandle,
Vector &value)
\brief Looks up a position stored with the event.
\param[in] EventHandle dmz::Handle of the event.
\param[in] AttributeHandle Attribute handle.
\param[in] value Vector containing the position.
\return Returns dmz::True if the attribute was found.

\fn dmz::Boolean dmz::EventModule::store_orientation (
const Handle EventHandle,
const Handle AttributeHandle,
const Matrix &Value)
\brief Stores an orientation with the event.
\param[in] EventHandle dmz::Handle of event.
\param[in] AttributeHandle Attribute handle.
\param[in] Value Matrix containing the orientation.
\return Returns dmz::True if the attribute was successfully updated.

\fn dmz::Boolean dmz::EventModule::lookup_orientation (
const Handle EventHandle,
const Handle AttributeHandle,
Matrix &value)
\brief Looks up an orientation stored with the event.
\param[in] EventHandle dmz::Handle of the event.
\param[in] AttributeHandle Attribute handle.
\param[in] value Matrix containing the orientation.
\return Returns dmz::True if the attribute was found.

\fn dmz::Boolean dmz::EventModule::store_velocity (
const Handle EventHandle,
const Handle AttributeHandle,
const Vector &Value)
\brief Stores a velocity with the event.
\param[in] EventHandle dmz::Handle of event.
\param[in] AttributeHandle Attribute handle.
\param[in] Value Vector containing the velocity.
\return Returns dmz::True if the attribute was successfully updated.

\fn dmz::Boolean dmz::EventModule::lookup_velocity (
const Handle EventHandle,
const Handle AttributeHandle,
Vector &value)
\brief Looks up a velocity stored with the event.
\param[in] EventHandle dmz::Handle of the event.
\param[in] AttributeHandle Attribute handle.
\param[in] value Vector containing the velocity.
\return Returns dmz::True if the attribute was found.

\fn dmz::Boolean dmz::EventModule::store_acceleration (
const Handle EventHandle,
const Handle AttributeHandle,
const Vector &Value)
\brief Stores an accleration with the event.
\param[in] EventHandle dmz::Handle of event.
\param[in] AttributeHandle Attribute handle.
\param[in] Value Vector containing the acceleration.
\return Returns dmz::True if the attribute was successfully updated.

\fn dmz::Boolean dmz::EventModule::lookup_acceleration (
const Handle EventHandle,
const Handle AttributeHandle,
Vector &value)
\brief Looks up an acceleration stored with the event.
\param[in] EventHandle dmz::Handle of the event.
\param[in] AttributeHandle Attribute handle.
\param[in] value Vector containing the acceleration.
\return Returns dmz::True if the attribute was found.

\fn dmz::Boolean dmz::EventModule::store_scale (
const Handle EventHandle,
const Handle AttributeHandle,
const Vector &Value)
\brief Stores a scale with the event.
\param[in] EventHandle dmz::Handle of event.
\param[in] AttributeHandle Attribute handle.
\param[in] Value Vector containing the scale.
\return Returns dmz::True if the attribute was successfully updated.

\fn dmz::Boolean dmz::EventModule::lookup_scale (
const Handle EventHandle,
const Handle AttributeHandle,
Vector &value)
\brief Looks up a scale stored with the event.
\param[in] EventHandle dmz::Handle of the event.
\param[in] AttributeHandle Attribute handle.
\param[in] value Vector containing the scale.
\return Returns dmz::True if the attribute was found.

\fn dmz::Boolean dmz::EventModule::store_vector (
const Handle EventHandle,
const Handle AttributeHandle,
const Vector &Value)
\brief Stores a generic vector with the event.
\param[in] EventHandle dmz::Handle of event.
\param[in] AttributeHandle Attribute handle.
\param[in] Value Vector containing the generic vector.
\return Returns dmz::True if the attribute was successfully updated.

\fn dmz::Boolean dmz::EventModule::lookup_vector (
const Handle EventHandle,
const Handle AttributeHandle,
Vector &value)
\brief Looks up a generic vector stored with the event.
\param[in] EventHandle dmz::Handle of the event.
\param[in] AttributeHandle Attribute handle.
\param[in] value Vector containing the generic vector.
\return Returns dmz::True if the attribute was found.

\fn dmz::Boolean dmz::EventModule::store_scalar (
const Handle EventHandle,
const Handle AttributeHandle,
const Float64 Value)
\brief Stores a scalar with the event.
\param[in] EventHandle dmz::Handle of event.
\param[in] AttributeHandle Attribute handle.
\param[in] Value Float64 containing the scalar.
\return Returns dmz::True if the attribute was successfully updated.

\fn dmz::Boolean dmz::EventModule::lookup_scalar (
const Handle EventHandle,
const Handle AttributeHandle,
Float64 &value)
\brief Looks up a scalar stored with the event.
\param[in] EventHandle dmz::Handle of the event.
\param[in] AttributeHandle Attribute handle.
\param[in] value Float64 containing the scalar.
\return Returns dmz::True if the attribute was found.

\fn dmz::Boolean dmz::EventModule::store_text (
const Handle EventHandle,
const Handle AttributeHandle,
const String &Value)
\brief Stores text with the event.
\param[in] EventHandle dmz::Handle of event.
\param[in] AttributeHandle Attribute handle.
\param[in] Value String containing the text.
\return Returns dmz::True if the attribute was successfully updated.

\fn dmz::Boolean dmz::EventModule::lookup_text (
const Handle EventHandle,
const Handle AttributeHandle,
String &value)
\brief Looks up text stored with the event.
\param[in] EventHandle dmz::Handle of the event.
\param[in] AttributeHandle Attribute handle.
\param[in] value String containing the text.
\return Returns dmz::True if the attribute was found.

\fn dmz::Boolean dmz::EventModule::store_data (
const Handle EventHandle,
const Handle AttributeHandle,
const Data &Value)
\brief Stores data with the event.
\param[in] EventHandle dmz::Handle of event.
\param[in] AttributeHandle Attribute handle.
\param[in] Value Data object containing the data.
\return Returns dmz::True if the attribute was successfully updated.

\fn dmz::Boolean dmz::EventModule::lookup_data (
const Handle EventHandle,
const Handle AttributeHandle,
Data &value)
\brief Looks up data stored with the event.
\param[in] EventHandle dmz::Handle of the event.
\param[in] AttributeHandle Attribute handle.
\param[in] value Data object containing the data.
\return Returns dmz::True if the attribute was found.

*/
