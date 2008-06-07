/*!

\class dmz::ObjectObserver
\ingroup Object
\brief Observes object attribute updates.
\details A pure virtual interface for observing attribute updates to object stored
in the ObjectModule. This class is only an interface. Derived classes should use the
dmz::ObjectObserverUtil class.
\sa dmz::ObjectObserverUtil

\fn dmz::ObjectObserver::ObjectObserver (const PluginInfo &Info)
\brief Constructor.
\param[in] Info PluginInfo containing initialization data.

\fn dmz::ObjectObserver::~ObjectObserver ()
\brief Destructor.

\fn dmz::ObjectObserver *dmz::ObjectObserver::cast (
const Plugin *PluginPtr,
const String &PluginName);
\brief Casts Plugin pointer to an ObjectObserver pointer.
\details If the Plugin object implements the ObjectObserver interface,
a pointer to the ObjectObserver interface of the Plugin is returned.
\param[in] PluginPtr Pointer to the Plugin to cast.
\param[in] PluginName String containing the name of the desired ObjectObserver.
\return Returns pointer to the ObjectObserver. Return NULL if the PluginPtr does not
implement the ObjectObserver interface or the \a PluginName is not empty
and not equal to the Plugin's name.

\fn dmz::Boolean dmz::ObjectObserver::is_valid (
const Handle ObserverHandle,
RuntimeContext *context)
\brief Tests if runtime handle belongs to a valid object observer interface.
\param[in] ObserverHandle Unique runtime handle.
\param[in] context Pointer to the runtime context.
\return Returns dmz::True if the runtime handle is valid and the Plugin supports the
object observer interface.

\fn dmz::String dmz::ObjectObserver::get_object_observer_name ()
\brief Gets object observer's name.
\return Returns String containing object observer's name.

\fn dmz::Handle dmz::ObjectObserver::get_object_observer_handle ()
\brief Gets object observers unique runtime handle.
\return Returns unique runtime handle.

\fn void dmz::ObjectObserver::store_object_module (
const String &Name,
ObjectModule &module)
\brief Stores object module in observer.
\param[in] Name String containing name of the module.
\param[in] module ObjectModule reference.

\fn void dmz::ObjectObserver::remove_object_module (
const String &Name,
ObjectModule &module)
\brief Removes object module in observer.
\param[in] Name String containing name of the module.
\param[in] module ObjectModule reference.

\fn void dmz::ObjectObserver::create_object (
const UUID &Identity,
const Handle ObjectHandle,
const ObjectType &Type,
const ObjectLocalityEnum Locality)
\brief Object creation callback.
\param[in] Identity Object's UUID.
\param[in] ObjectHandle Object's unique runtime handle.
\param[in] Type Object's ObjectType.
\param[in] Locality Object's locality.

\fn void dmz::ObjectObserver::destroy_object (
const UUID &Identity,
const Handle ObjectHandle)
\brief Destroy object callback.
\param[in] Identity Object's UUID.
\param[in] ObjectHandle Object's unique runtime handle.

\fn void dmz::ObjectObserver::update_object_uuid (
const Handle ObjectHandle,
const UUID &Identity,
const UUID &PrevIdentity)
\brief Update object UUID callback.
\param[in] ObjectHandle Object's unique runtime handle.
\param[in] Identity Object's current UUID.
\param[in] PrevIdentity Object's previous UUID.

\fn void dmz::ObjectObserver::remove_object_attribute (
const UUID &Identity,
const Handle ObjectHandle,
const Handle AttributeHandle,
const Mask &AttrMask)
\brief Remove object attribute callback.
\param[in] Identity Object's UUID.
\param[in] ObjectHandle Object's unique runtime handle.
\param[in] AttributeHandle Attribute handle.
\param[in] AttrMask Mask containing attributes being removed.

\fn void dmz::ObjectObserver::update_object_locality (
const UUID &Identity,
const Handle ObjectHandle,
const ObjectLocalityEnum Locality,
const ObjectLocalityEnum PrevLocality)
\brief Update object locality callback.
\param[in] Identity Object's UUID.
\param[in] ObjectHandle Object's unique runtime handle.
\param[in] Locality Object's current locality.
\param[in] PrevLocality Object's previous locality.

\fn void dmz::ObjectObserver::link_objects (
const Handle LinkHandle,
const Handle AttributeHandle,
const UUID &SuperIdentity,
const Handle SuperHandle,
const UUID &SubIdentity,
const Handle SubHandle)
\brief Link objects callback.
\param[in] LinkHandle Link handle.
\param[in] AttributeHandle Attribute handle.
\param[in] SuperIdentity Super object's UUID.
\param[in] SuperHandle Super object's unique runtime handle.
\param[in] SubIdentity Sub object's UUID.
\param[in] SubHandle Sub object's unique runtime handle.

\fn void dmz::ObjectObserver::unlink_objects (
const Handle LinkHandle,
const Handle AttributeHandle,
const UUID &SuperIdentity,
const Handle SuperHandle,
const UUID &SubIdentity,
const Handle SubHandle)
\brief Unlink objects callback.
\param[in] LinkHandle Link handle.
\param[in] AttributeHandle Attribute handle.
\param[in] SuperIdentity Super object's UUID.
\param[in] SuperHandle Super object's unique runtime handle.
\param[in] SubIdentity Sub object's UUID.
\param[in] SubHandle Sub object's unique runtime handle.

\fn void dmz::ObjectObserver::update_link_attribute_object (
const Handle LinkHandle,
const Handle AttributeHandle,
const UUID &SuperIdentity,
const Handle SuperHandle,
const UUID &SubIdentity,
const Handle SubHandle,
const UUID &AttributeIdentity,
const Handle AttributeObjectHandle,
const UUID &PrevAttributeIdentity,
const Handle PrevAttributeObjectHandle)
\brief Attach attribute object to a link callback.
\param[in] LinkHandle Link handle.
\param[in] AttributeHandle Attribute handle.
\param[in] SuperIdentity Super object's UUID.
\param[in] SuperHandle Super object's unique runtime handle.
\param[in] SubIdentity Sub object's UUID.
\param[in] SubHandle Sub object's unique runtime handle.
\param[in] AttributeIdentity Attribute object's UUID.
\param[in] AttributeObjectHandle Attribute object's unique runtime handle.
\param[in] PrevAttributeIdentity Attribute object's previous UUID.
\param[in] PrevAttributeObjectHandle Attribute object's previous unique runtime handle.

\fn void dmz::ObjectObserver::update_object_type (
const UUID &Identity,
const Handle ObjectHandle,
const Handle AttributeHandle,
const ObjectType &Value,
const ObjectType *PreviousValue)
\brief Update object type callback.
\param[in] Identity Object's UUID.
\param[in] ObjectHandle Objects unique runtime handle.
\param[in] AttributeHandle Attribute handle.
\param[in] Value Current object type.
\param[in] PreviousValue Pointer to previous object type.
\note \a PreviousValue will be NULL if the there is no previous value.

\fn void dmz::ObjectObserver::update_object_state (
const UUID &Identity,
const Handle ObjectHandle,
const Handle AttributeHandle,
const Mask &Value,
const Mask *PreviousValue)
\brief Update object state callback.
\param[in] Identity Object's UUID.
\param[in] ObjectHandle Objects unique runtime handle.
\param[in] AttributeHandle Attribute handle.
\param[in] Value Current object state.
\param[in] PreviousValue Pointer to previous object state.
\note \a PreviousValue will be NULL if the there is no previous value.

\fn void dmz::ObjectObserver::update_object_flag (
const UUID &Identity,
const Handle ObjectHandle,
const Handle AttributeHandle,
const Boolean Value,
const Boolean *PreviousValue)
\brief Update object flag callback.
\param[in] Identity Object's UUID.
\param[in] ObjectHandle Objects unique runtime handle.
\param[in] AttributeHandle Attribute handle.
\param[in] Value Current object flag.
\param[in] PreviousValue Pointer to previous object flag.
\note \a PreviousValue will be NULL if the there is no previous value.

\fn void dmz::ObjectObserver::update_object_time_stamp (
const UUID &Identity,
const Handle ObjectHandle,
const Handle AttributeHandle,
const Float64 Value,
const Float64 *PreviousValue)
\brief Update object time stamp callback.
\param[in] Identity Object's UUID.
\param[in] ObjectHandle Objects unique runtime handle.
\param[in] AttributeHandle Attribute handle.
\param[in] Value Current object time stamp.
\param[in] PreviousValue Pointer to previous object time stamp.
\note \a PreviousValue will be NULL if the there is no previous value.

\fn void dmz::ObjectObserver::update_object_position (
const UUID &Identity,
const Handle ObjectHandle,
const Handle AttributeHandle,
const Vector &Value,
const Vector *PreviousValue)
\brief Update object position callback.
\param[in] Identity Object's UUID.
\param[in] ObjectHandle Objects unique runtime handle.
\param[in] AttributeHandle Attribute handle.
\param[in] Value Current object position.
\param[in] PreviousValue Pointer to previous object position.
\note \a PreviousValue will be NULL if the there is no previous value.

\fn void dmz::ObjectObserver::update_object_orientation (
const UUID &Identity,
const Handle ObjectHandle,
const Handle AttributeHandle,
const Matrix &Value,
const Matrix *PreviousValue)
\brief Update object orientation callback.
\param[in] Identity Object's UUID.
\param[in] ObjectHandle Objects unique runtime handle.
\param[in] AttributeHandle Attribute handle.
\param[in] Value Current object orientation.
\param[in] PreviousValue Pointer to previous object orientation.
\note \a PreviousValue will be NULL if the there is no previous value.

\fn void dmz::ObjectObserver::update_object_velocity (
const UUID &Identity,
const Handle ObjectHandle,
const Handle AttributeHandle,
const Vector &Value,
const Vector *PreviousValue)
\brief Update object velocity callback.
\param[in] Identity Object's UUID.
\param[in] ObjectHandle Objects unique runtime handle.
\param[in] AttributeHandle Attribute handle.
\param[in] Value Current object velocity 
\param[in] PreviousValue Pointer to previous object velocity 
\note \a PreviousValue will be NULL if the there is no previous value.

\fn void dmz::ObjectObserver::update_object_acceleration (
const UUID &Identity,
const Handle ObjectHandle,
const Handle AttributeHandle,
const Vector &Value,
const Vector *PreviousValue)
\brief Update object acceleration callback.
\param[in] Identity Object's UUID.
\param[in] ObjectHandle Objects unique runtime handle.
\param[in] AttributeHandle Attribute handle.
\param[in] Value Current object acceleration 
\param[in] PreviousValue Pointer to previous object acceleration 
\note \a PreviousValue will be NULL if the there is no previous value.

\fn void dmz::ObjectObserver::update_object_scale (
const UUID &Identity,
const Handle ObjectHandle,
const Handle AttributeHandle,
const Vector &Value,
const Vector *PreviousValue)
\brief Update object scale callback.
\param[in] Identity Object's UUID.
\param[in] ObjectHandle Objects unique runtime handle.
\param[in] AttributeHandle Attribute handle.
\param[in] Value Current object scale 
\param[in] PreviousValue Pointer to previous object scale 
\note \a PreviousValue will be NULL if the there is no previous value.

\fn void dmz::ObjectObserver::update_object_vector (
const UUID &Identity,
const Handle ObjectHandle,
const Handle AttributeHandle,
const Vector &Value,
const Vector *PreviousValue)
\brief Update object vector callback.
\param[in] Identity Object's UUID.
\param[in] ObjectHandle Objects unique runtime handle.
\param[in] AttributeHandle Attribute handle.
\param[in] Value Current object vector 
\param[in] PreviousValue Pointer to previous object vector 
\note \a PreviousValue will be NULL if the there is no previous value.

\fn void dmz::ObjectObserver::update_object_scalar (
const UUID &Identity,
const Handle ObjectHandle,
const Handle AttributeHandle,
const Float64 Value,
const Float64 *PreviousValue)
\brief Update object scalar callback.
\param[in] Identity Object's UUID.
\param[in] ObjectHandle Objects unique runtime handle.
\param[in] AttributeHandle Attribute handle.
\param[in] Value Current object scalar 
\param[in] PreviousValue Pointer to previous object scalar 
\note \a PreviousValue will be NULL if the there is no previous value.

\fn void dmz::ObjectObserver::update_object_text (
const UUID &Identity,
const Handle ObjectHandle,
const Handle AttributeHandle,
const String &Value,
const String *PreviousValue)
\brief Update object text callback.
\param[in] Identity Object's UUID.
\param[in] ObjectHandle Objects unique runtime handle.
\param[in] AttributeHandle Attribute handle.
\param[in] Value Current object text 
\param[in] PreviousValue Pointer to previous object text 
\note \a PreviousValue will be NULL if the there is no previous value.

\fn void dmz::ObjectObserver::update_object_data (
const UUID &Identity,
const Handle ObjectHandle,
const Handle AttributeHandle,
const Data &Value,
const Data *PreviousValue)
\brief Update object Data callback.
\param[in] Identity Object's UUID.
\param[in] ObjectHandle Objects unique runtime handle.
\param[in] AttributeHandle Attribute handle.
\param[in] Value Current object Data 
\param[in] PreviousValue Pointer to previous object Data 
\note \a PreviousValue will be NULL if the there is no previous value.

*/
