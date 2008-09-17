/*!

\class dmz::EventDump
\ingroup Event
\brief Provides an interface for dumping the attributes of an event.
\sa dmz::EventModule::dump_event()


\fn dmz::EventDump::EventDump ()
\brief Constructor.

\fn dmz::EventDump::~EventDump ()
\brief Destructor.

\fn void dmz::EventDump::start_dump_event (
const Handle EventHandle,
const EventType &Type,
const EventLocalityEnum Locality)
\brief Callback to indicate the start of an event dump.
\param[in] EventHandle Event's unique runtime handle.
\param[in] Type Event's EventType.
\param[in] Locality Event's locality.

\fn void dmz::EventDump::end_dump_event (const Handle EventHandle)
\brief Callback to indicate the end of an event dump.
\param[in] EventHandle Event's unique runtime handle.

\fn void dmz::EventDump::store_event_object_handle (
const Handle EventHandle,
const Handle AttributeHandle,
const Handle Value)
\brief Stores an object handle.
\param[in] EventHandle Event's unique runtime handle.
\param[in] AttributeHandle Attribute handle.
\param[in] Value Handle for an object.

\fn void dmz::EventDump::store_event_object_type (
const Handle EventHandle,
const Handle AttributeHandle,
const ObjectType &Value)
\brief Stores and ObjectType.
\param[in] EventHandle Event's unique runtime handle.
\param[in] AttributeHandle Attribute handle.
\param[in] Value ObjectType. Usually corresponds to the object Handle passed in through
dmz::EventDump::store_event_object_type ()

\fn void dmz::EventDump::store_event_state (
const Handle EventHandle,
const Handle AttributeHandle,
const Mask &Value)
\brief Stores an event state.
\param[in] EventHandle Event's unique runtime handle.
\param[in] AttributeHandle Attribute handle.
\param[in] Value Mask contain event state.

\fn void dmz::EventDump::store_event_time_stamp (
const Handle EventHandle,
const Handle AttributeHandle,
const Float64 &Value)
\brief Stores a time stamp.
\param[in] EventHandle Event's unique runtime handle.
\param[in] AttributeHandle Attribute handle.
\param[in] Value Event's time stamp.

\fn void dmz::EventDump::store_event_position (
const Handle EventHandle,
const Handle AttributeHandle,
const Vector &Value)
\brief Stores a position.
\param[in] EventHandle Event's unique runtime handle.
\param[in] AttributeHandle Attribute handle.
\param[in] Value Event's position.

\fn void dmz::EventDump::store_event_orientation (
const Handle EventHandle,
const Handle AttributeHandle,
const Matrix &Value)
\brief Stores an orientation.
\param[in] EventHandle Event's unique runtime handle.
\param[in] AttributeHandle Attribute handle.
\param[in] Value Event's orientation.

\fn void dmz::EventDump::store_event_velocity (
const Handle EventHandle,
const Handle AttributeHandle,
const Vector &Value)
\brief Stores a velocity.
\param[in] EventHandle Event's unique runtime handle.
\param[in] AttributeHandle Attribute handle.
\param[in] Value Event's velocity.

\fn void dmz::EventDump::store_event_acceleration (
const Handle EventHandle,
const Handle AttributeHandle,
const Vector &Value)
\brief Stores an acceleration.
\param[in] EventHandle Event's unique runtime handle.
\param[in] AttributeHandle Attribute handle.
\param[in] Value Event's acceleration.

\fn void dmz::EventDump::store_event_scale (
const Handle EventHandle,
const Handle AttributeHandle,
const Vector &Value)
\brief Stores a scale.
\param[in] EventHandle Event's unique runtime handle.
\param[in] AttributeHandle Attribute handle.
\param[in] Value Event's scale.

\fn void dmz::EventDump::store_event_vector (
const Handle EventHandle,
const Handle AttributeHandle,
const Vector &Value)
\brief Stores a vector.
\param[in] EventHandle Event's unique runtime handle.
\param[in] AttributeHandle Attribute handle.
\param[in] Value A generic vector.

\fn void dmz::EventDump::store_event_scalar (
const Handle EventHandle,
const Handle AttributeHandle,
const Float64 Value)
\brief Stores a scalar.
\param[in] EventHandle Event's unique runtime handle.
\param[in] AttributeHandle Attribute handle.
\param[in] Value A generic scalar value.

\fn void dmz::EventDump::store_event_text (
const Handle EventHandle,
const Handle AttributeHandle,
const String &Value)
\brief Stores text.
\param[in] EventHandle Event's unique runtime handle.
\param[in] AttributeHandle Attribute handle.
\param[in] Value Event's text.

\fn void dmz::EventDump::store_event_data (
const Handle EventHandle,
const Handle AttributeHandle,
const Data &Value)
\brief Stores data.
\param[in] EventHandle Event's unique runtime handle.
\param[in] AttributeHandle Attribute handle.
\param[in] Value Event's Data.

*/
